#include "ascii_art.h"
#include "terminal.h"
#include <M5Cardputer.h>

// Font definitions - 5x5 character matrices
// Using simplified fonts that fit the M5Cardputer display

// Banner font (5 rows tall) - STORED IN FLASH MEMORY (PROGMEM)
const char* const banner_font[26][5] PROGMEM = {
  // A
  {" ### ", "#   #", "#####", "#   #", "#   #"},
  // B
  {"#### ", "#   #", "#### ", "#   #", "#### "},
  // C
  {" ### ", "#   #", "#    ", "#   #", " ### "},
  // D
  {"#### ", "#   #", "#   #", "#   #", "#### "},
  // E
  {"#####", "#    ", "#### ", "#    ", "#####"},
  // F
  {"#####", "#    ", "#### ", "#    ", "#    "},
  // G
  {" ### ", "#    ", "#  ##", "#   #", " ### "},
  // H
  {"#   #", "#   #", "#####", "#   #", "#   #"},
  // I
  {"#####", "  #  ", "  #  ", "  #  ", "#####"},
  // J
  {"#####", "   # ", "   # ", "#  # ", " ##  "},
  // K
  {"#   #", "#  # ", "###  ", "#  # ", "#   #"},
  // L
  {"#    ", "#    ", "#    ", "#    ", "#####"},
  // M
  {"#   #", "## ##", "# # #", "#   #", "#   #"},
  // N
  {"#   #", "##  #", "# # #", "#  ##", "#   #"},
  // O
  {" ### ", "#   #", "#   #", "#   #", " ### "},
  // P
  {"#### ", "#   #", "#### ", "#    ", "#    "},
  // Q
  {" ### ", "#   #", "#   #", "#  ##", " ####"},
  // R
  {"#### ", "#   #", "#### ", "#  # ", "#   #"},
  // S
  {" ### ", "#    ", " ### ", "    #", "#### "},
  // T
  {"#####", "  #  ", "  #  ", "  #  ", "  #  "},
  // U
  {"#   #", "#   #", "#   #", "#   #", " ### "},
  // V
  {"#   #", "#   #", "#   #", " # # ", "  #  "},
  // W
  {"#   #", "#   #", "# # #", "## ##", "#   #"},
  // X
  {"#   #", " # # ", "  #  ", " # # ", "#   #"},
  // Y
  {"#   #", " # # ", "  #  ", "  #  ", "  #  "},
  // Z
  {"#####", "   # ", "  #  ", " #   ", "#####"}
};

// Block font (3 rows tall, chunky) - STORED IN FLASH MEMORY (PROGMEM)
const char* const block_font[26][3] PROGMEM = {
  // A
  {"███ ", "█ █ ", "███ "},
  // B
  {"██  ", "██  ", "██  "},
  // C
  {"███ ", "█   ", "███ "},
  // D
  {"██  ", "█ █ ", "██  "},
  // E
  {"███ ", "██  ", "███ "},
  // F
  {"███ ", "██  ", "█   "},
  // G
  {"███ ", "█ █ ", "███ "},
  // H
  {"█ █ ", "███ ", "█ █ "},
  // I
  {"███ ", " █  ", "███ "},
  // J
  {"███ ", "  █ ", "██  "},
  // K
  {"█ █ ", "██  ", "█ █ "},
  // L
  {"█   ", "█   ", "███ "},
  // M
  {"█ █ ", "███ ", "█ █ "},
  // N
  {"██  ", "█ █ ", "█ █ "},
  // O
  {"███ ", "█ █ ", "███ "},
  // P
  {"██  ", "██  ", "█   "},
  // Q
  {"███ ", "█ █ ", "███ "},
  // R
  {"██  ", "██  ", "█ █ "},
  // S
  {"███ ", " ██ ", "███ "},
  // T
  {"███ ", " █  ", " █  "},
  // U
  {"█ █ ", "█ █ ", "███ "},
  // V
  {"█ █ ", "█ █ ", " █  "},
  // W
  {"█ █ ", "███ ", "█ █ "},
  // X
  {"█ █ ", " █  ", "█ █ "},
  // Y
  {"█ █ ", " █  ", " █  "},
  // Z
  {"███ ", " █  ", "███ "}
};

// Small font (1 row, just styled letters)
const char* const small_font[26] PROGMEM = {
  "ᴀ", "ʙ", "ᴄ", "ᴅ", "ᴇ", "ꜰ", "ɢ", "ʜ", "ɪ", "ᴊ", "ᴋ", "ʟ", "ᴍ",
  "ɴ", "ᴏ", "ᴘ", "ǫ", "ʀ", "ꜱ", "ᴛ", "ᴜ", "ᴠ", "ᴡ", "x", "ʏ", "ᴢ"
};

