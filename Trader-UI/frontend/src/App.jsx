import { useState, useEffect, useRef } from 'react';
import { io } from 'socket.io-client';

const socket = io('http://localhost:3001');

function App() {
  const [reports, setReports] = useState([]);
  const [isConnected, setIsConnected] = useState(false);
  const tableEndRef = useRef(null);
  
  const [order, setOrder] = useState({
    clientId: 'user_123',
    instrument: 'Rose',
    side: '1',
    qty: '10',
    price: '50.00'
  });

  useEffect(() => {
    socket.on('connect', () => setIsConnected(true));
    socket.on('disconnect', () => setIsConnected(false));

    socket.on('execution_report', (packet) => {
      const [orderId, clId, inst, side, status, qty, price, reason, time] = packet.split('|');
      const newReport = { orderId, clId, inst, side, status, qty, price, reason, time };
      setReports((prev) => [...prev, newReport]);
    });

    return () => {
      socket.off('connect');
      socket.off('disconnect');
      socket.off('execution_report');
    };
  }, []);

  useEffect(() => {
    tableEndRef.current?.scrollIntoView({ behavior: 'smooth' });
  }, [reports]);

  const handleInputChange = (e) => {
    setOrder({ ...order, [e.target.name]: e.target.value });
  };

  const handleManualSubmit = (e) => {
    e.preventDefault();
    socket.emit('submit_order', order);
  };

  // NEW: Manual Reset Function
  const handleManualReset = () => {
    if (window.confirm("Are you sure you want to wipe the engine memory and clear all live reports?")) {
      setReports([]); 
      socket.emit('reset_engine'); 
    }
  };

  const handleFileUpload = (e) => {
    const file = e.target.files[0];
    if (!file) return;

    setReports([]);
    socket.emit('reset_engine');

    const reader = new FileReader();
    reader.onload = (event) => {
      const lines = event.target.result.split('\n').filter(line => line.trim() !== '');
      const startIndex = lines[0].toLowerCase().includes('client') ? 1 : 0;
      
      for (let i = startIndex; i < lines.length; i++) {
        const [clientId, instrument, side, qty, price] = lines[i].split(',');
        if (clientId && instrument && side && qty && price) {
          socket.emit('submit_order', { 
            clientId: clientId.trim(), 
            instrument: instrument.trim(), 
            side: side.trim(), 
            qty: qty.trim(), 
            price: price.trim() 
          });
        }
      }
    };
    reader.readAsText(file);
    e.target.value = null; 
  };

  const downloadCSV = () => {
    if (reports.length === 0) return;
    const header = "Order ID,Client ID,Instrument,Side,Execution Status,Quantity,Price,Reason,Transaction Time\n";
    const csvContent = reports.map(r => 
      `${r.orderId},${r.clId},${r.inst},${r.side},${r.status},${r.qty},${r.price},${r.reason},${r.time}`
    ).join('\n');

    const blob = new Blob([header + csvContent], { type: 'text/csv' });
    const url = window.URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'output.csv';
    a.click();
    window.URL.revokeObjectURL(url);
  };

  return (
    <div style={styles.container}>
      <header style={styles.header}>
        <h1 style={styles.title}>🌸 Institutional Flower Exchange</h1>
        <div style={styles.statusBadge}>
          <span style={{ color: isConnected ? '#10b981' : '#ef4444', marginRight: '8px' }}>●</span>
          {isConnected ? 'Engine Connected' : 'Disconnected'}
        </div>
      </header>

      <div style={styles.dashboard}>
        <div style={styles.controlPanel}>
          
          <div style={styles.card}>
            <h3 style={styles.cardTitle}>Manual Order Entry</h3>
            <form onSubmit={handleManualSubmit} style={styles.form}>
              <div style={styles.inputGroup}>
                <label style={styles.label}>Client ID</label>
                <input name="clientId" value={order.clientId} onChange={handleInputChange} style={styles.input} />
              </div>
              <div style={styles.inputGroup}>
                <label style={styles.label}>Instrument</label>
                <select name="instrument" value={order.instrument} onChange={handleInputChange} style={styles.input}>
                  <option>Rose</option><option>Lavender</option><option>Lotus</option><option>Tulip</option><option>Orchid</option>
                </select>
              </div>
              <div style={styles.inputGroup}>
                <label style={styles.label}>Side</label>
                <select name="side" value={order.side} onChange={handleInputChange} style={styles.input}>
                  <option value="1">Buy (1)</option><option value="2">Sell (2)</option>
                </select>
              </div>
              <div style={styles.row}>
                <div style={{...styles.inputGroup, flex: 1}}>
                  <label style={styles.label}>Qty</label>
                  <input name="qty" type="number" value={order.qty} onChange={handleInputChange} style={styles.input} />
                </div>
                <div style={{...styles.inputGroup, flex: 1}}>
                  <label style={styles.label}>Price ($)</label>
                  <input name="price" type="number" step="0.01" value={order.price} onChange={handleInputChange} style={styles.input} />
                </div>
              </div>
              <button type="submit" style={order.side === '1' ? styles.buyBtn : styles.sellBtn}>
                SUBMIT {order.side === '1' ? 'BUY' : 'SELL'} ORDER
              </button>
            </form>
          </div>

          <div style={styles.card}>
            <h3 style={styles.cardTitle}>Bulk Order Upload</h3>
            <p style={styles.subtext}>Upload a CSV file (Client ID, Instrument, Side, Qty, Price)</p>
            <input type="file" accept=".csv" onChange={handleFileUpload} style={styles.fileInput} />
          </div>

          <div style={styles.card}>
            <h3 style={styles.cardTitle}>Data Export</h3>
            <button onClick={downloadCSV} style={styles.exportBtn}>
              Download output.csv
            </button>
          </div>

          {/* NEW: System Controls */}
          <div style={{...styles.card, borderColor: '#7f1d1d', backgroundColor: '#450a0a11'}}>
            <h3 style={{...styles.cardTitle, color: '#f87171'}}>System Controls</h3>
            <p style={styles.subtext}>Wipe engine memory and clear physical output file.</p>
            <button onClick={handleManualReset} style={styles.dangerBtn}>
              ⚠️ HARD RESET ENGINE
            </button>
          </div>

        </div>

        <div style={styles.tablePanel}>
          <div style={styles.card}>
            <h3 style={styles.cardTitle}>Live Execution Reports ({reports.length})</h3>
            <div style={styles.tableContainer}>
              <table style={styles.table}>
                <thead style={styles.thead}>
                  <tr>
                    <th style={styles.th}>Order ID</th>
                    <th style={styles.th}>Client</th>
                    <th style={styles.th}>Asset</th>
                    <th style={styles.th}>Side</th>
                    <th style={styles.th}>Status</th>
                    <th style={styles.th}>Qty</th>
                    <th style={styles.th}>Price</th>
                    <th style={styles.th}>Time</th>
                  </tr>
                </thead>
                <tbody>
                  {reports.map((r, i) => (
                    <tr key={i} style={styles.tr}>
                      <td style={styles.td}>{r.orderId}</td>
                      <td style={styles.td}>{r.clId}</td>
                      <td style={{...styles.td, fontWeight: 'bold'}}>{r.inst}</td>
                      <td style={{...styles.td, color: r.side === '1' ? '#10b981' : '#ef4444'}}>
                        {r.side === '1' ? 'BUY' : 'SELL'}
                      </td>
                      <td style={styles.td}>
                        <span style={getStatusStyle(r.status)}>{r.status}</span>
                      </td>
                      <td style={styles.td}>{r.qty}</td>
                      <td style={styles.td}>${r.price}</td>
                      <td style={{...styles.td, color: '#94a3b8', fontSize: '0.85rem'}}>{r.time}</td>
                    </tr>
                  ))}
                  <tr ref={tableEndRef} />
                </tbody>
              </table>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

const getStatusStyle = (status) => {
  const base = { padding: '4px 8px', borderRadius: '4px', fontSize: '0.85rem', fontWeight: 'bold' };
  if (status === 'New') return { ...base, backgroundColor: '#3b82f633', color: '#60a5fa' };
  if (status === 'Fill') return { ...base, backgroundColor: '#10b98133', color: '#34d399' };
  if (status === 'Pfill') return { ...base, backgroundColor: '#f59e0b33', color: '#fbbf24' };
  if (status === 'Rejected') return { ...base, backgroundColor: '#ef444433', color: '#f87171' };
  return base;
};

const styles = {
  container: { minHeight: '100vh', backgroundColor: '#0f172a', color: '#f8fafc', fontFamily: 'system-ui, -apple-system, sans-serif' },
  header: { display: 'flex', justifyContent: 'space-between', alignItems: 'center', padding: '20px 30px', backgroundColor: '#1e293b', borderBottom: '1px solid #334155' },
  title: { margin: 0, fontSize: '1.5rem', fontWeight: '600', color: '#e2e8f0' },
  statusBadge: { backgroundColor: '#0f172a', padding: '6px 12px', borderRadius: '20px', fontSize: '0.9rem', fontWeight: '500', border: '1px solid #334155' },
  dashboard: { display: 'flex', gap: '20px', padding: '20px 30px', height: 'calc(100vh - 80px)' },
  controlPanel: { flex: '0 0 320px', display: 'flex', flexDirection: 'column', gap: '20px' },
  tablePanel: { flex: '1', display: 'flex', flexDirection: 'column' },
  card: { backgroundColor: '#1e293b', borderRadius: '10px', padding: '20px', border: '1px solid #334155', display: 'flex', flexDirection: 'column', height: '100%' },
  cardTitle: { margin: '0 0 15px 0', fontSize: '1.1rem', color: '#cbd5e1' },
  subtext: { margin: '0 0 15px 0', fontSize: '0.85rem', color: '#94a3b8' },
  form: { display: 'flex', flexDirection: 'column', gap: '12px' },
  row: { display: 'flex', gap: '10px' },
  inputGroup: { display: 'flex', flexDirection: 'column', gap: '4px' },
  label: { fontSize: '0.85rem', color: '#94a3b8', fontWeight: '500' },
  input: { padding: '10px', backgroundColor: '#0f172a', border: '1px solid #334155', borderRadius: '6px', color: '#f8fafc', fontSize: '0.95rem', outline: 'none' },
  fileInput: { color: '#94a3b8', fontSize: '0.9rem' },
  buyBtn: { padding: '12px', backgroundColor: '#10b981', color: '#ffffff', border: 'none', borderRadius: '6px', fontWeight: 'bold', cursor: 'pointer', marginTop: '5px' },
  sellBtn: { padding: '12px', backgroundColor: '#ef4444', color: '#ffffff', border: 'none', borderRadius: '6px', fontWeight: 'bold', cursor: 'pointer', marginTop: '5px' },
  exportBtn: { padding: '12px', backgroundColor: '#3b82f6', color: '#ffffff', border: 'none', borderRadius: '6px', fontWeight: 'bold', cursor: 'pointer' },
  dangerBtn: { padding: '12px', backgroundColor: 'transparent', color: '#f87171', border: '1px solid #f87171', borderRadius: '6px', fontWeight: 'bold', cursor: 'pointer', transition: 'all 0.2s' },
  tableContainer: { overflowY: 'auto', flex: 1, paddingRight: '5px' },
  table: { width: '100%', borderCollapse: 'collapse', textAlign: 'left' },
  thead: { position: 'sticky', top: 0, backgroundColor: '#1e293b', zIndex: 1 },
  th: { padding: '12px 10px', borderBottom: '2px solid #334155', color: '#94a3b8', fontSize: '0.9rem', fontWeight: '600' },
  tr: { borderBottom: '1px solid #334155' },
  td: { padding: '12px 10px', fontSize: '0.95rem' }
};

export default App;