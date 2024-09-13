#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char* ssid = "Chơi đá không em?"; 
const char* password = "12345678";

IPAddress local_ip(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);
WebServer server(80);

const char HTML_PAGE[] = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Drone Control with Sliders</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
        }
        .slider-container {
            margin: 20px;
        }
        .slider {
            width: 300px;
        }
    </style>
</head>
<body>
    <h1>Drone Control Interface</h1>

    <!-- Throttle Slider -->
    <div class="slider-container">
        <label for="throttle">Throttle</label>
        <input type="range" id="throttle" class="slider" min="1000" max="2000" value="0">
        <span id="throttle-value">0</span>
    </div>

    <!-- Yaw Slider -->
    <div class="slider-container">
        <label for="yaw">Yaw</label>
        <input type="range" id="yaw" class="slider" min="1000" max="2000" value="1500">
        <span id="yaw-value">1500</span>
    </div>

    <!-- Pitch Slider -->
    <div class="slider-container">
        <label for="pitch">Pitch</label>
        <input type="range" id="pitch" class="slider" min="1000" max="2000" value="1500">
        <span id="pitch-value">1500</span>
    </div>

    <!-- Roll Slider -->
    <div class="slider-container">
        <label for="roll">Roll</label>
        <input type="range" id="roll" class="slider" min="1000" max="2000" value="1500">
        <span id="roll-value">1500</span>
    </div>

    <script>
        const throttleSlider = document.getElementById('throttle');
        const yawSlider = document.getElementById('yaw');
        const pitchSlider = document.getElementById('pitch');
        const rollSlider = document.getElementById('roll');

        const throttleValue = document.getElementById('throttle-value');
        const yawValue = document.getElementById('yaw-value');
        const pitchValue = document.getElementById('pitch-value');
        const rollValue = document.getElementById('roll-value');

        // Function to update and send values to ESP32
        function updateValues() {
            const throttle = throttleSlider.value;
            const yaw = yawSlider.value;
            const pitch = pitchSlider.value;
            const roll = rollSlider.value;

            throttleValue.textContent = throttle;
            yawValue.textContent = yaw;
            pitchValue.textContent = pitch;
            rollValue.textContent = roll;

            // Send values to Flask server
            fetch('http://192.168.4.1/control', {
                method: 'POST',
                body: JSON.stringify({ throttle, yaw, pitch, roll }),
                headers: { 'Content-Type': 'application/json' }
            });
        }

        // Function to reset yaw, pitch, and roll to center when mouse is released
        function resetToCenter() {
            yawSlider.value = 1500;
            pitchSlider.value = 1500;
            rollSlider.value = 1500;

            updateValues();  // Update values after resetting
        }

        // Update values on slider input
        throttleSlider.addEventListener('input', updateValues);
        yawSlider.addEventListener('input', updateValues);
        pitchSlider.addEventListener('input', updateValues);
        rollSlider.addEventListener('input', updateValues);

        // Reset yaw, pitch, roll to center when mouse is released
        yawSlider.addEventListener('mouseup', resetToCenter);
        pitchSlider.addEventListener('mouseup', resetToCenter);
        rollSlider.addEventListener('mouseup', resetToCenter);
    </script>
</body>
</html>
)=====";

void handle_Root(){
  server.send(200,"text/html",HTML_PAGE); 
}
void handle_Control(){
  //get data from request
  if(server.hasArg("plain")){
    String json=server.arg("plain");

    //parse
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, json);
    
    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }
   int throttle = doc["throttle"];
    int yaw = doc["yaw"];
    int pitch = doc["pitch"];
    int roll = doc["roll"];

    //print value
    Serial.print("Throttle: ");
    Serial.println(throttle);
    Serial.print("Yaw: ");
    Serial.println(yaw);
    Serial.print("Pitch: ");
    Serial.println(pitch);
    Serial.print("Roll: ");
    Serial.println(roll);

    // send a response to the client
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"No JSON data\"}");
  }
}

void setup(){
  Serial.begin(115200);
  WiFi.softAP(ssid,password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  server.on("/", HTTP_GET, handle_Root);
  server.on("/control",HTTP_POST,handle_Control);
  server.begin();
}

void loop(){
  server.handleClient();
}
