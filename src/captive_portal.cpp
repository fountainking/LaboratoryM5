#include "captive_portal.h"
#include "ui.h"
#include "settings.h"
#include "portal_manager.h"

// Portal globals
PortalState portalState = PORTAL_STOPPED;
WebServer* portalWebServer = nullptr;
DNSServer* portalDNS = nullptr;
int portalVisitorCount = 0;
String portalSSID = "";
unsigned long portalStartTime = 0;

// Simple HTML page - placeholder for Laboratory pitch deck
const char PORTAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Laboratory</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
            margin: 0;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            min-height: 100vh;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        }
        .container {
            max-width: 600px;
            background: rgba(255,255,255,0.1);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            padding: 40px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.3);
        }
        h1 {
            font-size: 3em;
            margin: 0 0 10px 0;
            text-align: center;
        }
        h2 {
            font-size: 1.5em;
            margin: 0 0 30px 0;
            text-align: center;
            opacity: 0.9;
        }
        p {
            font-size: 1.1em;
            line-height: 1.6;
            text-align: center;
            opacity: 0.9;
        }
        .cta {
            margin-top: 30px;
            text-align: center;
        }
        button {
            background: white;
            color: #667eea;
            border: none;
            padding: 15px 40px;
            font-size: 1.1em;
            font-weight: 600;
            border-radius: 50px;
            cursor: pointer;
            box-shadow: 0 4px 15px rgba(0,0,0,0.2);
        }
        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 20px rgba(0,0,0,0.3);
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Laboratory</h1>
        <h2>Workforce Development & Innovation</h2>
        <p>Building the future through education, technology, and creative solutions.</p>
        <p>This is a demonstration of our M5Cardputer business card system.</p>
        <div class="cta">
            <button onclick="alert('Contact functionality coming soon!')">Get In Touch</button>
        </div>
    </div>
</body>
</html>
)rawliteral";

