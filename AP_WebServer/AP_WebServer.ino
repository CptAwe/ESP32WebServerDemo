// Load Wi-Fi library
#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <SPIFFS.h>

#define BUILTIN_LED 2

const char* SSID     = "ESP32-Access-Point";
const char* PASSWORD = "123456789";

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
  <h2>Hello!</h2>
  <img src="img" style="max-width: 100%; height: auto">

  <p>
    <button id="toggleledbtn" type="button">Flash Me!</button>
  </p>

  <h2>How I am made:</h2>
  <img src="gitqr" style="max-width: 100%; height: auto">

  <script>
    var button = document.getElementById('toggleledbtn');
    const btn_txt_off = "Flash Me!";
    const btn_txt_on = "Ah! Stop!";

    button.addEventListener('click', async _ => {
      try {     
        const response = await fetch('/toggleled', {
          method: 'post'
        });
        const text = await response.text()
        if (text == "ON") {
          button.innerHTML = btn_txt_on;
        } else {
          button.innerHTML = btn_txt_off;
        }
      } catch(err) {
        console.error(`Error: ${err}`);
      }
    });
  </script>
</body>  
</html>)rawliteral";

bool led_state=false;

void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, led_state);// Turn off the LED

  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(SSID, PASSWORD);
  
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
  
  // The qr for the git repo link
  server.on(
    "/gitqr",
    HTTP_GET,
    [](AsyncWebServerRequest *request) {
      request->send(SPIFFS, "/gitrepo.png", "image/png");
    }
  );

  // Toggle the on board LED
  server.on(
    "/toggleled",
    HTTP_POST,
    [](AsyncWebServerRequest *request) {
      led_state = !led_state;
      digitalWrite(BUILTIN_LED, led_state);
      
      // Respond with the current state of the LED
      if (led_state){
        request->send(200, "text/plain", "ON");
      } else {
        request->send(200, "text/plain", "OFF");
      }
    }
  );

  server.begin();
}

void loop(){
  
}
