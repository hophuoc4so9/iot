import React, { useEffect, useState } from 'react';
import axios from 'axios';
import {
  Box, Button, Card, CardContent, Container, Divider,
  TextField, Typography, Dialog, DialogTitle,
  DialogContent, DialogActions
} from '@mui/material';
import SendIcon from '@mui/icons-material/Send';
import AddIcon from '@mui/icons-material/Add';
import VisibilityIcon from '@mui/icons-material/Visibility';
import './App.css';

function App() {
  const [devices, setDevices] = useState([]);
  const [newDevice, setNewDevice] = useState({ name: '', topic: '' });
  const [payloads, setPayloads] = useState({});
  const [telemetry, setTelemetry] = useState([]);
  const [selectedDevice, setSelectedDevice] = useState(null);
  const [openDialog, setOpenDialog] = useState(false);

  useEffect(() => {
    fetchDevices();
  }, []);

  const fetchDevices = async () => {
    try {
      const res = await axios.get('http://localhost:8080/devices');
      setDevices(res.data);
      const initPayloads = {};
      res.data.forEach(d => initPayloads[d.id] = '');
      setPayloads(initPayloads);
    } catch (error) {
      console.error('Error fetching devices:', error);
    }
  };

  const handleSend = async (id) => {
    const payload = payloads[id];
    if (!payload) {
      alert('Vui lòng nhập lệnh điều khiển!');
      return;
    }
    try {
      await axios.post(`http://localhost:8080/devices/${id}/control`, payload, {
        headers: { 'Content-Type': 'text/plain' }
      });
      alert('Lệnh đã gửi thành công!');
    } catch (error) {
      console.error('Error sending command:', error);
      alert('Lỗi khi gửi lệnh!');
    }
  };

  const handleCreate = async () => {
    if (!newDevice.name || !newDevice.topic) {
      alert('Vui lòng điền đầy đủ thông tin thiết bị!');
      return;
    }
    try {
      await axios.post('http://localhost:8080/devices', newDevice);
      setNewDevice({ name: '', topic: '' });
      fetchDevices();
      alert('Thiết bị đã được tạo thành công!');
    } catch (error) {
      console.error('Error creating device:', error);
      alert('Lỗi khi tạo thiết bị!');
    }
  };

  const fetchTelemetry = async (deviceId) => {
    try {
      const res = await axios.get(`http://localhost:8080/telemetry/${deviceId}`);
      return res.data;
    } catch (error) {
      console.error('Error fetching telemetry:', error);
      return [];
    }
  };

  const handleViewTelemetry = async (device) => {
    const data = await fetchTelemetry(device.id);
    setTelemetry(data);
    setSelectedDevice(device);
    setOpenDialog(true);
  };

  return (
    <Container maxWidth="sm" sx={{ mt: 4 }}>
      <Typography variant="h4" textAlign="center" fontWeight="bold" gutterBottom>
        📡 IoT Device Dashboard
      </Typography>

      <Typography variant="h6" gutterBottom sx={{ mt: 3 }}>
        📋 Danh sách thiết bị
      </Typography>
      
      {devices.length === 0 ? (
        <Typography color="text.secondary" sx={{ mb: 2 }}>
          Chưa có thiết bị nào. Hãy thêm thiết bị mới bên dưới.
        </Typography>
      ) : (
        devices.map(device => (
          <Card key={device.id} sx={{ mb: 2, backgroundColor: '#f0f4ff' }}>
            <CardContent>
              <Typography fontWeight="bold" variant="h6">
                {device.name}
              </Typography>
              <Typography variant="body2" gutterBottom color="text.secondary">
                MQTT Topic: <code>{device.topic}</code>
              </Typography>
              <TextField
                fullWidth
                label="Lệnh điều khiển"
                placeholder='{"data":20}'
                multiline
                maxRows={3}
                value={payloads[device.id] || ''}
                onChange={(e) => setPayloads({ ...payloads, [device.id]: e.target.value })}
                sx={{ mt: 1, mb: 2 }}
                size="small"
              />
              <Box display="flex" justifyContent="flex-end" gap={1}>
                <Button
                  variant="contained"
                  onClick={() => handleSend(device.id)}
                  endIcon={<SendIcon />}
                  sx={{ textTransform: 'none' }}
                >
                  Gửi lệnh
                </Button>
                <Button
                  variant="outlined"
                  onClick={() => handleViewTelemetry(device)}
                  startIcon={<VisibilityIcon />}
                  sx={{ textTransform: 'none' }}
                >
                  Xem dữ liệu
                </Button>
              </Box>
            </CardContent>
          </Card>
        ))
      )}

      <Divider sx={{ my: 3 }} />

      <Typography variant="h6" gutterBottom>➕ Thêm thiết bị mới</Typography>
      <TextField
        fullWidth
        label="Tên thiết bị"
        variant="outlined"
        sx={{ mb: 2 }}
        value={newDevice.name}
        onChange={(e) => setNewDevice({ ...newDevice, name: e.target.value })}
        placeholder="VD: Temperature Sensor"
      />
      <TextField
        fullWidth
        label="Topic MQTT"
        variant="outlined"
        sx={{ mb: 2 }}
        value={newDevice.topic}
        onChange={(e) => setNewDevice({ ...newDevice, topic: e.target.value })}
        placeholder="VD: /sensor/temp"
      />
      <Button
        variant="contained"
        fullWidth
        onClick={handleCreate}
        startIcon={<AddIcon />}
        sx={{ textTransform: 'none' }}
      >
        Tạo thiết bị
      </Button>

      {/* Dialog hiển thị telemetry */}
      {selectedDevice && (
        <Dialog open={openDialog} onClose={() => setOpenDialog(false)} fullWidth maxWidth="sm">
          <DialogTitle>📊 Telemetry - {selectedDevice.name}</DialogTitle>
          <DialogContent dividers>
            {telemetry.length === 0 ? (
              <Typography>Không có dữ liệu telemetry</Typography>
            ) : (
              telemetry.map((t, i) => (
                <Box key={i} sx={{ mb: 2, p: 1, bgcolor: '#f5f5f5', borderRadius: 1 }}>
                  <Typography><b>Giá trị:</b> {t.payload}</Typography>
                  <Typography variant="caption" color="text.secondary">
                    {new Date(t.timestamp).toLocaleString('vi-VN')}
                  </Typography>
                </Box>
              ))
            )}
          </DialogContent>
          <DialogActions>
            <Button onClick={() => setOpenDialog(false)}>Đóng</Button>
          </DialogActions>
        </Dialog>
      )}
    </Container>
  );
}

export default App;
