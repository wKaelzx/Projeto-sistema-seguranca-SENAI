#include <Arduino.h>
#include "WiFiManager.h"
#include "MQTTManager.h"
#include "DebugManager.h"
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <LED.h>
#include <LiquidCrystal_I2C.h>

const int PIN_LED_RGB = 48;
const int QNTD_LEDS = 1;
const char TOPICO_COMANDO[] = "esp32/comando";
const int PIN_LAMPADA = 46;
const int PIN_BOTAO = 0;
bool movimento = false;

LED Lampada(PIN_LAMPADA);

Adafruit_NeoPixel ledRGB(
    QNTD_LEDS,
    PIN_LED_RGB,
    NEO_GRB + NEO_KHZ800 // --> Igual a 82
);

LiquidCrystal_I2C lcd(0x27, 20, 4);

/*
Nome:
Data:
Programa:
Descrição:
Versão: 1.0
*/

void tratarMensagemRecebida(const char *topico, const String &mensagem);
void configurarLedRGB();
void alterarCorLedRGB(int red, int green, int blue);
void tratarJsonComando(const String &mensagem);
void alterarLampada(bool estado);
void chaveReset();

void setup()
{

  lcd.init();
  lcd.backlight();

  configurarDebug();
  conectarWiFi();
  configurarMQTT();
  registrarCallbackMensagem(tratarMensagemRecebida);
  conectarMQTT();
  pinMode(PIN_LAMPADA, OUTPUT);

}

void loop()
{
  garantirWiFiConectado();
  garantirMQTTConectado();
  MQTTLoop();
  Lampada.update();
  ledRGB.show();
  static bool looped = false;
 if (movimento && !looped)
 {
        debugInfo("Movimento recebido e processado");
       	alterarCorLedRGB(0,255,0);
	debugInfo("Acendendo lampada");
	Lampada.acender();
	looped = true;

 }

 // TODO
 //
 // if(botao.fall()){
 //	debugInfo("Botao reset do sistema de seguranca foi pressionado");
 //	debugInfo("Reiniciando sistema...");
 //	movimento = false;
 //	looped = false;
 // }
}

void tratarMensagemRecebida(const char *topico, const String &mensagem)
{
  debugInfo("=======================================");
  debugInfo("Mensagem recebida da aplicação");
  debugInfo("=======================================");

  if (topico == nullptr)
  {
    debugErro("Tópico MQTT inválido");
    return;
  }

  debugInfo("Tópico: " + String(topico));
  debugInfo("Mensagem: " + mensagem);

  if (strcmp(topico, TOPICO_COMANDO) == 0)
  {
    tratarJsonComando(mensagem);
    return;
  }
  debugErro("Tópico não tratado: " + String(topico));
}

void configurarLedRGB()
{

  ledRGB.begin();
  ledRGB.setBrightness(80);
  ledRGB.clear();
  ledRGB.show();
  debugInfo("LED RGB configurado no GPIO " + String(PIN_LED_RGB));
}

void alterarCorLedRGB(int vermelho, int verde, int azul)
{
  constrain(vermelho, 0, 255);
  constrain(verde, 0, 255);
  constrain(azul, 0, 255);

  ledRGB.setPixelColor(0, ledRGB.Color(vermelho, verde, azul));
  ledRGB.show();
  debugInfo("Cor aplicada no LED RGB");
  debugInfo("R: " + String(vermelho));
  debugInfo("G: " + String(verde));
  debugInfo("B: " + String(azul));
}

void tratarJsonComando(const String &mensagem)
{
  JsonDocument doc;
  DeserializationError erro = deserializeJson(doc, mensagem);
  if (erro)
  {
    debugErro("Erro ao deserializar JSON, código: " + String(erro.c_str()));
    return;
  }
  if (!doc["sensor_virtual"].is<JsonObject>())
  {
    debugErro("JSON inválido, não contém chave para 'sensor_virtual' ");
    return;
  }
  if (!doc["sensor_virtual"].is<bool>()
  {
  // 0 - vigiando
  // 1 - detectado
  	if(doc["led"].as<bool>)
	{
		debugInfo("Movimento recebido e processado")
		movimento = true;
	}
  }
  else
  {
  	debugErro("Tipo improprio para chave JSON: sensor_virtual")
	return;
  }
  
}

void alterarLampada(bool estado)
{
  if (estado)
  {
    debugInfo("Ligando lampada");
    digitalWrite(PIN_LAMPADA, HIGH);
  }
  else
  {
    debugInfo("Desligando lampada");
    digitalWrite(PIN_LAMPADA, LOW);
  }
}

void loopDisplay()
{ // com movimento
  if (movimento)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Movimento detectado");
  }
  else
  {
    lcd.clear()
    lcd.setCursor(0, 0);
    lcd.print("Sem movimento");
  }
}

void chaveReset(){
// TODO  Funcao para a senha e botao, precisamos importar a biblioteca bounce
}
/*
 FORMATO JSON
*/
