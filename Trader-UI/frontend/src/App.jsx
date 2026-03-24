import { useState, useEffect } from 'react';
import { io } from 'socket.io-client';

// Connect to our Node Bridge
const socket = io('http://localhost:3001');

function App() {
  const [reports, setReports] = useState([]);
  const [order, setOrder] = useState({
    clientId: 'user_123',
    instrument: 'Rose',
    side: '1',
    qty: '10',
    price: '50.00'
  });

  useEffect(() => {
    // Listen for receipts coming back from the C++ engine
    socket.on('execution_report', (packet) => {
      // Packet format: ord1|user_123|Rose|1|New|10|50.00||20260321-120000.000
      const [orderId, clId, inst, side, status, qty, price, reason, time] = packet.split('|');
      const newReport = { orderId, clId, inst, side, status, qty, price, reason, time };
      
      // Add the new receipt to the top of our table
      setReports((prev) => [newReport, ...prev]);
    });

    return () => socket.off('execution_report');
  }, []);

  const handleChange = (e) => {
    setOrder({ ...order, [e.target.name]: e.target.value });
  };

  const handleSubmit = (e) => {
    e.preventDefault();
    socket.emit('submit_order', order); // Send to Node Bridge
  };

  return (
    <div style={{ padding: '20px', fontFamily: 'system-ui, sans-serif', backgroundColor: '#1e1e2e', color: '#cdd6f4', minHeight: '100vh' }}>
      <h1>🌸 Flower Exchange Terminal</h1>
      
      {/* Order Submission Form */}
      <div style={{ backgroundColor: '#313244', padding: '20px', borderRadius: '8px', marginBottom: '20px' }}>
        <h3>Submit New Order</h3>
        <form onSubmit={handleSubmit} style={{ display: 'flex', gap: '15px', alignItems: 'flex-end' }}>
          <label>Client ID:<br/><input name="clientId" value={order.clientId} onChange={handleChange} style={inputStyle} /></label>
          <label>Flower:<br/>
            <select name="instrument" value={order.instrument} onChange={handleChange} style={inputStyle}>
              <option>Rose</option><option>Lavender</option><option>Lotus</option><option>Tulip</option><option>Orchid</option>
            </select>
          </label>
          <label>Side:<br/>
            <select name="side" value={order.side} onChange={handleChange} style={inputStyle}>
              <option value="1">Buy</option><option value="2">Sell</option>
            </select>
          </label>
          <label>Quantity:<br/><input name="qty" type="number" step="10" value={order.qty} onChange={handleChange} style={inputStyle} /></label>
          <label>Price ($):<br/><input name="price" type="number" step="0.01" value={order.price} onChange={handleChange} style={inputStyle} /></label>
          <button type="submit" style={{ padding: '10px 20px', backgroundColor: '#89b4fa', color: '#11111b', border: 'none', borderRadius: '4px', cursor: 'pointer', fontWeight: 'bold' }}>
            SEND ORDER
          </button>
        </form>
      </div>

      {/* Execution Reports Table */}
      <div style={{ backgroundColor: '#313244', padding: '20px', borderRadius: '8px' }}>
        <h3>Live Execution Reports</h3>
        <table style={{ width: '100%', textAlign: 'left', borderCollapse: 'collapse' }}>
          <thead>
            <tr style={{ borderBottom: '1px solid #585b70' }}>
              <th>Order ID</th><th>Flower</th><th>Side</th><th>Status</th><th>Qty</th><th>Price</th><th>Time</th>
            </tr>
          </thead>
          <tbody>
            {reports.map((r, i) => (
              <tr key={i} style={{ borderBottom: '1px solid #45475a', color: r.side === '1' ? '#a6e3a1' : '#f38ba8' }}>
                <td style={{ padding: '8px 0' }}>{r.orderId}</td>
                <td>{r.inst}</td>
                <td>{r.side === '1' ? 'Buy' : 'Sell'}</td>
                <td style={{ fontWeight: 'bold' }}>{r.status}</td>
                <td>{r.qty}</td>
                <td>${r.price}</td>
                <td>{r.time}</td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
}

const inputStyle = { padding: '8px', borderRadius: '4px', border: '1px solid #6c7086', backgroundColor: '#181825', color: '#cdd6f4', marginTop: '5px' };

export default App;