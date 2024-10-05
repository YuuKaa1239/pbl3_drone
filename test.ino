// #include <WiFi.h>
// #include <PubSubClient.h>
#include <ArduinoJson.h>
// #include <WiFiClientSecure.h>

// // Wi-Fi configuration
// const char* ssid = "P204 / 209";
// const char* password = "30041975";

// // MQTT broker configuration
// const char* mqtt_server = "8b28df54d30c4a2b8d0d33b102030c12.s1.eu.hivemq.cloud";
// const int mqtt_port = 8883;
// const char* mqtt_user = "peppa";  // MQTT username
// const char* mqtt_pass = "Peppa_pig1";  // MQTT password

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

// // Wi-Fi and MQTT client objects
// WiFiClientSecure espClient;
// PubSubClient client(espClient);

// // MQTT topic
// const char* topic = "drone/transfer";

// void callback(char* topic, byte* payload, unsigned int length) {
//   Serial.print("Message arrived on topic: ");
//   Serial.println(topic);

//   String message;
//   for (int i = 0; i < length; i++) {
//     message += (char)payload[i];
//   }

//   // Display the received message
//   Serial.println("Payload: " + message);

//   // Parse JSON
//   StaticJsonDocument<256> doc; // Increased size to accommodate more data
//   DeserializationError error = deserializeJson(doc, message);

//   if (error) {
//     Serial.print("deserializeJson() failed: ");
//     Serial.println(error.c_str());
//     return;
//   }

//   // Get values from JSON
//   float pitch = doc["pitch"];
//   float yaw = doc["yaw"];
//   float roll = doc["roll"];
//   float throttle = doc["throttle"];

//   // Display the values
//   Serial.print("Pitch: ");
//   Serial.println(pitch);
//   Serial.print("Yaw: ");
//   Serial.println(yaw);
//   Serial.print("Roll: ");
//   Serial.println(roll);
//   Serial.print("Throttle: ");
//   Serial.println(throttle);

//   // TODO: Add your code here to handle the roll and throttle values
// }

// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");

//     String clientId = "ESP32Client-";
//     clientId += String(random(0xffff), HEX);

//     if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
//       Serial.println("Connected");
//       client.subscribe(topic);
//     } else {
//       Serial.print("Failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" try again in 5 seconds");
//       delay(5000);
//     }
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   setup_wifi();
//   client.setServer(mqtt_server, mqtt_port);
//   client.setCallback(callback);
// }

// void setup_wifi() {
//   delay(10);
//   Serial.println();
//   Serial.print("Connecting to ");
//   Serial.println(ssid);

//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("");
//   Serial.println("WiFi connected");
//   Serial.println("IP address: ");
//   Serial.println(WiFi.localIP());
// }

// void loop() {
//   if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();
// }


#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char* ssid = "P204 / 209";
const char* password = "30041975";

const char* mqtt_server = "8b28df54d30c4a2b8d0d33b102030c12.s1.eu.hivemq.cloud";
const int mqtt_port = 8883; // TLS MQTT port
const char* mqtt_user = "peppa";
const char* mqtt_password = "Peppa_pig1";

WiFiClientSecure espClient;
PubSubClient client(espClient);

// Topic where yaw, pitch, roll, throttle data will be sent
// const char* topic_yaw = "drone/yaw";
// const char* topic_pitch = "drone/pitch";
// const char* topic_roll = "drone/roll";
// const char* topic_throttle = "drone/throttle";
const char* topic = "drone/transfer";
const char* topic_data = "drone/data";

void publishData(float x, float y, float z, int battery_status);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Subscribe to the topics for yaw, pitch, roll, throttle
      client.subscribe(topic);
      // client.subscribe(topic_pitch);
      // client.subscribe(topic_roll);
      // client.subscribe(topic_throttle);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Handle the data based on the topic
  // if (String(topic) == topic_yaw) {
  //   Serial.println("Received Yaw: " + messageTemp);
  //   // Add logic for yaw control
  // } else if (String(topic) == topic_pitch) {
  //   Serial.println("Received Pitch: " + messageTemp);
  //   // Add logic for pitch control
  // } else if (String(topic) == topic_roll) {
  //   Serial.println("Received Roll: " + messageTemp);
  //   // Add logic for roll control
  // } else if (String(topic) == topic_throttle) {
  //   Serial.println("Received Throttle: " + messageTemp);
  //   // Add logic for throttle control
  // }
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Get values from JSON
  float pitch = doc["pitch"];
  float yaw = doc["yaw"];
  float roll = doc["roll"];
  float throttle = doc["throttle"];

  // Display the values
  Serial.print("Pitch: ");
  Serial.println(pitch);
  Serial.print("Yaw: ");
  Serial.println(yaw);
  Serial.print("Roll: ");
  Serial.println(roll);
  Serial.print("Throttle: ");
  Serial.println(throttle);
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  
  espClient.setCACert(root_ca);  // Set your CA certificate here if needed
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Example values for x, y, z, and battery_status
  float x = random(0,1000)/10.0;  // Replace with your sensor value
  float y = random(0,1000)/10.0;  // Replace with your sensor value
  float z = random(0,1000)/10.0;  // Replace with your sensor value
  int battery_status = random(0,101);  // Replace with your actual battery status

  // Send data periodically (adjust the delay as needed)
  publishData(x, y, z, battery_status);
  // delay(5000);
}


void publishData(float x, float y, float z, int battery_status) {
  // Create a JSON-like string with the data
  String payload = "{\"x\": " + String(x) + ", \"y\": " + String(y) + ", \"z\": " + String(z) + ", \"battery_status\": " + String(battery_status) + "}";
  
  // Publish the payload to the topic
  if (client.publish(topic_data, payload.c_str())) {
    Serial.println("Data published successfully");
  } else {
    Serial.println("Data publish failed");
  }
}
