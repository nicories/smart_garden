#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <multiplexer.h>
#include <uri/UriBraces.h>

const char *ssid = "<SSID>";
const char *password = "<PASSWORD>";

ESP8266WebServer server(80);

const int led = LED_BUILTIN;

const int GPIO = A0;

int controls[2] = {D0, D1};

Multiplexer multiplexer = Multiplexer(controls, D0);

int sensor_data = 0;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

int analog_multiplex_16(unsigned int index) {

  if (index > 15) {
    return -1;
  }
  digitalWrite(D0, index & 0b1);
  digitalWrite(D1, (index & 0b10) >> 1);
  digitalWrite(D2, (index & 0b100) >> 2);
  digitalWrite(D3, (index & 0b1000) >> 3);

  return analogRead(A0);
}

void setup(void) {
  pinMode(A0, INPUT);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);

  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on(UriBraces("/sensors/{}"), []() {
    String sensor_index = server.pathArg(0);
    sensor_data = analog_multiplex_16(sensor_index.toInt());
    server.send(200, "text/plain",
                "Sensor " + sensor_index + " : " + String(sensor_data));
  });

  server.on("/inline",
            []() { server.send(200, "text/plain", "this works as well"); });

  server.onNotFound(
      []() { server.send(404, "text/plain", "nothing to see here"); });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) { server.handleClient(); }
