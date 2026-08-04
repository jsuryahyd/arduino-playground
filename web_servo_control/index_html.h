#ifndef INDEX_HTML_H
#define INDEX_HTML_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Arduino R4 IoT Servo Dashboard</title>
  <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;700&display=swap" rel="stylesheet">
  <style>
    :root {
      --bg: #0b0f19;
      --card-bg: rgba(255, 255, 255, 0.04);
      --card-border: rgba(255, 255, 255, 0.08);
      --accent: #6366f1;
      --accent-hover: #4f46e5;
      --cyan: #06b6d4;
      --text: #f3f4f6;
      --text-muted: #9ca3af;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Inter', sans-serif; }
    body { background: var(--bg); color: var(--text); min-height: 100vh; padding: 24px 16px; display: flex; flex-direction: column; align-items: center; }
    .container { max-width: 640px; width: 100%; }
    header { text-align: center; margin-bottom: 28px; }
    header h1 { font-size: 1.8rem; font-weight: 700; background: linear-gradient(135deg, #a5b4fc, #06b6d4); -webkit-background-clip: text; -webkit-text-fill-color: transparent; }
    header p { color: var(--text-muted); font-size: 0.9rem; margin-top: 6px; }
    
    .card { background: var(--card-bg); border: 1px solid var(--card-border); border-radius: 16px; padding: 24px; backdrop-filter: blur(12px); margin-bottom: 20px; box-shadow: 0 10px 30px rgba(0,0,0,0.3); }
    .gauge-container { text-align: center; padding: 12px 0; }
    .angle-display { font-size: 3.5rem; font-weight: 700; color: var(--cyan); letter-spacing: -1px; }
    .status-badge { display: inline-block; padding: 6px 14px; background: rgba(99, 102, 241, 0.15); border: 1px solid rgba(99, 102, 241, 0.3); border-radius: 20px; font-size: 0.85rem; color: #a5b4fc; font-weight: 600; margin-top: 8px; }

    .slider-box { margin: 24px 0 12px; }
    input[type=range] { width: 100%; height: 8px; border-radius: 4px; background: #1f2937; outline: none; -webkit-appearance: none; cursor: pointer; }
    input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; width: 24px; height: 24px; border-radius: 50%; background: var(--accent); cursor: pointer; box-shadow: 0 0 12px rgba(99, 102, 241, 0.8); transition: transform 0.1s; }
    input[type=range]::-webkit-slider-thumb:hover { transform: scale(1.15); }

    .btn-grid { display: grid; grid-template-columns: repeat(5, 1fr); gap: 8px; margin-top: 16px; }
    .btn { background: rgba(255,255,255,0.05); border: 1px solid var(--card-border); color: var(--text); padding: 10px 0; border-radius: 10px; font-weight: 600; cursor: pointer; transition: all 0.2s ease; font-size: 0.85rem; }
    .btn:hover { background: var(--accent); border-color: var(--accent); transform: translateY(-2px); }
    .btn.active { background: var(--accent); border-color: var(--accent); box-shadow: 0 0 12px rgba(99, 102, 241, 0.5); }
    
    .mode-grid { display: grid; grid-template-columns: repeat(5, 1fr); gap: 8px; }
    .toggle-row { display: flex; justify-content: space-between; align-items: center; margin-top: 16px; padding-top: 16px; border-top: 1px solid var(--card-border); }
    
    footer { text-align: center; color: var(--text-muted); font-size: 0.8rem; margin-top: 16px; }
  </style>
</head>
<body>
  <div class="container">
    <header>
      <h1>🤖 R4 WiFi Servo Dashboard</h1>
      <p>Wireless Non-Blocking Motion & Dynamic LED Matrix Interface</p>
    </header>

    <div class="card">
      <div class="gauge-container">
        <div class="angle-display" id="angleVal">90°</div>
        <div class="status-badge" id="modeBadge">Mode: MANUAL</div>
      </div>

      <div class="slider-box">
        <input type="range" id="angleSlider" min="0" max="180" value="90" oninput="onSliderChange(this.value)">
      </div>

      <div class="btn-grid">
        <button class="btn" onclick="setAngle(0)">0°</button>
        <button class="btn" onclick="setAngle(45)">45°</button>
        <button class="btn" onclick="setAngle(90)">90°</button>
        <button class="btn" onclick="setAngle(135)">135°</button>
        <button class="btn" onclick="setAngle(180)">180°</button>
      </div>
    </div>

    <div class="card">
      <h3 style="font-size: 1rem; margin-bottom: 14px; color: var(--text-muted);">MOTION MODES</h3>
      <div class="mode-grid">
        <button class="btn" id="btn-manual" onclick="setMode('manual')">MANUAL</button>
        <button class="btn" id="btn-sweep" onclick="setMode('sweep')">SWEEP</button>
        <button class="btn" id="btn-step" onclick="setMode('step')">STEP</button>
        <button class="btn" id="btn-ease" onclick="setMode('ease')">EASE</button>
        <button class="btn" id="btn-bounce" onclick="setMode('bounce')">BOUNCE</button>
      </div>

      <div class="toggle-row">
        <span style="font-size: 0.9rem; color: var(--text-muted);">PWM Signal Status</span>
        <button class="btn" id="btnPower" style="padding: 8px 16px;" onclick="togglePower()">ATTACHED</button>
      </div>

      <div class="toggle-row" style="margin-top: 16px; padding-top: 16px; border-top: 1px solid var(--card-border);">
        <span style="font-size: 0.9rem; color: var(--text-muted);">Servo Movement Speed</span>
        <span id="speedVal" style="font-weight: 600; color: var(--cyan); font-size: 0.9rem;">180 °/s</span>
      </div>
      <div class="slider-box" style="margin: 10px 0 0;">
        <input type="range" id="speedSlider" min="15" max="360" value="180" oninput="onSpeedChange(this.value)">
      </div>
    </div>

    <footer>
      Arduino UNO R4 WiFi &bull; Antigravity IDE Playground
    </footer>
  </div>

  <script>
    let isAttached = true;

    function setAngle(deg) {
      document.getElementById('angleSlider').value = deg;
      onSliderChange(deg);
    }

    function onSliderChange(deg) {
      document.getElementById('angleVal').innerText = deg + '°';
      fetch('/api/set?angle=' + deg)
        .then(r => { if (!r.ok) alert('Set Angle Failed: HTTP ' + r.status); })
        .catch(e => alert('Network Error: ' + e.message));
    }

    function onSpeedChange(degSec) {
      document.getElementById('speedVal').innerText = degSec + ' °/s';
      fetch('/api/speed?val=' + degSec)
        .then(r => { if (!r.ok) alert('Set Speed Failed: HTTP ' + r.status); })
        .catch(e => alert('Network Error: ' + e.message));
    }

    function setMode(mode) {
      fetch('/api/mode?name=' + mode)
        .then(r => { if (!r.ok) alert('Set Mode Failed: HTTP ' + r.status); })
        .catch(e => alert('Network Error: ' + e.message));
    }

    function togglePower() {
      const action = isAttached ? 'detach' : 'attach';
      fetch('/api/power?action=' + action)
        .then(r => { if (!r.ok) alert('Power Action Failed: HTTP ' + r.status); })
        .catch(e => alert('Network Error: ' + e.message));
    }

    // Real-time Server-Sent Events (SSE) Stream Connection
    const evtSource = new EventSource('/api/events');
    evtSource.onmessage = function(e) {
      try {
        const data = JSON.parse(e.data);
        document.getElementById('angleVal').innerText = data.angle + '°';
        document.getElementById('angleSlider').value = data.angle;
        document.getElementById('modeBadge').innerText = 'Mode: ' + data.mode;
        
        if (data.speed !== undefined) {
          document.getElementById('speedVal').innerText = data.speed + ' °/s';
          document.getElementById('speedSlider').value = data.speed;
        }

        isAttached = data.attached;
        
        const pBtn = document.getElementById('btnPower');
        pBtn.innerText = isAttached ? 'ATTACHED' : 'DETACHED';
        pBtn.style.background = isAttached ? '#10b981' : '#ef4444';
        pBtn.style.borderColor = isAttached ? '#10b981' : '#ef4444';

        ['manual','sweep','step','ease','bounce'].forEach(m => {
          const btn = document.getElementById('btn-' + m);
          if (btn) {
            if (data.mode.toLowerCase() === m) btn.classList.add('active');
            else btn.classList.remove('active');
          }
        });
      } catch (err) {
        console.error('SSE Parse Error:', err);
      }
    };
    evtSource.onerror = function(err) {
      console.warn('SSE Disconnected, automatic reconnecting...', err);
    };
  </script>
</body>
</html>
)rawliteral";

#endif // INDEX_HTML_H
