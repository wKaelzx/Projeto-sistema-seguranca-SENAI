#include <Arduino.h>
#include "WiFiManager.h"
#include "MQTTManager.h"
#include "DebugManager.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

const int SDA_PIN = 8;
const int SCL_PIN = 9;
Adafruit_VL53L0X sensor = Adafruit_VL53L0X();

const char TOPICO_COMANDO[] = "sistema/estado";

void tratarJsonComando(const String &mensagem);
void processarJson(bool estado);
bool haMovimento();
void sensorConfig();

void setup()
{
  configurarDebug();
  conectarWiFi();
  configurarMQTT();
  conectarMQTT();
  sensorConfig();
}

void loop()
{
  garantirWiFiConectado();
  garantirMQTTConectado();
  MQTTLoop();
  bool mov = haMovimento();
  static bool pastMov = mov;
  if (mov != pastMov)
  {
    processarJson(mov);
  }
  pastMov = mov;
  delay(500);
}

/*
 FORMATO JSON
*/

bool haMovimento()
{
  VL53L0X_RangingMeasurementData_t measure;

  sensor.rangingTest(&measure, false);

  if (measure.RangeStatus != 4 && measure.RangeMilliMeter > 50 && measure.RangeMilliMeter < 5000)
  {
    debugInfo("Distancia (mm): " + String(measure.RangeMilliMeter));
    return true;
  }
  else
  {
    return false;
  }
}

void sensorConfig()
{
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!sensor.begin())
  {
    debugErro("Erro ao iniciar VL53L0X");
  }

  debugInfo("Sensor OK!");
}

void processarJson(bool estado)
{
  JsonDocument doc;
  String msg;
  doc["violacao"] = estado;
  serializeJson(doc, msg);
  publicarMensagem(TOPICO_COMANDO, msg.c_str());
}