// Slant font (6 rows tall, slanted style) - STORED IN FLASH MEMORY (PROGMEM)
const char* const slant_font[26][6] PROGMEM = {
  // A
  {"   ___   ", "  / _ |  ", " / __ |  ", "/_/ |_|  ", "         ", "         "},
  // B
  {"   ___   ", "  / _ )  ", " / _  |  ", "/____/   ", "         ", "         "},
  // C
  {"  ______", " / ___/  ", "/ /__    ", "\\___/    ", "         ", "         "},
  // D
  {"   ___   ", "  / _ \\  ", " / // /  ", "/____/   ", "         ", "         "},
  // E
  {"   ____  ", "  / __/  ", " / _/    ", "/___/    ", "         ", "         "},
  // F
  {"   ____  ", "  / __/  ", " / _/    ", "/_/      ", "         ", "         "},
  // G
  {"  _____  ", " / ___/  ", "/ (_ /   ", "\\___/    ", "         ", "         "},
  // H
  {"   __  __", "  / / / /", " / /_/ / ", "/___/_/  ", "         ", "         "},
  // I
  {"   ____  ", "  /  _/  ", " _/ /    ", "/___/    ", "         ", "         "},
  // J
  {"     __ ", "    / / ", " __/ /  ", "/___/   ", "        ", "        "},
  // K
  {"   __ __", "  / //_/", " / ,<   ", "/_/|_|  ", "        ", "        "},
  // L
  {"   ____  ", "  / /    ", " / /__  ", "/____/  ", "         ", "         "},
  // M
  {"   __  ___", "  /  |/  /", " / /|_/ / ", "/_/  /_/  ", "          ", "          "},
  // N
  {"   _  __", "  / |/ /", " /    / ", "/_/|_/  ", "        ", "        "},
  // O
  {"   ____  ", "  / __ \\ ", " / /_/ / ", " \\____/  ", "         ", "         "},
  // P
  {"   ___   ", "  / _ \\  ", " / ___/  ", "/_/      ", "         ", "         "},
  // Q
  {"   ____ ", "  / __ \\", " / /_/ /", " \\___\\_\\", "        ", "        "},
  // R
  {"   ___   ", "  / _ \\  ", " / , _/  ", "/_/|_|   ", "         ", "         "},
  // S
  {"   _____", "  / __/ ", " _\\ \\   ", "/___/   ", "        ", "        "},
  // T
  {"  ______", " /_  __/", "  / /   ", " /_/    ", "        ", "        "},
  // U
  {"   __  __", "  / / / /", " / /_/ / ", " \\____/  ", "         ", "         "},
  // V
  {"  _   __", " | | / /", " | |/ / ", " |___/  ", "        ", "        "},
  // W
  {"  _    __", " | |  / /", " | | / / ", " |_|/_/  ", "         ", "         "},
  // X
  {"   _  __", "  | |/_/", " _>  <  ", "/_/|_|  ", "        ", "        "},
  // Y
  {"  __  __", " / / / /", "/ /_/ / ", "\\__, /  ", "/____/  ", "        "},
  // Z
  {"   ____", "  /_  /", " / __/ ", "/____/ ", "       ", "       "}
};

// Gradient color interpolation
uint16_t interpolateColor(uint16_t color1, uint16_t color2, float t) {
  // Extract RGB components from RGB565
  uint8_t r1 = (color1 >> 11) & 0x1F;
  uint8_t g1 = (color1 >> 5) & 0x3F;
  uint8_t b1 = color1 & 0x1F;

  uint8_t r2 = (color2 >> 11) & 0x1F;
  uint8_t g2 = (color2 >> 5) & 0x3F;
  uint8_t b2 = color2 & 0x1F;

  // Interpolate
  uint8_t r = r1 + (r2 - r1) * t;
  uint8_t g = g1 + (g2 - g1) * t;
  uint8_t b = b1 + (b2 - b1) * t;

  // Recombine to RGB565
  return (r << 11) | (g << 5) | b;
}

