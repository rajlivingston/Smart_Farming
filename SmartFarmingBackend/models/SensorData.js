const mongoose = require('mongoose');

const sensorSchema = new mongoose.Schema({
  soil: Number,
  light: Number,
  rain: Number,
  water: Number,
  temp: Number,
  hum: Number,
  dry: Boolean,
  raining: Boolean,
  waterLow: Boolean,
  fire: Boolean,
  timestamp: { type: Date, default: Date.now }
});

module.exports = mongoose.model('SensorData', sensorSchema);
