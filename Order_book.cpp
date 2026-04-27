#include <iostream>
#include <map>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <fstream>   // ✅ added

using namespace std;

struct Order {
    int id;
    string side;
    int price;
    int quantity;
};

// -------- GLOBAL STATS --------
long long total_latency = 0;
long long min_latency = LLONG_MAX;
long long max_latency = 0;
int processed_orders = 0;
int total_trades = 0;

ofstream logFile("output.csv");   // ✅ file for Python

// -------- THREAD SAFE QUEUE --------
class SafeQueue {
private:
    queue<Order> q;
    mutex mtx;
    condition_variable cv;

public:
    void push(Order order) {
        unique_lock<mutex> lock(mtx);
        q.push(order);
        cv.notify_one();
    }

    Order pop() {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [&]{ return !q.empty(); });

        Order order = q.front();
        q.pop();
        return order;
    }
};

// -------- ORDER BOOK --------
class OrderBook {
public:
    map<int, queue<Order>, greater<int>> bids;
    map<int, queue<Order>> asks;

    mutex ob_mutex;

    void processOrder(Order order) {
        lock_guard<mutex> lock(ob_mutex);

        if (order.side == "buy") {
            bids[order.price].push(order);
        } else {
            asks[order.price].push(order);
        }

        match();
    }

    void match() {
        while (!bids.empty() && !asks.empty()) {
            auto bestBid = bids.begin();
            auto bestAsk = asks.begin();

            if (bestBid->first >= bestAsk->first) {
                auto &buyQ = bestBid->second;
                auto &sellQ = bestAsk->second;

                Order &buy = buyQ.front();
                Order &sell = sellQ.front();

                int tradeQty = min(buy.quantity, sell.quantity);

                cout << "Trade: " << tradeQty << " @ " << bestAsk->first << endl;

                total_trades++;

                buy.quantity -= tradeQty;
                sell.quantity -= tradeQty;

                if (buy.quantity == 0) buyQ.pop();
                if (sell.quantity == 0) sellQ.pop();

                if (buyQ.empty()) bids.erase(bestBid);
                if (sellQ.empty()) asks.erase(bestAsk);
            } else break;
        }
    }
};

SafeQueue orderQueue;
OrderBook ob;

// -------- PRODUCER --------
void producer() {
    for (int i = 0; i < 20; i++) {
        Order o = {i, (i % 2 == 0 ? "buy" : "sell"), 100 + (i % 3), 5 + i};
        orderQueue.push(o);
    }
}

// -------- CONSUMER --------
void consumer() {
    for (int i = 0; i < 20; i++) {
        auto start = chrono::high_resolution_clock::now();

        Order o = orderQueue.pop();
        ob.processOrder(o);

        auto end = chrono::high_resolution_clock::now();

        long long latency = chrono::duration_cast<chrono::microseconds>(end - start).count();

        cout << "Latency: " << latency << " microseconds\n";

        // ✅ Write to CSV
        logFile << latency << "," << total_trades << "\n";

        total_latency += latency;
        min_latency = min(min_latency, latency);
        max_latency = max(max_latency, latency);
        processed_orders++;
    }
}

// -------- MAIN --------
int main() {
    thread t1(producer);
    thread t2(consumer);

    t1.join();
    t2.join();

    logFile.close();   // ✅ important

    cout << "\n===== PERFORMANCE STATS =====\n";
    cout << "Total Orders: " << processed_orders << endl;
    cout << "Total Trades: " << total_trades << endl;

    cout << "Avg Latency: " << total_latency / processed_orders << endl;
    cout << "Min Latency: " << min_latency << endl;
    cout << "Max Latency: " << max_latency << endl;

    return 0;
}