void startCaptivePortal(const String& ssid) {
    stopCaptivePortal(); // Stop any existing portal

    portalSSID = ssid;
    portalVisitorCount = 0;
    portalStartTime = millis();

    // Configure Access Point with custom IP configuration
    WiFi.mode(WIFI_AP);

    // Set custom AP IP configuration for better captive portal detection
    IPAddress local_IP(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(local_IP, gateway, subnet);

    WiFi.softAP(ssid.c_str());

    delay(100);

    IPAddress IP = WiFi.softAPIP();

    // Start DNS server (captures all DNS requests)
    portalDNS = new DNSServer();
    portalDNS->setTTL(3600);
    portalDNS->start(53, "*", IP); // Redirect everything to our IP

    // Start web server
    portalWebServer = new WebServer(80);

    // Handle all requests with the same HTML page
    portalWebServer->onNotFound([]() {
        portalVisitorCount++;
        portalWebServer->send(200, "text/html", PORTAL_HTML);
    });

    // Root handler
    portalWebServer->on("/", []() {
        portalVisitorCount++;
        portalWebServer->send(200, "text/html", PORTAL_HTML);
    });

    // Captive portal detection endpoints - redirect to trigger portal popup
    // Android
    portalWebServer->on("/generate_204", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    portalWebServer->on("/gen_204", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    // iOS/Apple
    portalWebServer->on("/hotspot-detect.html", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    portalWebServer->on("/library/test/success.html", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    // Windows - special case: redirect to logout.net
    portalWebServer->on("/connecttest.txt", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://logout.net", true);
        portalWebServer->send(302, "text/plain", "");
    });

    portalWebServer->on("/ncsi.txt", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    // Ubuntu/Linux
    portalWebServer->on("/canonical.html", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    portalWebServer->on("/connectivity-check.html", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    // Firefox
    portalWebServer->on("/success.txt", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    portalWebServer->begin();

    portalState = PORTAL_RUNNING;
    drawPortalScreen();
}

// Global variable to store custom HTML
String customPortalHTML = "";

void startCaptivePortalFromProfile(const PortalProfile& profile) {
    stopCaptivePortal(); // Stop any existing portal

    portalSSID = profile.ssid;
    portalVisitorCount = 0;
    portalStartTime = millis();

    // Load custom HTML from profile
    customPortalHTML = loadPortalHTML(profile);

    // Configure Access Point with custom IP configuration
    WiFi.mode(WIFI_AP);

    // Set custom AP IP configuration for better captive portal detection
    IPAddress local_IP(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(local_IP, gateway, subnet);

    WiFi.softAP(profile.ssid.c_str());

    delay(100);

    IPAddress IP = WiFi.softAPIP();

    // Start DNS server (captures all DNS requests)
    portalDNS = new DNSServer();
    portalDNS->setTTL(3600);
    portalDNS->start(53, "*", IP); // Redirect everything to our IP

    // Start web server
    portalWebServer = new WebServer(80);

    // Handle all requests with custom HTML
    portalWebServer->onNotFound([]() {
        portalVisitorCount++;
        portalWebServer->send(200, "text/html", customPortalHTML);
    });

    // Root handler
    portalWebServer->on("/", []() {
        portalVisitorCount++;
        portalWebServer->send(200, "text/html", customPortalHTML);
    });

    // Captive portal detection endpoints - redirect to trigger portal popup
    // Android
    portalWebServer->on("/generate_204", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    portalWebServer->on("/gen_204", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    // iOS/Apple
    portalWebServer->on("/hotspot-detect.html", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    portalWebServer->on("/library/test/success.html", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    // Windows - special case: redirect to logout.net
    portalWebServer->on("/connecttest.txt", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://logout.net", true);
        portalWebServer->send(302, "text/plain", "");
    });

    portalWebServer->on("/ncsi.txt", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    // Ubuntu/Linux
    portalWebServer->on("/canonical.html", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    portalWebServer->on("/connectivity-check.html", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    // Firefox
    portalWebServer->on("/success.txt", []() {
        portalVisitorCount++;
        portalWebServer->sendHeader("Location", "http://192.168.4.1", true);
        portalWebServer->send(302, "text/plain", "");
    });

    portalWebServer->begin();

    portalState = PORTAL_RUNNING;
    drawPortalScreen();
}

void stopCaptivePortal() {
    if (portalDNS != nullptr) {
        portalDNS->stop();
        delete portalDNS;
        portalDNS = nullptr;
    }

    if (portalWebServer != nullptr) {
        portalWebServer->stop();
        delete portalWebServer;
        portalWebServer = nullptr;
    }

    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(100);

    portalState = PORTAL_STOPPED;
    portalSSID = "";
}

void handlePortalLoop() {
    if (portalState == PORTAL_RUNNING) {
        if (portalDNS != nullptr) {
            portalDNS->processNextRequest();
        }
        if (portalWebServer != nullptr) {
            portalWebServer->handleClient();
        }
    }
}

bool isPortalRunning() {
    return portalState == PORTAL_RUNNING;
}

void drawPortalScreen() {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    drawStatusBar(true);

    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setTextColor(TFT_CYAN);
    M5Cardputer.Display.drawString("Portal Active", 50, 30);

    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_WHITE);

    // Show SSID
    String ssidDisplay = "SSID: " + portalSSID;
    if (ssidDisplay.length() > 35) {
        ssidDisplay = ssidDisplay.substring(0, 35) + "...";
    }
    M5Cardputer.Display.drawString(ssidDisplay.c_str(), 10, 55);

    // Show IP
    IPAddress IP = WiFi.softAPIP();
    String ipStr = "IP: " + IP.toString();
    M5Cardputer.Display.drawString(ipStr.c_str(), 10, 70);

    // Show connected clients
    int clients = WiFi.softAPgetStationNum();
    M5Cardputer.Display.setTextColor(TFT_GREEN);
    M5Cardputer.Display.drawString(("Connected: " + String(clients)).c_str(), 10, 85);

    // Show visitor count
    M5Cardputer.Display.setTextColor(TFT_YELLOW);
    M5Cardputer.Display.drawString(("Visitors: " + String(portalVisitorCount)).c_str(), 10, 100);

    // Show uptime
    unsigned long uptime = (millis() - portalStartTime) / 1000;
    int minutes = uptime / 60;
    int seconds = uptime % 60;
    char timeStr[20];
    sprintf(timeStr, "Uptime: %02d:%02d", minutes, seconds);
    M5Cardputer.Display.setTextColor(TFT_LIGHTGREY);
    M5Cardputer.Display.drawString(timeStr, 10, 115);

    // Instructions
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString("Press any key to stop", 50, 122);
}
