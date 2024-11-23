#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <PubSubClient.h>

// UART configuration for CC2530 module
const int RXpin = 19; // GPIO19 (RX)
const int TXpin = 20; // GPIO20 (TX)
HardwareSerial vietnam(2); // UART2

// Wi-Fi Access Point credentials
const char* ap_ssid = "DroneControlAP";
const char* ap_password = "123123123";

// Wi-Fi Station credentials (mạng Wi-Fi công cộng)
const char* sta_ssid = "P204 / 209";
const char* sta_password = "30041975";

// MQTT Broker
const char* mqtt_server = "cda79e6780fe42829598bfb61e6ba1fe.s1.eu.hivemq.cloud";
const int mqtt_port = 8883; // TLS MQTT port
const char* mqtt_user = "peppa";
const char* mqtt_password = "Peppa_pig1";
const char* mqtt_topic = "drone/data"; // Topic để đẩy dữ liệu từ UART lên MQTT

WiFiClientSecure espClient;
PubSubClient client(espClient);

// Root CA Certificate
static const char* root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// Control variables (from web interface)
int throttle = 1000; // Initialize to minimum value
int yaw = 1500;      // Center value
int pitch = 1500;    // Center value
int roll = 1500;     // Center value

// Variables to hold received data via UART
float pitch_recv = 0.0;
float roll_recv = 0.0;
float voltage_recv = 0.0;

// Networking
IPAddress local_ip(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

// Web server on port 80
WebServer server(80);

// Flag to indicate new data to send
volatile bool newDataToSend = false;

// Data packet structure for sending data
struct __attribute__((packed)) DataPacketSend {
  uint8_t startByte;
  int32_t throttle;
  int32_t pitch;
  int32_t roll;
  int32_t yaw;
  uint8_t endByte;
};

// Data packet structure for receiving data
struct __attribute__((packed)) DataPacketReceive {
  uint8_t startByte;
  float pitch_recev;
  float roll_recev;
  float voltage_recev;
  uint8_t endByte;
};

// Function to send data via UART
void sendData(int32_t throttle, int32_t pitch, int32_t roll, int32_t yaw) {
  DataPacketSend packet;
  packet.startByte = 0xAA; // Start byte
  packet.throttle = throttle;
  packet.pitch = pitch;
  packet.roll = roll;
  packet.yaw = yaw;
  packet.endByte = 0x55; // End byte
  vietnam.write((uint8_t*)&packet, sizeof(packet)); // Send the entire packet
}

// Giao diện HTML mới đã được chỉnh sửa
const char HTML_PAGE[] = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Drone Control Interface</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Drone!!!!!!</title>
    <style>
        body {
            margin: 0;
            font-family: Arial, sans-serif;
            background-color: #220000;
            color: #ffffff;
            display: flex;
            flex-direction: column;
            align-items: center;
        }
        .control-panel {
            display: flex;
            justify-content: space-around;
            align-items: center;
            width: 100%;
            padding: 10px;
            background-color: #440000;
        }
        #status-display {
            display: flex;
            /* text-align: left; */
            gap: 10px; /* Khoảng cách giữa các phần tử */
            justify-content: center;
            align-items: center;
        }
        #battery_status {
            font-size: 14px;
        }
        #mqtt_status_display {
            color: #ffcc00;
        }
        .controls-container {
            display: flex;
            justify-content: space-between;
            width: 90%;
            padding: 20px;
            flex-wrap: wrap;
        }
        .slider-group {
            display: flex;
            flex-direction: column;
            align-items: center;
            width: 45%;
            margin-bottom: 30px;
        }
        .vertical-slider-container, .horizontal-slider-container {
            position: relative;
            margin: 20px 0;
            width: 100%;
        }
        /* Tăng độ dày của thanh trượt throttle và pitch */
        .vertical-slider {
            -webkit-appearance: slider-vertical;
            width: 60px; /* Tăng độ rộng */
            height: 130px; /* Tăng chiều cao */
            writing-mode: bt-lr; /* For Firefox */
            margin: 0 20px;
        }
        /* Tăng độ dày của thanh trượt yaw và roll */
        .horizontal-slider {
            width: 300px; /* Tăng độ rộng */
            height: 30px; /* Tăng độ cao để tăng độ dày */
            -webkit-appearance: none;
            background: #ddd;
            outline: none;
            opacity: 0.9;
            -webkit-transition: .2s;
            transition: opacity .2s;
            border-radius: 5px;
        }
        .horizontal-slider:hover {
            opacity: 1;
        }
        .horizontal-slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 35px; /* Tăng độ rộng */
            height: 35px; /* Tăng độ cao */
            background: #ff0000;
            cursor: pointer;
            border-radius: 50%;
            border: none;
        }
        .horizontal-slider::-moz-range-thumb {
            width: 35px;
            height: 35px;
            background: #ff0000;
            cursor: pointer;
            border-radius: 50%;
            border: none;
        }
        .vertical-slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 60px; /* Tăng độ rộng */
            height: 60px; /* Tăng độ cao */
            background: #ff0000;
            cursor: pointer;
            border-radius: 50%;
            border: none;
        }
        .vertical-slider::-moz-range-thumb {
            width: 60px;
            height: 60px;
            background: #ff0000;
            cursor: pointer;
            border-radius: 50%;
            border: none;
        }
        label {
            font-weight: bold;
            margin-bottom: 10px;
        }
        .vertical-slider-container label {
            position: absolute;
           /* right: -10px; /* Đặt nhãn bên phải của thanh trượt */
            top: 50%;
            transform: translateY(-50%);
            white-space: nowrap;
        }
        .horizontal-slider-container label {
            display: block;
            margin-bottom: 5px;
            font-weight: bold;
        }
        .value-display {
            margin-top: 5px;
            font-size: 16px;
            font-weight: bold;
        }
        /* @media (max-width: 1000px) {
            .controls-container {
                flex-direction: column;
                align-items: center;
            }
            .slider-group {
                width: 80%;
                height: 40%;
            }
            .horizontal-slider {
                width: 100%;
            }
            .vertical-slider-container label {
                right: -70px; /* Điều chỉnh lại cho màn hình nhỏ */
            } */
        }
    </style>