// Get color for position in gradient
uint16_t getGradientColor(const String& gradientName, int position, int totalWidth) {
  float t = (float)position / (float)totalWidth;

  if (gradientName == "rainbow") {
    // Cycle through rainbow colors
    if (t < 0.17) return interpolateColor(0xF800, 0xFC00, t / 0.17);  // Red to Orange
    else if (t < 0.33) return interpolateColor(0xFC00, 0xFFE0, (t - 0.17) / 0.16);  // Orange to Yellow
    else if (t < 0.5) return interpolateColor(0xFFE0, 0x07E0, (t - 0.33) / 0.17);  // Yellow to Green
    else if (t < 0.67) return interpolateColor(0x07E0, 0x001F, (t - 0.5) / 0.17);  // Green to Blue
    else if (t < 0.83) return interpolateColor(0x001F, 0x780F, (t - 0.67) / 0.16);  // Blue to Indigo
    else return interpolateColor(0x780F, 0xF81F, (t - 0.83) / 0.17);  // Indigo to Violet
  }
  else if (gradientName == "fire") {
    // Red to orange to yellow
    if (t < 0.5) return interpolateColor(0x7800, 0xF800, t / 0.5);  // Dark red to Red
    else return interpolateColor(0xF800, 0xFFE0, (t - 0.5) / 0.5);  // Red to Yellow
  }
  else if (gradientName == "ocean") {
    // Dark blue to cyan to light blue
    if (t < 0.5) return interpolateColor(0x0010, 0x07FF, t / 0.5);  // Dark blue to Cyan
    else return interpolateColor(0x07FF, 0xAD55, (t - 0.5) / 0.5);  // Cyan to Light blue
  }
  else if (gradientName == "sunset") {
    // Purple to pink to orange
    if (t < 0.5) return interpolateColor(0x780F, 0xF81F, t / 0.5);  // Purple to Magenta
    else return interpolateColor(0xF81F, 0xFC00, (t - 0.5) / 0.5);  // Magenta to Orange
  }
  else if (gradientName == "matrix") {
    // Dark green to bright green
    return interpolateColor(0x0200, 0x07E0, t);  // Dark green to Green
  }
  else {
    // Default: white
    return 0xFFFF;
  }
}

// Render text in selected font with optional gradient
void renderAsciiArt(const String& text, const String& font, const String& gradient) {
  String upperText = text;
  upperText.toUpperCase();

  if (font == "slant") {
    // 6-row slant font
    String lines[6] = {"", "", "", "", "", ""};

    for (int i = 0; i < upperText.length(); i++) {
      char c = upperText.charAt(i);
      if (c >= 'A' && c <= 'Z') {
        int idx = c - 'A';
        for (int row = 0; row < 6; row++) {
          lines[row] += slant_font[idx][row];
        }
      } else if (c == ' ') {
        for (int row = 0; row < 6; row++) {
          lines[row] += "  ";
        }
      }
    }

    // Output each line (skip last 2 empty rows)
    for (int row = 0; row < 4; row++) {
      if (gradient != "none") {
        // Apply gradient across the line
        for (int i = 0; i < lines[row].length(); i++) {
          uint16_t color = getGradientColor(gradient, i, lines[row].length());
          terminalPrintColored(String(lines[row].charAt(i)), color);
        }
      } else {
        terminalPrintColored(lines[row], TFT_YELLOW);
      }
      terminalPrintln("");
    }
  }
  else if (font == "small") {
    // Single line output
    String output = "";
    for (int i = 0; i < upperText.length(); i++) {
      char c = upperText.charAt(i);
      if (c >= 'A' && c <= 'Z') {
        output += small_font[c - 'A'];
        output += " ";
      } else if (c == ' ') {
        output += "  ";
      }
    }

    if (gradient != "none") {
      // Output with gradient (character by character)
      for (int i = 0; i < output.length(); i++) {
        uint16_t color = getGradientColor(gradient, i, output.length());
        terminalPrintColored(String(output.charAt(i)), color);
      }
    } else {
      terminalPrintColored(output, TFT_YELLOW);
    }
    terminalPrintln("");
  }
  else if (font == "block") {
    // 3-row block font
    String lines[3] = {"", "", ""};

    for (int i = 0; i < upperText.length(); i++) {
      char c = upperText.charAt(i);
      if (c >= 'A' && c <= 'Z') {
        int idx = c - 'A';
        for (int row = 0; row < 3; row++) {
          lines[row] += block_font[idx][row];
        }
      } else if (c == ' ') {
        for (int row = 0; row < 3; row++) {
          lines[row] += "  ";
        }
      }
    }

    // Output each line
    for (int row = 0; row < 3; row++) {
      if (gradient != "none") {
        // Apply gradient across the line
        for (int i = 0; i < lines[row].length(); i++) {
          uint16_t color = getGradientColor(gradient, i, lines[row].length());
          terminalPrintColored(String(lines[row].charAt(i)), color);
        }
      } else {
        terminalPrintColored(lines[row], TFT_YELLOW);
      }
      terminalPrintln("");
    }
  }
  else {
    // Default: banner font (5 rows)
    String lines[5] = {"", "", "", "", ""};

    for (int i = 0; i < upperText.length(); i++) {
      char c = upperText.charAt(i);
      if (c >= 'A' && c <= 'Z') {
        int idx = c - 'A';
        for (int row = 0; row < 5; row++) {
          lines[row] += banner_font[idx][row];
        }
      } else if (c == ' ') {
        for (int row = 0; row < 5; row++) {
          lines[row] += "  ";
        }
      }
    }

    // Output each line
    for (int row = 0; row < 5; row++) {
      if (gradient != "none") {
        // Apply gradient across the line
        for (int i = 0; i < lines[row].length(); i++) {
          uint16_t color = getGradientColor(gradient, i, lines[row].length());
          terminalPrintColored(String(lines[row].charAt(i)), color);
        }
      } else {
        terminalPrintColored(lines[row], TFT_YELLOW);
      }
      terminalPrintln("");
    }
  }
}

