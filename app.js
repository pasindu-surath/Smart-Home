import { initializeApp } from "https://www.gstatic.com/firebasejs/12.1.0/firebase-app.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/12.1.0/firebase-database.js";

// =====================================================
// YOUR FIREBASE WEB CONFIG
// =====================================================
const firebaseConfig = {
  apiKey: "AIzaSyBknnhznrjjmUyTKxjylUvyJGAQFudcibk",
  authDomain: "smart-home-iot-9226f.firebaseapp.com",
  databaseURL: "https://smart-home-iot-9226f-default-rtdb.firebaseio.com",
  projectId: "smart-home-iot-9226f",
  storageBucket: "smart-home-iot-9226f.firebasestorage.app",
  messagingSenderId: "833923559172",
  appId: "1:833923559172:web:7e2d551829a480661e6a51",
  measurementId: "G-40GZZ9RQH2"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

const smartHomeRef = ref(db, "smartHome");

// Connection status eka console eke pennanna
const connectedRef = ref(db, ".info/connected");
onValue(connectedRef, (snap) => {
  if (snap.val() === true) {
    console.log("🟢 Firebase ekata hariyata connect wuna!");
  } else {
    console.log("🔴 Firebase connection eka disconnect wela...");
  }
});

function setText(id, value) {
  document.getElementById(id).textContent = value;
}

function setStatus(id, text, active) {
  const el = document.getElementById(id);
  el.textContent = text;
  if (active) {
    el.classList.remove("bg-success");
    el.classList.add("bg-danger");
  } else {
    el.classList.remove("bg-danger");
    el.classList.add("bg-success");
  }
}

onValue(smartHomeRef, (snapshot) => {
  const data = snapshot.val() || {};

  const temperature = Number(data.temperature);
  const humidity = Number(data.humidity);
  const gas = Number(data.gas);

  setText("temperature", Number.isFinite(temperature) ? temperature.toFixed(1) + " °C" : "--");
  setText("humidity", Number.isFinite(humidity) ? humidity.toFixed(1) + " %" : "--");
  setText("gas", Number.isFinite(gas) ? String(Math.round(gas)) : "--");

  const laserAlert = data.securityAlert === true || data.laser === "ALERT";
  const gasAlert = data.gasAlert === true;

  setStatus("laserStatus", laserAlert ? "⚠ LASER ALERT" : "✓ SAFE", laserAlert);
  setStatus("gasStatus", gasAlert ? "⚠ GAS / SMOKE ALERT" : "✓ NORMAL", gasAlert);

  const overall = document.getElementById("overallStatus");

  if (laserAlert || gasAlert) {
    overall.textContent = "⚠ WARNING";
    overall.className = "alert alert-danger d-inline-block fw-bold shadow-sm px-4 py-2 mt-3";
  } else {
    overall.textContent = "✓ SYSTEM NORMAL";
    overall.className = "alert alert-success d-inline-block fw-bold shadow-sm px-4 py-2 mt-3";
  }

  const now = new Date();
  setText("lastSeen", "Updated: " + now.toLocaleTimeString());
}, (error) => {
  console.error(error);
  setText("lastSeen", "Firebase connection error");
});