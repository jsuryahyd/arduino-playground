/*
  Arduino UNO R4 WiFi - Web Server Dashboard Starter
  
  This starter sketch connects your Uno R4 WiFi to your Wi-Fi network and
  hosts a lightweight web dashboard accessible from your phone or browser!
*/

#include "WiFiS3.h"
#include "Arduino_LED_Matrix.h"
#include "secrets.h"

// Load Wi-Fi credentials from secrets.h
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;

int status = WL_IDLE_STATUS;
WiFiServer server(80);
ArduinoLEDMatrix matrix;

void setup() {
  Serial.begin(115200);
  matrix.begin();
  matrix.loadFrame(LEDMATRIX_CLOUD_WIFI);

  Serial.println("Arduino UNO R4 WiFi Dashboard Starter");
  
  // Check for Wi-Fi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with Wi-Fi module failed!");
    while (true);
  }

  // Attempt to connect to Wi-Fi network
  while (status != WL_CONNECTED) {
    Serial.print("Connecting to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  server.begin();
  Serial.println("Wi-Fi connected successfully!");
  Serial.print("Web server IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Web Client Connected!");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // HTTP response headers
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // HTML Web Page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>body{font-family:Arial;text-align:center;background:#121212;color:#fff;padding:40px;}</style></head>");
            client.println("<body><h1>Arduino UNO R4 WiFi</h1>");
            client.println("<p>Board Status: <b>Online</b></p>");
            client.println("<p>Uptime: " + String(millis() / 1000) + " seconds</p>");
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected.");
  }
}
