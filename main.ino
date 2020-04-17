
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <Ticker.h>
#include "eeprom_.h"

//Name and Password for hotspot network
const char *local_ssid = "ESPap";
const char *local_password = "00000000";

//Blynk auth code
char auth[] = "XytyN4EYgCAPcBLX-XuUylkbV1m2Lw72";

bool blynk_connected = false;
bool hasInternet = true;
bool ledState = false;
bool wifiConnected = false;

char ssid[32] = "Default";
char pass[32] = "Default";

int time_counter = 0;

char *host = "google.com";



ESP8266WebServer server(80);

#define TURN_OFF 0
#define TURN_ON 1
#define TOGGLE 2

BLYNK_WRITE(V1)
{
  int pin_value = param.asInt();
  turn_on_off(1, pin_value);
}

void turn_on_off(int source, int command)
{
  Serial.print("Source ");
  Serial.print(source);
  Serial.print(" Is ");
  if (command == TURN_ON)
  {
    write_char(source, TURN_ON);
    //TODO send with UART to MCU TURN ON
    server.send(200, "text/plain", "1");
    Serial.println("ON");
  }
  else if (command == TURN_OFF)
  {
    write_char(source, TURN_OFF);
    //TODO send with UART to MCU TURN OFF
    server.send(200, "text/plain", "0");
    Serial.println("OFF");
  }
  else if (command == TOGGLE)
  {
    if (read_char(source) == TURN_ON)
    {
      write_char(source, TURN_OFF);
      //TODO send with UART to MCU TURN OFF
      server.send(200, "text/plain", "0");
      Serial.println("OFF");
    }
    else
    {
      write_char(source, TURN_ON);
      //TODO send with UART to MCU TURN ON
      server.send(200, "text/plain", "1");
      Serial.println("ON");
    }
  }
  else if (command == 3)
  {
    char status_ = send_status(source);
    if (status_ == 1)
      Serial.println("ON");
    else
      Serial.println("OFF");
  }
}

char send_status(char source)
{
  char status_ = read_char(source);
  if (status_ == TURN_ON)
    server.send(200, "text/plain", "1");
  else
    server.send(200, "text/plain", "0");
  return status_;
}

void handleRoot()
{
  server.send(200, "text/html", "<h1>You are connected</h1>");
}



void handle_command()
{ 
  int source = server.arg(0).toInt();
  int command = server.arg(1).toInt();
  turn_on_off(source, command);
}

void handle_settings()
{ 
  String ssid = server.arg(0);
  String password = server.arg(1);
  int ssidL = ssid.length();
  int passL = password.length();
  ssid = ssid + "                                                         ";
  password = password +  "                                                         ";
  char ssid_ [32];
  char password_ [32];
  ssid.toCharArray(ssid_ , 32);
  password.toCharArray(password_ , 32);
  write_ssid(ssid_);
  write_pass(password_);
  Serial.println();
  for(int i = 0; i<32; i++)
    Serial.print(ssid_[i]);
  Serial.println();
  for(int i = 0; i<32; i++)
    Serial.print(password_[i]);
  Serial.println();
}

bool check_internet_connection()
{
  WiFiClient client;

  int httpPort = 80;
  Serial.println(client.connect(host, httpPort));
  if (!client.connect(host, httpPort))
    hasInternet = false;
  else
    hasInternet = true;
  return hasInternet;
}




void setup()
{
  Serial.begin(9600);
  EEPROM.begin(512);
  //Etisalat-PtGA

  //write_ssid("Etisalat-PtGA");
  //write_pass("11223399");
  read_ssid(ssid);
  read_pass(pass);

  WiFi.begin(ssid, pass);
  for (int counter = 0; counter < 100; counter++)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.print("*");
      delay(100);
    }
    else
    {
      wifiConnected = true;
      Serial.println();
      Serial.println("Connected to wifi");
      break;
    }
  }

  if (!wifiConnected)
  {
    Serial.println("Error Connecting");
    Serial.println("Configuring access point...");
    WiFi.softAP(local_ssid, local_password);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    Serial.println("HTTP server started");
  }
  else
  {
    check_internet_connection();
    if (hasInternet)
    {
      Blynk.begin(auth, ssid, pass);
      blynk_connected = true;
    }
    else
    {
      Serial.println("No Internet Connection");
    }
  }
  server.on("/", handleRoot);
  server.on("/command", handle_command);
  server.on("/set", handle_settings);
  server.begin();


}
void loop()
{
  server.handleClient();
  if (hasInternet)
  {
    if (!blynk_connected)
    {
      Blynk.begin(auth, ssid, pass);
      blynk_connected = true;
    }
    Blynk.run();
  }

}
