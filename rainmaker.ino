#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"

//SET DEFAULT
#define DEFAULT_VOLTAGE 12.6
#define DEFAULT_ROLL 0
#define DEFAULT_PITCH 0

//init control variables
static float voltage = DEFAULT_VOLTAGE;
static int roll_ = DEFAULT_ROLL;
static int pitch_ = DEFAULT_PITCH;

// BLE Credentils
const char *service_name = "PROV_1357";
const char *pop = "abcd1234";

bool wifi_connected = 0;
//GPIO for push button reset
static int gpio_0 = 0;

float generateData(float minValue, float maxValue) {
    return minValue + (float)rand() / (RAND_MAX / (maxValue - minValue));
}
//The framework provides some standard device types like switch, lightbulb, fan, temperature sensor.
Device  roll("Roll", "custom.device.roll_");
Device  pitch("Pitch","custom.device.pitch_");
Device  battery("Battery","custom.device.battery");

void sysProvEvent(arduino_event_t *sys_event)
{
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
  #if CONFIG_IDF_TARGET_ESP32
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
        printQR(service_name, pop, "ble");
  #else
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
        printQR(service_name, pop, "softap");
  #endif
        break;
  }
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
  const char *device_name = device->getDeviceName();
  const char *param_name = param->getParamName();

  if (strcmp(param_name, "Voltage") == 0) {
    Serial.printf("Received value = %0.2f for %s - %s\n", val.val.f, device_name, param_name);
    voltage = val.val.f;
    param->updateAndReport(val);
  }

  if (strcmp(param_name, "Roll_Val") == 0) {
    Serial.printf("Received value = %d for %s - %s\n", val.val.i, device_name, param_name);
    roll_=val.val.i;
    param->updateAndReport(val);
  }
  if (strcmp(param_name, "Pitch_Val") == 0) {
    Serial.printf("Received value = %d for %s - %s\n", val.val.i, device_name, param_name);
    pitch_ = val.val.i;
    param->updateAndReport(val);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(gpio_0, INPUT);
  //------------------------------------------- Declaring Node -----------------------------------------------------//
  Node my_node;
  my_node = RMaker.initNode("Droneeee");

  //Add param
  Param voltageParam("Voltage (V)", "custom.param.voltage", value((float)DEFAULT_VOLTAGE), PROP_FLAG_READ );
  battery.addParam(voltageParam);
  battery.assignPrimaryParam(battery.getParamByName("Voltage (V)"));

  Param rollParam("Roll_Val", "custom.param.roll", value((float)DEFAULT_ROLL), PROP_FLAG_READ );
  roll.addParam(rollParam);
  roll.assignPrimaryParam(roll.getParamByName("Roll_Val"));

  Param pitchParam("Pitch_Val", "custom.param.pitch", value((float)DEFAULT_PITCH), PROP_FLAG_READ );
  pitch.addParam(pitchParam);
  pitch.assignPrimaryParam(pitch.getParamByName("Pitch_Val"));


  //Callback
  
  roll.addCb(write_callback);
  pitch.addCb(write_callback);
  battery.addCb(write_callback);

  //------------------------------------------- Adding Devices in Node -----------------------------------------------------//
  my_node.addDevice(battery);
  my_node.addDevice(pitch);
  my_node.addDevice(roll);


  //This is optional
  RMaker.enableOTA(OTA_USING_PARAMS);
  RMaker.enableTZService();
  RMaker.enableSchedule();

  Serial.printf("\nStarting ESP-RainMaker\n");
  RMaker.start();

  // Timer for Sending Sensor's Data
  // Timer.setInterval(1000);
  WiFi.onEvent(sysProvEvent);
  #if CONFIG_IDF_TARGET_ESP32
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
  #else
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
  #endif
}

