# Institutional Flower Exchange Platform

**LSEG Workshop Final Project - C++ Matching Engine & React UI**

This project is a high-performance, multithreaded order matching engine built in C++. It simulates a real-world financial exchange for trading flower commodities (Rose, Tulip, Orchid, Lotus, Lavender) and includes a full-stack web interface for live trading simulation.

To demonstrate both high-throughput batch processing and low-latency network trading, this project features a **Decoupled Dual-Architecture**, allowing the engine to run in two distinct modes.

---

## ✨ Core Features

*   **High-Performance Matching Logic:** The central matching engine (`Exchange` and `OrderBook`) is entirely decoupled from the I/O layers.
*   **$O(1)$ Best-Price Lookups:** The Buy-Side order book utilizes a Red-Black Tree (`std::map` with a custom `std::greater<double>` comparator) to mathematically guarantee the highest bid is always instantly accessible.
*   **Thread-Safe & Concurrent:** The engine uses `std::mutex` and `std::lock_guard` to prevent race conditions during concurrent network order processing.
*   **Modern C++ Practices:** Employs `std::unique_ptr` for strict, leak-free memory management and follows SOLID principles for a clean, decoupled design.
*   **Live Trading Dashboard:** A React frontend communicates via WebSockets to a Node.js bridge, which then streams raw TCP packets into the C++ engine for a real-world HFT simulation.

---

## 📂 Project Structure

```
📁 LSEG Final Project/
├── 📁 Flower-Trade/            # The C++ Core Engine
│   ├── Exchange.cpp / .h       # Engine routing and initialization
│   ├── OrderBook.cpp / .h      # The Red-Black tree matching logic
│   ├── main.cpp                # Entry point for Mode 1 (CLI)
│   ├── main_server.cpp         # Entry point for Mode 2 (TCP GUI Server)
│   └── ... (Data models & interfaces)
├── 📁 Trader-UI/               # The Web Interface
│   ├── bridge.js               # Node.js WebSocket-to-TCP translator
│   └── 📁 frontend/            # React dashboard source code
├── package.json                # Master startup script (npm run dev)
└── README.md                   # This file
```

---

## 🛠️ Prerequisites

Before you begin, ensure you have the following installed:
*   **C++ Compiler:** `g++` (MinGW on Windows) or any other standard C++ compiler.
*   **Node.js:** Required for the Trader UI and the WebSocket-to-TCP bridge.

---

## ⚙️ Setup & Compilation

### 1. C++ Engine Compilation

Open your terminal in the `Flower-Trade` directory and run the following commands to build the two required executables.

*   **Build the CLI Executable (for automated grading):**
    ```bash
    g++ main.cpp Exchange.cpp OrderBook.cpp Order.cpp ExecutionReport.cpp CSVReader.cpp CSVWriter.cpp OrderValidator.cpp -o flower_cli.exe
    ```

*   **Build the GUI TCP Server (for live network simulation):**
    ```bash
    g++ main_server.cpp Exchange.cpp OrderBook.cpp Order.cpp ExecutionReport.cpp OrderValidator.cpp -o flower_gui.exe -lws2_32
    ```
    *(Note: The `-lws2_32` flag links the Windows Socket API, which is required for the TCP server).*

### 2. Web Interface Setup

There are three separate `package.json` files that need their dependencies installed. Run these commands from the **main project root** (`LSEG final project`).

*   **Install Master Controller Dependencies:**
    ```bash
    npm install concurrently
    ```

*   **Install Node.js Bridge Dependencies:**
    ```bash
    cd Trader-UI
    npm install express socket.io cors
    cd ..
    ```

*   **Install React Frontend Dependencies:**
    ```bash
    cd Trader-UI/frontend
    npm install
    cd ../..
    ```

---

## 🚀 Running the Project

You can run the engine in two different modes.

### Mode 1: Automated Evaluation (CLI)

This mode is designed for high-throughput batch processing and is used for the automated LSEG grading scripts. It bypasses all networking for maximum performance.

1.  Place any order CSV file (e.g., `my_orders.csv`) into the `Flower-Trade` directory.
2.  Run the CLI executable, passing the name of your file as a command-line argument:
    ```bash
    cd Flower-Trade
    ./flower_cli.exe my_orders.csv
    ```
The engine will process the batch file and generate a corresponding `my_orders_execution_rep.csv` in the same folder. You can do this for any number of files.

### Mode 2: Live Trading Simulation (GUI)

This mode demonstrates a real-world High-Frequency Trading (HFT) environment. It boots the C++ TCP Server, the Node.js WebSocket bridge, and the React web dashboard all at once.

1.  Navigate to the project's root directory (`LSEG final project`).
2.  Boot the entire stack with a single command:
    ```bash
    npm run dev
    ```
A browser window will open automatically with the trading dashboard. You can now submit manual orders or upload CSV files directly through the interface. All trades will be logged live in the UI and the `execution_rep.csv` file simultaneously.