// Parse and execute art command
void cmd_art(const String& args) {
  String font = "banner";  // Default font
  String gradient = "none";  // Default no gradient
  String text = "";

  // Simple argument parsing
  String argsCopy = args;
  argsCopy.trim();

  // Check for help flag
  if (argsCopy == "-h" || argsCopy == "--help" || argsCopy == "help") {
    terminalPrintln("Usage: art [options] <text>");
    terminalPrintln("Options:");
    terminalPrintln("  -h, --help             Show this help");
    terminalPrintln("  --demo, --fonts        Show font demos");
    terminalPrintln("  -f, --font <name>      Font: banner, block, slant, small");
    terminalPrintln("  -g, --gradient <name>  Gradient: rainbow, fire, ocean, sunset, matrix");
    terminalPrintln("Examples:");
    terminalPrintln("  art HELLO");
    terminalPrintln("  art -f block TEST");
    terminalPrintln("  art -g rainbow COOL");
    terminalPrintln("  art -f block -g fire HOT");
    return;
  }

  // Check for demo flag
  if (argsCopy == "--demo" || argsCopy == "--fonts" || argsCopy == "demo") {
    terminalPrintln("=== BANNER FONT ===");
    renderAsciiArt("ABC", "banner", "none");
    terminalPrintln("");
    terminalPrintln("=== BLOCK FONT ===");
    renderAsciiArt("ABC", "block", "none");
    terminalPrintln("");
    terminalPrintln("=== SMALL FONT ===");
    renderAsciiArt("ABC", "small", "none");
    terminalPrintln("");
    terminalPrintln("=== SLANT FONT ===");
    renderAsciiArt("ABC", "slant", "none");
    return;
  }

  // Check for flags
  int textStart = 0;

  if (argsCopy.startsWith("-f ") || argsCopy.startsWith("--font ")) {
    int spacePos = argsCopy.indexOf(' ', argsCopy.startsWith("-f") ? 3 : 7);
    if (spacePos > 0) {
      font = argsCopy.substring(argsCopy.startsWith("-f") ? 3 : 7, spacePos);
      font.trim();
      textStart = spacePos + 1;
    }
  }

  if (argsCopy.indexOf("--gradient ") >= 0 || argsCopy.indexOf("-g ") >= 0) {
    int gPos = argsCopy.indexOf("--gradient ");
    bool shortForm = false;
    if (gPos < 0) {
      gPos = argsCopy.indexOf("-g ");
      shortForm = true;
    }

    if (gPos >= 0) {
      int gStart = gPos + (shortForm ? 3 : 11);
      int spacePos = argsCopy.indexOf(' ', gStart);
      if (spacePos > 0) {
        gradient = argsCopy.substring(gStart, spacePos);
        gradient.trim();
      } else {
        gradient = argsCopy.substring(gStart);
        gradient.trim();
      }
    }
  }

  // Extract text (everything after flags, or entire args if no flags)
  if (argsCopy.indexOf("-f") >= 0 || argsCopy.indexOf("--font") >= 0 ||
      argsCopy.indexOf("-g") >= 0 || argsCopy.indexOf("--gradient") >= 0) {
    // Find the actual text after all flags
    int lastFlagPos = max(argsCopy.lastIndexOf("-f "), argsCopy.lastIndexOf("--font "));
    lastFlagPos = max(lastFlagPos, argsCopy.lastIndexOf("-g "));
    lastFlagPos = max(lastFlagPos, argsCopy.lastIndexOf("--gradient "));

    if (lastFlagPos >= 0) {
      int nextSpace = argsCopy.indexOf(' ', lastFlagPos + 3);
      if (nextSpace >= 0) {
        text = argsCopy.substring(nextSpace + 1);
      }
    }
  } else {
    text = argsCopy;
  }

  text.trim();

  // Validate and show help if needed
  if (text.length() == 0) {
    terminalPrintln("Usage: art [options] <text>");
    terminalPrintln("Options:");
    terminalPrintln("  -f, --font <name>      Font: banner, block, slant, small");
    terminalPrintln("  -g, --gradient <name>  Gradient: rainbow, fire, ocean, sunset, matrix");
    terminalPrintln("Examples:");
    terminalPrintln("  art HELLO");
    terminalPrintln("  art -f block TEST");
    terminalPrintln("  art -g rainbow COOL");
    terminalPrintln("  art -f block -g fire HOT");
    return;
  }

  // Render the art!
  renderAsciiArt(text, font, gradient);
}
