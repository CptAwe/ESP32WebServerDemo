// Load Wi-Fi library
#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <SPIFFS.h>

// Replace with your network credentials
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

// Set web server port number to 80
AsyncWebServer server(80);

// Variable to store the HTTP request
String header;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <h2>Happy Birthday!</h2>
  <img src="img">
</body>  
</html>)rawliteral";

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);
  
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // The index
  server.on(
    "/",
    HTTP_GET,
    [](AsyncWebServerRequest *request) {
      request->send_P(200, "text/html", index_html);
    }
  );

  // The image
  server.on(
    "/img",
    HTTP_GET,
    [](AsyncWebServerRequest *request) {
      request->send(SPIFFS, "/qr.png", "image/png");
    }
  );
  
  server.begin();
}

void loop(){
  
}
