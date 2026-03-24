const net = require('net');
const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const cors = require('cors');

const app = express();
app.use(cors());
const server = http.createServer(app);
const io = new Server(server, { cors: { origin: "*" } });

// 1. Connect to the C++ Exchange Server
const tcpClient = new net.Socket();
tcpClient.connect(8080, '127.0.0.1', () => {
    console.log('Connected to C++ Exchange Engine on TCP 8080');
});

// 2. Receive Receipts from C++ and forward to React
tcpClient.on('data', (data) => {
    const packets = data.toString().trim().split('\n');
    packets.forEach(packet => {
        if (packet) {
            io.emit('execution_report', packet); 
        }
    });
});

tcpClient.on('error', (err) => console.error('TCP Error:', err.message));

// 3. Receive Orders from React and forward to C++
io.on('connection', (socket) => {
    console.log('React Frontend Connected');
    socket.on('submit_order', (order) => {
        const packet = `${order.clientId}|${order.instrument}|${order.side}|${order.qty}|${order.price}\n`;
        tcpClient.write(packet);
    });
});

server.listen(3001, () => {
    console.log('Node WebSocket Bridge running on port 3001');
});