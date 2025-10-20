#ifndef TERMINAL_H
#define TERMINAL_H

#include <Arduino.h>
#include <vector>
#include <SD.h>

// Terminal configuration
#define MAX_OUTPUT_LINES 50
#define MAX_INPUT_LENGTH 120
#define TERMINAL_HISTORY_SIZE 20

// Colored text structure
struct ColoredText {
  String text;
  uint16_t color;
  ColoredText(const String& t, uint16_t c) : text(t), color(c) {}
};

struct OutputLine {
  std::vector<ColoredText> segments;
  OutputLine() {}
};

// Terminal state
extern String currentInput;
extern std::vector<OutputLine> outputBuffer;
extern std::vector<String> commandHistory;
extern int historyIndex;
extern String currentDirectory;
extern int scrollOffset;
extern OutputLine currentLine;

// Terminal functions
void enterTerminal();
void drawTerminal();
void handleTerminalInput(char key);
void executeCommand(const String& cmd);
void addOutput(const String& text);
void clearOutput();
void scrollTerminalUp();
void scrollTerminalDown();

// Command implementations
void cmd_help();
void cmd_clear();
void cmd_ls(const String& path = "");
void cmd_cd(const String& path);
void cmd_pwd();
void cmd_cat(const String& filename);
void cmd_rm(const String& filename);
void cmd_mkdir(const String& dirname);
void cmd_rmdir(const String& dirname);
void cmd_free();
void cmd_uptime();
void cmd_reboot();
void cmd_wifi_scan();
void cmd_wifi_connect(const String& ssid, const String& password);
void cmd_wifi_status();
void cmd_wifi_disconnect();
void cmd_echo(const String& text);
void cmd_stars();
void cmd_art(const String& args);
void cmd_colors();
void cmd_img(const String& filename);
void cmd_ssh(const String& args);
void cmd_run(const String& filename);
void cmd_edit(const String& filename);
void cmd_scan(const String& args);
void cmd_ping(const String& args);
void cmd_arp(const String& args);        // arp-scan -> "arp" (find devices on local network)
void cmd_trace(const String& args);      // traceroute -> "trace" (trace network path)
void cmd_whois(const String& args);      // whois lookup
void cmd_dns(const String& args);        // dig/nslookup -> "dns" (DNS lookup)
void cmd_netstat(const String& args);    // show active connections -> "netstat"
void cmd_mac(const String& args);        // macchanger -> "mac" (change MAC address)
void cmd_speed(const String& args);      // speedtest -> "speed" (network speed test)
void cmd_ip();                           // show IP address

// Helper functions
void terminalPrintln(const String& text);
void terminalPrintColored(const String& text, uint16_t color);
String getAbsolutePath(const String& path);
bool fileExists(const String& path);
bool isDirectory(const String& path);
void listDirectory(File dir, const String& indent = "");

#endif