</head>
<body>
    <div class="control-panel">
        <div id="status-display">
            Throt: <span id="throttle-display">1000</span>
            Roll: <span id="roll-display">1500</span>
            Pitch: <span id="pitch-display">1500</span>
            Yaw: <span id="yaw-display">1500</span>
        </div>
        <div id="battery_status">
            Battery: <span id="bat_status">0%</span>
        </div>
        <div id="mqtt_status">MQTT: <span id="mqtt_status_display">Disconnected</span></div>
    </div>

    <div class="controls-container">
        <!-- Throttle và Yaw -->
        <div class="slider-group">
            <div class="vertical-slider-container">
                <input type="range" id="throttle" class="slider vertical-slider" min="1000" max="2000" value="1000" orient="vertical">
                <label for="throttle">Throttle</label>
                <span id="throttle-value" class="value-display">1000</span>
                 <!-- <label for="throttle" id="throttle-value" class="value-display"></label> -->
            </div>
            <div class="horizontal-slider-container">
                <label for="yaw">Yaw</label>
                <input type="range" id="yaw" class="slider horizontal-slider" min="1000" max="2000" value="1500">
                <span id="yaw-value" class="value-display">1500</span>
            </div>
        </div>

        <!-- Pitch và Roll -->
        <div class="slider-group">
            <div class="vertical-slider-container">
                <input type="range" id="pitch" class="slider vertical-slider" min="1000" max="2000" value="1500" orient="vertical">
                <label for="pitch">Pitch</label>
                <span id="pitch-value" class="value-display">1500</span>
            </div>
            <div class="horizontal-slider-container">
                <label for="roll">Roll</label>
                <input type="range" id="roll" class="slider horizontal-slider" min="1000" max="2000" value="1500">
                <span id="roll-value" class="value-display">1500</span>
            </div>
        </div>
    </div>

    <script>
        const throttleSlider = document.getElementById('throttle');
        const yawSlider = document.getElementById('yaw');
        const pitchSlider = document.getElementById('pitch');
        const rollSlider = document.getElementById('roll');

        const throttleDisplay = document.getElementById('throttle-display');
        const yawDisplay = document.getElementById('yaw-display');
        const pitchDisplay = document.getElementById('pitch-display');
        const rollDisplay = document.getElementById('roll-display');
        const batteryDisplay = document.getElementById('bat_status');

        const mqttStatusDisplay = document.getElementById('mqtt_status_display');

        // Throttle slider
        throttleSlider.addEventListener('input', function () {
            let throttleVal = throttleSlider.value;
            throttleDisplay.textContent = throttleVal;
            document.getElementById('throttle-value').textContent = throttleVal;
            publishDroneData();
        });

        // Yaw slider
        yawSlider.addEventListener('input', function () {
            let yawVal = yawSlider.value;
            yawDisplay.textContent = yawVal;
            document.getElementById('yaw-value').textContent = yawVal;
            publishDroneData();
        });

        // Pitch slider
        pitchSlider.addEventListener('input', function () {
            let pitchVal = pitchSlider.value;
            pitchDisplay.textContent = pitchVal;
            document.getElementById('pitch-value').textContent = pitchVal;
            publishDroneData();
        });

        // Roll slider
        rollSlider.addEventListener('input', function () {
            let rollVal = rollSlider.value;
            rollDisplay.textContent = rollVal;
            document.getElementById('roll-value').textContent = rollVal;
            publishDroneData();
        });

        // Reset pitch và roll sliders to center when mouseup or touchend
        function resetSlider(slider, display) {
            slider.value = 1500;
            display.textContent = 1500;
            publishDroneData();
        }

        // Pitch slider reset
        pitchSlider.addEventListener('mouseup', function() {
            resetSlider(pitchSlider, pitchDisplay);
        });
        pitchSlider.addEventListener('touchend', function() {
            resetSlider(pitchSlider, pitchDisplay);
        });

        // Roll slider reset
        rollSlider.addEventListener('mouseup', function() {
            resetSlider(rollSlider, rollDisplay);
        });
        rollSlider.addEventListener('touchend', function() {
            resetSlider(rollSlider, rollDisplay);
        });

        function publishDroneData() {
            const data = {
                throttle: parseInt(throttleSlider.value),
                yaw: parseInt(yawSlider.value),
                pitch: parseInt(pitchSlider.value),
                roll: parseInt(rollSlider.value)
            };
            fetch('/control', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(data)
            })
            .then(response => response.json())
            .then(result => {
                if(result.status === 'ok') {
                    console.log('Data sent successfully');
                } else {
                    console.error('Error sending data');
                }
            })
            .catch(error => {
                console.error('Error:', error);
            });
        }

        // Cập nhật trạng thái MQTT từ ESP32
        setInterval(() => {
            fetch('/mqtt_status')
                .then(response => response.json())
                .then(data => {
                    mqttStatusDisplay.textContent = data.status || 'Disconnected';
                })
                .catch(error => console.error('Error fetching MQTT status:', error));
        }, 1000); // Cập nhật mỗi giây

        // Cập nhật các giá trị từ MQTT đã được đẩy lên ESP32
        setInterval(() => {
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('X_geo').textContent = data.x || 0;
                    document.getElementById('Y_geo').textContent = data.y || 0;
                    document.getElementById('Z_geo').textContent = data.z || 0;
                    batteryDisplay.textContent = data.battery_status ? data.battery_status + '%' : '0%';
                })
                .catch(error => console.error('Error fetching status:', error));
        }, 1000); // Cập nhật mỗi giây
    </script>
