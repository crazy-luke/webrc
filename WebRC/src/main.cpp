#include "LittleFS.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include "CrazyMoto.cpp"

#define HANDSERVO_PIN D1
#define WHEELSERVO_PIN D2
#define LED_PIN D3
#define PIN_AUDIO D7

CrazyMoto moto(HANDSERVO_PIN, WHEELSERVO_PIN, LED_PIN, PIN_AUDIO);
AsyncWebServer server(80);

const char *ssid = "crazymoto-black";
const char *pw = "";

const bool DEBUG = true;
void print(String msg)
{
  if (DEBUG)
  {
    Serial.println(msg);
  }
}

void initWiFi()
{
  WiFi.mode(WIFI_AP);
  IPAddress ip(192, 168, 0, 1);
  IPAddress netmask(255, 255, 255, 0);
  WiFi.softAPConfig(ip, ip, netmask);
  bool result = WiFi.softAP(ssid, pw);
  if (result)
  {
    print("WiFi init Ok");
  }
  else
  {
    print("WiFi failed");
  }
}

void initWebServer()
{
  if (!LittleFS.begin())
  {
    print("initWebServer failed: LittleFS!!!!");
    return;
  }
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    request->send(LittleFS,"index.html","text/html");
    print("send html"); });
    
  server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/bootstrap.min.css", "/bootstrap.min.css"); });
  server.serveStatic("/favicon.ico", LittleFS, "/favicon.ico");
  server.serveStatic("/bootstrap.min.js", LittleFS, "/bootstrap.min.js");
  server.serveStatic("/jquery.min.js", LittleFS, "/jquery.min.js");
  server.serveStatic("/virtualjoystick.js", LittleFS, "/virtualjoystick.js");
  server.serveStatic("/joystick.html", LittleFS, "/joystick.html");

  server.on("/crazyHandle", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    String angle = request->arg("angle");
    print("handangle:"+angle);
    moto.doHandle(angle.toInt());
    request->send(200); });

  server.on("/crazyWheel", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    String angle = request->arg("angle");
    print("wheelangle:"+angle);
    moto.doWheel(angle.toInt());
    request->send(200); });

  server.on("/btnAct", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    String act = request->arg("act");
    print("act:"+act);
    if(act.equalsIgnoreCase("forward")){
      moto.doAct(CrazyMoto::Act::Forward);
    }else if(act.equalsIgnoreCase("standby")){
      moto.doAct(CrazyMoto::Act::Standby);
    }else if(act.equalsIgnoreCase("backward")){
      moto.doAct(CrazyMoto::Act::Backward);
    }else if(act.equalsIgnoreCase("led")){
       moto.doAct(CrazyMoto::Act::LedSwitch);
    }else if(act.equalsIgnoreCase("speaker")){
      moto.doAct(CrazyMoto::Act::Horn);
    }
    request->send(200); });

  server.on("/joystick", HTTP_PUT, [](AsyncWebServerRequest *request)
            {
    String xPostion = request->arg("x");
    String yPostion = request->arg("y");
    String xReverse = request ->arg("xReverse");
    String yAction = request ->arg("yAction");

    print("x:"+ xPostion + "y:"+yPostion + "xReverse:"+xReverse+ "yAction:"+yAction);

    int x = xPostion.toInt();
    int y = yPostion.toInt();
    if(yAction.toInt()==1){
      if(y>=90) {
        moto.doAct(CrazyMoto::Act::Forward);
      }else if(y<=-90){
        moto.doAct(CrazyMoto::Act::Backward);
      }
    }
    int handAngle = xReverse.toInt()==1 ? map(x,-100,100,0,180) : map(x,-100,100,180,0);
    moto.doHandle(handAngle);
    print("handAngle:"+String(handAngle));

    request->send(200); });

  server.begin();
  print("initWebServer OK");
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  print("Start...");
  //moto.init(HANDSERVO_PIN, WHEELSERVO_PIN, LED_PIN, PIN_AUDIO);
  initWiFi();
  initWebServer();

  moto.doAct(CrazyMoto::Act::LedRGB);
  print("Ready!");
}

void loop()
{
}