# 🌸 Flower Trade - High-Performance Trading Platform

A high-performance, full-stack trading platform built with a C++ matching engine, a Node.js WebSocket bridge, and a React frontend. The system processes real-time order matching, supports manual order entry, and features bulk CSV order processing with automated engine memory management.

## 📋 Table of Contents

- [Architecture](#-architecture-monorepo)
- [Prerequisites](#-prerequisites)
- [Setup & Installation](#-setup--installation)
- [Compiling the C++ Engine](#-compiling-the-c-engine)
- [Running the Application](#-running-the-application)
- [How to Use the Platform](#-how-to-use-the-platform)

## 🏗️ Architecture (Monorepo)

The project is structured as a monorepo containing the backend engine and the frontend UI:

```text
📦 Master-Project-Folder
 ┣ 📂 Flower-Trade          # C++ Matching Engine (TCP Server)
 ┃ ┣ 📜 main.cpp
 ┃ ┣ 📜 ExchangeServer.cpp
 ┃ ┣ 📜 OrderBook.cpp
 ┃ ┗ 📜 ... (other C++ headers and files)
 ┣ 📂 Trader-UI             # Middleware & Frontend
 ┃ ┣ 📂 frontend            # React UI Dashboard (Vite)
 ┃ ┣ 📜 bridge.js           # Node.js TCP-to-WebSocket Bridge
 ┃ ┗ 📜 package.json        
 ┣ 📜 package.json          # Root package for 1-click start
 ┗ 📜 README.md
```

## 📋 Prerequisites

Before running the project, ensure you have the following installed:

- **Node.js & npm** — For the React UI and Node Bridge
- **C++ Compiler (MinGW/GCC)** — For compiling the matching engine
- **Git** — For version control

## 🚀 Setup & Installation

### 1. Clone the Repository

```bash
git clone <your-repo-url>
cd <your-master-folder>
```

### 2. Install Root Dependencies

This installs concurrently to run everything together.

```bash
npm install
```

### 3. Install Frontend Dependencies

Navigate to the React folder and install the UI packages.

```bash
cd Trader-UI/frontend
npm install
cd ../..
```

## 🛠️ Compiling the C++ Engine

Whenever you make changes to the C++ code, you must recompile the engine.

1. Open a terminal and navigate to the Flower-Trade folder:

```bash
cd Flower-Trade
```

2. Run the compilation command (includes the Windows socket library -lws2_32):

```bash
g++ main.cpp ExchangeServer.cpp OrderBook.cpp Order.cpp ExecutionReport.cpp -o ExchangeServer.exe -lws2_32
```

3. Return to the root folder:

```bash
cd ..
```

## 🏃‍♂️ Running the Application

You do not need to open multiple terminals. Thanks to the root package.json scripts, you can launch the entire stack (C++ Engine, Node Bridge, and React UI) with a single command.

From the root master folder, run:

```bash
npm run dev
```

### Service Ports

- **React UI** — http://localhost:5173 (automatically opens in your browser)
- **C++ Engine** — TCP port 8080
- **Node Bridge** — port 3001

## 📖 How to Use the Platform

### 1. Manual Order Entry

Use the Manual Order Entry card on the left side of the dashboard.

- Input a Client ID
- Select an Instrument (e.g., Rose, Orchid)
- Choose Buy/Sell
- Set your Quantity and Price
- Click Submit

The execution report will instantly appear in the live table.

### 2. Bulk CSV Upload

You can upload a massive batch of orders using the Bulk Order Upload button.

**Important:** Uploading a CSV automatically triggers a RESET command. The UI will clear, the C++ engine's memory will wipe, and the output.csv file will be overwritten to provide a clean test scenario.

#### Expected Input CSV Format

```csv
Client ID,Instrument,Side,Quantity,Price
user_1,Rose,1,100,55.00
user_2,Rose,2,50,54.00
user_3,Orchid,1,200,105.50
```

**Note:** Side 1 is Buy, Side 2 is Sell

### 3. Exporting Results

The C++ engine automatically writes all execution reports safely to `Flower-Trade/output.csv` on your hard drive.

You can also click the Download output.csv button in the UI to instantly download the results directly from your browser.