void loop()
{
  if (digitalRead(gpio_0) == LOW) { //Push button pressed

    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(gpio_0) == LOW) delay(50);
    int endTime = millis();

    if ((endTime - startTime) > 10000) {
      // If key pressed for more than 10secs, reset all
      Serial.printf("Reset to factory.\n");
      RMakerFactoryReset(2);
    } else if ((endTime - startTime) > 3000) {
      Serial.printf("Reset Wi-Fi.\n");
      // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
      RMakerWiFiReset(2);
    } else {
      // Toggle device state??
    }
  }
  voltage = generateData(10.0, 12.6); // Random battery voltage (10.0V to 12.6V)
  roll_ = generateData(-180.0, 180.0); // Random roll (-180° to 180°)
  pitch_ = generateData(-90.0, 90.0);  // Random pitch (-90° to 90°)

  //Update and report new values
  
  roll.updateAndReportParam("Roll_Val", (float)roll_);
  pitch.updateAndReportParam("Pitch_Val", (float)pitch_);
  battery.updateAndReportParam("Voltage (V)", voltage);

  Serial.printf("Battery: %.2f V, Roll: %.2f, Pitch: %.2f\n", voltage, (float)roll_, (float)pitch_);
  delay(250);
}


// #include "RMaker.h"
// #include "WiFi.h"
// #include "WiFiProv.h"

// /*
//    Default Settings
// */
// #define DEFAULT_VOLTAGE 24.1
// #define DEFAULT_MIN_VOLTAGE 22.0
// #define DEFAULT_MAX_VOLTAGE 28.0
// #define DEFAULT_MAX_DELTA 50
// #define DEFAULT_CHARGE_ENABLE true
// #define DEFAULT_LOAD_ENABLE true

// /*
//    Init control variables
// */
// static float minVoltage = DEFAULT_MIN_VOLTAGE;
// static float maxVoltage = DEFAULT_MAX_VOLTAGE;
// static bool chargeEnable = DEFAULT_CHARGE_ENABLE;
// static bool loadEnable = DEFAULT_LOAD_ENABLE;
// static int maxDelta = DEFAULT_MAX_DELTA;

// /*
//     Pins
// */
// static int pchargeEnable = 12;
// static int ploadEnable = 19;
// //GPIO for push button reset
// static int gpio_0 = 0;

// /*
//    Provisioning Service Name
// */
// const char *service_name = "PROV_1357";
// const char *pop = "abcd1234";

// /*
//    Create space for the Custom Device
// */
// static Device battery("Battery", "custom.device.battery");

// /*
//    Say how you want provisioning done
// */
// void sysProvEvent(arduino_event_t *sys_event)
// {
//   switch (sys_event->event_id) {
//     case ARDUINO_EVENT_PROV_START:
// #if CONFIG_IDF_TARGET_ESP32
//       Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
//       printQR(service_name, pop, "ble");
// #else
//       Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
//       printQR(service_name, pop, "softap");
// #endif
//       break;
//   }
// }

// void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
// {
//   const char *device_name = device->getDeviceName();
//   const char *param_name = param->getParamName();

//   if (strcmp(param_name, "Low Voltage") == 0) {
//     Serial.printf("Received value = %2.2f for %s - %s\n", val.val.f, device_name, param_name);
//     minVoltage = val.val.f;
//     param->updateAndReport(val);
//   }

//   else if (strcmp(param_name, "High Voltage") == 0) {
//     Serial.printf("Received value = %2.2f for %s - %s\n", val.val.f, device_name, param_name);
//     param->updateAndReport(val);
//   }

//   else if (strcmp(param_name, "Charge Enable") == 0) {
//     Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
//     chargeEnable = val.val.b;
//     param->updateAndReport(val);
//     digitalWrite(pchargeEnable, chargeEnable);
//   }
//   else if (strcmp(param_name, "Load Enable") == 0) {
//     Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
//     loadEnable = val.val.b;
//     param->updateAndReport(val);
//     digitalWrite(ploadEnable, loadEnable);
//   }
//   else if (strcmp(param_name, "Max Delta (mV)") == 0) {
//     Serial.printf("Received value = %d for %s - %s\n", val.val.i, device_name, param_name);
//     maxDelta = val.val.i;
//     param->updateAndReport(val);
//   }
// }