</body>
</html>
)=====";

// Function to serve the HTML page
void handleRoot() {
  server.send(200, "text/html", HTML_PAGE);
}

// Handle control updates from the web interface
void handleControl() {
  if (server.method() == HTTP_POST) {
    if (server.hasArg("plain")) {
      String json = server.arg("plain");
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, json);

      if (error) {
        server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
      }

      throttle = doc["throttle"];
      yaw = doc["yaw"];
      pitch = doc["pitch"];
      roll = doc["roll"];

      Serial.println("Received control data from web:");
      Serial.print("Throttle: "); Serial.println(throttle);
      Serial.print("Yaw: ");     Serial.println(yaw);
      Serial.print("Pitch: ");   Serial.println(pitch);
      Serial.print("Roll: ");    Serial.println(roll);

      newDataToSend = true; // Set flag to indicate new data is available

      server.send(200, "application/json", "{\"status\":\"ok\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"No JSON data\"}");
    }
  } else {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
  }
}

// Function to handle MQTT status
void handleMQTTStatus() {
  StaticJsonDocument<100> doc;
  if (client.connected()) {
    doc["status"] = "Connected";
  } else {
    doc["status"] = "Disconnected";
  }
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Function to handle status data to be fetched by web interface
void handleStatus() {
  StaticJsonDocument<200> doc;
  doc["x"] = pitch_recv;
  doc["y"] = roll_recv;
  doc["z"] = 0; // Bạn muốn giữ nguyên, nhưng sẽ loại bỏ khi publish lên MQTT
  doc["battery_status"] = voltage_recv;
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Reconnect to MQTT broker if disconnected
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Nếu cần subscribe topic nào đó, bạn có thể thêm vào đây
      // client.subscribe("drone/transfer");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Callback function to handle incoming MQTT messages
void callback(char* topic, byte* message, unsigned int length) {
  // Handle messages from MQTT broker if needed
}

void setup_wifi() {
  // Initialize Wi-Fi in AP+STA mode
  WiFi.mode(WIFI_AP_STA);

  // Start Wi-Fi Access Point
  WiFi.softAP(ap_ssid, ap_password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  Serial.println("Access Point started");

  // Connect to public Wi-Fi network (Station mode)
  Serial.print("Connecting to ");
  Serial.println(sta_ssid);
  WiFi.begin(sta_ssid, sta_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected in STA mode");
}

void setup() {
  Serial.begin(115200);

  // Initialize UART for CC2530 communication
  vietnam.begin(115200, SERIAL_8N1, RXpin, TXpin);

  // Set up Wi-Fi
  setup_wifi();

  // Set up MQTT client
  espClient.setCACert(root_ca);  // Set your CA certificate here
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Set up web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/control", HTTP_POST, handleControl);
  server.on("/mqtt_status", HTTP_GET, handleMQTTStatus);
  server.on("/status", HTTP_GET, handleStatus);

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient(); // Handle web server client

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  static unsigned long lastSendTime = 0;
  unsigned long currentTime = millis();

  // Send data via UART at regular intervals or when new data is available
  if (currentTime - lastSendTime >= 50 || newDataToSend) {
    sendData(throttle, pitch, roll, yaw);
    lastSendTime = currentTime;
    newDataToSend = false; // Reset the flag
  }

  // Receive data from UART and publish to MQTT
  receiveAndPublishData();
}

void receiveAndPublishData() {
  static DataPacketReceive packet;
  static uint8_t *ptr = (uint8_t*)&packet;
  static size_t index = 0;
  static bool receiving = false;

  while (vietnam.available()) {
    uint8_t byteReceived = vietnam.read();

    if (!receiving) {
      if (byteReceived == 0xAB) { // Start byte for received data
        ptr[0] = byteReceived;
        index = 1;
        receiving = true;
      }
    } else {
      ptr[index++] = byteReceived;

      if (index == sizeof(DataPacketReceive)) {
        receiving = false;
        index = 0;

        // Verify end byte
        if (packet.endByte == 0xBA) {
          // Process the received data
          pitch_recv = packet.pitch_recev;
          roll_recv = packet.roll_recev;
          voltage_recv = packet.voltage_recev;

          Serial.println("Received Data:");
          Serial.print("Pitch Received: "); Serial.println(pitch_recv);
          Serial.print("Roll Received: ");  Serial.println(roll_recv);
          Serial.print("Voltage Received: "); Serial.println(voltage_recv);

          // Create JSON payload with pitch_recv, roll_recv, voltage_recv
          StaticJsonDocument<256> doc;
          doc["pitch_recv"] = pitch_recv;
          doc["roll_recv"] = roll_recv;
          doc["voltage_recv"] = voltage_recv;

          char payload[256];
          size_t n = serializeJson(doc, payload);

          // Publish to MQTT (loại bỏ z)
          if (client.publish(mqtt_topic, payload, n)) {
            Serial.println("Data published to MQTT");
          } else {
            Serial.println("Failed to publish data to MQTT");
          }

        } else {
          Serial.println("Invalid end byte in received data");
        }
      }
    }
  }
}