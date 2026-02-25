const express = require('express');
const mongoose = require('mongoose');
const cors = require('cors');
const SensorData = require('./models/SensorData');
require('dotenv').config();

const app = express();
const PORT = 5000;

app.use(cors());
app.use(express.json());

const twilio = require('twilio');
const client = twilio(process.env.TWILIO_SID, process.env.TWILIO_AUTH);

mongoose.connect('mongodb://127.0.0.1:27017/smartfarm', {
  useNewUrlParser: true,
  useUnifiedTopology: true
})
.then(() => console.log("Connected to MongoDB"))
.catch(err => console.error("MongoDB connection error:", err));

app.post('/api/data', async (req, res) => {
  try {
    const data = req.body;

    const newData = new SensorData(data);
    await newData.save();
    console.log("Data received:", data);

    let alertMessage = "";

    if (data.dry === true) alertMessage += "⚠️ Soil Dry!\n";
    if (data.raining === true) alertMessage += "☔ Heavy Rain Detected!\n";
    if (data.fire === true) alertMessage += "🔥 FIRE ALERT!\n";
    if (data.waterLow === true) alertMessage += "💧 Water Level Low!\n";

    if (alertMessage !== "") {
      alertMessage =
        "🌾 *Smart Farming Alert!*\n\n" +
        alertMessage +
        `\n📍 Temperature: ${data.temp}°C\n💧 Humidity: ${data.hum}%`;

     
await client.messages.create({
  from: 'whatsapp:+14155238886',        
  to: process.env.TWILIO_TO,            
  body: alertMessage
});


      console.log("WhatsApp alert sent!");
    }

    res.status(201).json({ message: 'Data saved & alert sent (if needed)' });

  } catch (err) {
    console.error("Error saving data:", err.message);
    res.status(500).json({ error: 'Failed to save data' });
  }
});

app.get('/api/data/latest', async (req, res) => {
  try {
    const latest = await SensorData.findOne().sort({ timestamp: -1 });
    res.json(latest);
  } catch (err) {
    console.error("Error fetching data:", err.message);
    res.status(500).json({ error: 'Failed to fetch data' });
  }
});

app.get('/', (req, res) => {
  res.send('Smart Farming Backend is running.');
});

app.listen(PORT, '0.0.0.0', () =>
  console.log(` Server running at http://0.0.0.0:${PORT}`)
);
