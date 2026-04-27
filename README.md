# Multithreaded Order Matching Engine (C++ + Python Dashboard)

## Overview

This project implements a multithreaded order matching engine simulating core exchange mechanics used in financial markets.

## Features

* FIFO (price-time priority) matching engine
* Multithreaded producer-consumer architecture
* Thread-safe queue using mutex and condition variables
* Latency benchmarking (microseconds level)
* Python dashboard using Streamlit for visualization

## Tech Stack

* C++
* Python (Streamlit, Pandas, Matplotlib)

## Dashboard

The dashboard visualizes:

* Latency over time
* Trade flow
* Latency distribution

## How to Run

### C++ Engine

```bash
g++ -std=c++17 order_book.cpp -o order_book
./order_book
```

### Dashboard

```bash
pip install -r requirements.txt
streamlit run app.py
```

## Learnings

* Market microstructure fundamentals
* Multithreading and synchronization
* Performance analysis and latency measurement

## Author

Hamd
