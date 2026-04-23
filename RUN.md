# How to Run This Project

This project has two main components: a C++ matching engine and a React-based trading UI. You can run it in two different modes.

---

## Mode 1: Automated Evaluation (CLI)

This mode is for processing order files in batches from the command line.

1.  **Compile the C++ engine:**
    Open a terminal in the `Flower-Trade` directory and run:
    ```bash
    g++ main.cpp Exchange.cpp OrderBook.cpp Order.cpp ExecutionReport.cpp CSVReader.cpp CSVWriter.cpp OrderValidator.cpp -o flower_cli.exe
    ```

2.  **Run the engine:**
    From the `Flower-Trade` directory, run the executable with the name of an order file. For example:
    ```bash
    ./flower_cli.exe Example1.csv
    ```
    This will generate an execution report file named `Example1_execution_rep.csv`.

---

## Mode 2: Live Trading Simulation (GUI)

This mode runs a full stack with a C++ TCP server, a Node.js bridge, and a React frontend for live trading.

1.  **Compile the C++ TCP Server:**
    In the `Flower-Trade` directory, run:
    ```bash
    g++ main_server.cpp Exchange.cpp OrderBook.cpp Order.cpp ExecutionReport.cpp OrderValidator.cpp -o flower_gui.exe -lws2_32
    ```

2.  **Install JavaScript dependencies:**
    Run these commands from the project's root directory (`LSEG final project`):
    ```bash
    npm install concurrently
    cd Trader-UI
    npm install express socket.io cors
    cd frontend
    npm install
    cd ../..
    ```

3.  **Run the full stack:**
    From the root directory, simply run:
    ```bash
    npm run dev
    ```
    This will start all the necessary components and open the trading dashboard in your browser.
