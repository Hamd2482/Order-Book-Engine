import streamlit as st
import pandas as pd
import matplotlib.pyplot as plt

# ---------------- CONFIG ----------------
st.set_page_config(page_title="Order Book Dashboard", layout="wide")

st.title("⚡ Order Matching Engine Dashboard")

# ---------------- LOAD DATA ----------------
df = pd.read_csv("output.csv", names=["latency", "trades"])

# ---------------- KPI METRICS ----------------
col1, col2, col3 = st.columns(3)

col1.metric("Total Orders", len(df))
col2.metric("Total Trades", int(df["trades"].max()))
col3.metric("Avg Latency (µs)", int(df["latency"].mean()))

st.markdown("---")

# ---------------- GRAPHS (SIDE BY SIDE) ----------------
col1, col2 = st.columns(2)

with col1:
    st.subheader("📊 Latency")
    fig1, ax1 = plt.subplots(figsize=(5, 3))
    ax1.plot(df["latency"], linewidth=2)
    ax1.set_xlabel("Order")
    ax1.set_ylabel("Latency (µs)")
    ax1.grid()
    st.pyplot(fig1)

with col2:
    st.subheader("📈 Trades")
    fig2, ax2 = plt.subplots(figsize=(5, 3))
    ax2.plot(df["trades"], linewidth=2)
    ax2.set_xlabel("Order")
    ax2.set_ylabel("Trades")
    ax2.grid()
    st.pyplot(fig2)

# ---------------- SMALL CENTERED HISTOGRAM ----------------
st.subheader("📉 Latency Distribution")

left, center, right = st.columns([1, 2, 1])

with center:
    fig3, ax3 = plt.subplots(figsize=(4, 2))  # small size
    ax3.hist(df["latency"], bins=15)
    ax3.set_xlabel("Latency (µs)")
    ax3.set_ylabel("Freq")
    ax3.grid()
    st.pyplot(fig3)

# ---------------- RAW DATA ----------------
with st.expander("🔍 View Raw Data"):
    st.dataframe(df)