// void setup()
// {
//   Serial.begin(115200);

//   pinMode(gpio_0, INPUT);
//   pinMode(ploadEnable, OUTPUT);
//   pinMode(pchargeEnable, OUTPUT);

//   /*
//      Setup the default Pin states
//   */
//   digitalWrite(ploadEnable, DEFAULT_LOAD_ENABLE);
//   digitalWrite(pchargeEnable, DEFAULT_CHARGE_ENABLE);

//   /*
//      Create the Node
//   */
//   Node my_node;
//   my_node = RMaker.initNode("BMS Node");

//   /*
//      Create and add custom voltage parameters
//   */
//   Param voltageParam("Voltage (V)", "custom.param.voltage", value((float)DEFAULT_VOLTAGE), PROP_FLAG_READ );
//   battery.addParam(voltageParam);
//   battery.assignPrimaryParam(battery.getParamByName("Voltage (V)"));

//   Param deltaParam("Delta (mV)", "custom.param.dvoltage", value((int)DEFAULT_MAX_DELTA - 10), PROP_FLAG_READ );
//   battery.addParam(deltaParam);

//   Param lowVoltageParam("Low Voltage", "custom.param.lvoltage", value((float)DEFAULT_MIN_VOLTAGE), PROP_FLAG_WRITE );
//   battery.addParam(lowVoltageParam);

//   Param highVoltageParam("High Voltage", "custom.param.hvoltage", value((float)DEFAULT_MAX_VOLTAGE), PROP_FLAG_WRITE );
//   battery.addParam(highVoltageParam);

//   Param maxDeltaParam("Max Delta (mV)", "custom.param.maxdelta", value((int)DEFAULT_MAX_DELTA), PROP_FLAG_WRITE );
//   battery.addParam(maxDeltaParam);

//   /*
//      Now add the switches
//   */
//   Param Charge_Enable("Charge Enable", "custom.param.enable", value(DEFAULT_CHARGE_ENABLE), PROP_FLAG_READ | PROP_FLAG_WRITE);
//   Charge_Enable.addUIType(ESP_RMAKER_UI_TOGGLE);
//   battery.addParam(Charge_Enable);

//   Param Load_Enable("Load Enable", "custom.param.enable", value(DEFAULT_LOAD_ENABLE), PROP_FLAG_READ | PROP_FLAG_WRITE);
//   Load_Enable.addUIType(ESP_RMAKER_UI_TOGGLE);
//   battery.addParam(Load_Enable);

//   /*
//      Now point at the callback code so we know what to do when something happens
//   */
//   battery.addCb(write_callback);

//   /*
//      Add custom battery device to the node
//   */
//   my_node.addDevice(battery);

//   /*
//      This is optional for OTA updates
//   */
//   RMaker.enableOTA(OTA_USING_PARAMS);

//   /*
//      Start the Rainmaker Service
//   */
//   RMaker.start();

//   /*
//      Provisioning via BLE or WiFI
//   */
//   WiFi.onEvent(sysProvEvent);
// #if CONFIG_IDF_TARGET_ESP32
//   WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
// #else
//   WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
// #endif
// }

// void loop()
// {
//   if (digitalRead(gpio_0) == LOW) { //Push button pressed

//     // Key debounce handling
//     delay(100);
//     int startTime = millis();
//     while (digitalRead(gpio_0) == LOW) delay(50);
//     int endTime = millis();

//     if ((endTime - startTime) > 10000) {
//       // If key pressed for more than 10secs, reset all
//       Serial.printf("Reset to factory.\n");
//       RMakerFactoryReset(2);
//     } else if ((endTime - startTime) > 3000) {
//       Serial.printf("Reset Wi-Fi.\n");
//       // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
//       RMakerWiFiReset(2);
//     } else {
//       // Toggle device state??
//     }
//   }
//   delay(100);
// }
