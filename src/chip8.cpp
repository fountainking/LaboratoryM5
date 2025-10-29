#include "chip8.h"
#include <M5Cardputer.h>
#include <SD.h>
#include <AnimatedGIF.h>
#include "ui.h"
#include "config.h"

// CHIP-8 fontset (built into memory at 0x000-0x04F)
static const uint8_t chip8_fontset[80] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// SCHIP large fontset (10-byte sprites at 0x050-0x09F)
static const uint8_t schip_fontset[160] = {
  // 0
  0x3C, 0x7E, 0xE7, 0xC3, 0xC3, 0xC3, 0xC3, 0xE7, 0x7E, 0x3C,
  // 1
  0x18, 0x38, 0x58, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C,
  // 2
  0x3E, 0x7F, 0xC3, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xFF, 0xFF,
  // 3
  0x3C, 0x7E, 0xC3, 0x03, 0x0E, 0x0E, 0x03, 0xC3, 0x7E, 0x3C,
  // 4
  0x06, 0x0E, 0x1E, 0x36, 0x66, 0xC6, 0xFF, 0xFF, 0x06, 0x06,
  // 5
  0xFF, 0xFF, 0xC0, 0xC0, 0xFC, 0xFE, 0x03, 0xC3, 0x7E, 0x3C,
  // 6
  0x3E, 0x7C, 0xC0, 0xC0, 0xFC, 0xFE, 0xC3, 0xC3, 0x7E, 0x3C,
  // 7
  0xFF, 0xFF, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x60, 0x60,
  // 8
  0x3C, 0x7E, 0xC3, 0xC3, 0x7E, 0x7E, 0xC3, 0xC3, 0x7E, 0x3C,
  // 9
  0x3C, 0x7E, 0xC3, 0xC3, 0x7F, 0x3F, 0x03, 0x03, 0x3E, 0x7C,
  // A
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // B
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // C
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // D
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // E
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // F
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Global CHIP-8 instance
Chip8 chip8;
bool chip8Running = false;
static unsigned long lastCycleTime = 0;
static unsigned long lastTimerUpdate = 0;

// ROM browser state
static String romFiles[50];
static int numRoms = 0;
static int selectedRomIndex = 0;

// Gradient settings (user customizable later)
static uint16_t gradientColor1 = TFT_PURPLE;    // Top color
static uint16_t gradientColor2 = TFT_CYAN;      // Bottom color
static bool useGradient = true;

// Helper: Calculate gradient color for Y position
uint16_t getGradientColorForY(int y, int maxY) {
  if (!useGradient) return gradientColor1;

  float t = (float)y / (float)maxY;  // 0.0 to 1.0

  // Extract RGB from color1
  uint8_t r1 = (gradientColor1 >> 11) & 0x1F;
  uint8_t g1 = (gradientColor1 >> 5) & 0x3F;
  uint8_t b1 = gradientColor1 & 0x1F;

  // Extract RGB from color2
  uint8_t r2 = (gradientColor2 >> 11) & 0x1F;
  uint8_t g2 = (gradientColor2 >> 5) & 0x3F;
  uint8_t b2 = gradientColor2 & 0x1F;

  // Interpolate
  uint8_t r = r1 + (r2 - r1) * t;
  uint8_t g = g1 + (g2 - g1) * t;
  uint8_t b = b1 + (b2 - b1) * t;

  return (r << 11) | (g << 5) | b;
}

// CHIP-8 Constructor
Chip8::Chip8() {
  reset();
}

void Chip8::reset() {
  // Clear memory
  memset(memory, 0, sizeof(memory));

  // Load fontsets into memory
  memcpy(memory, chip8_fontset, 80);         // 0x000-0x04F: Standard font
  memcpy(&memory[0x50], schip_fontset, 160); // 0x050-0x09F: SCHIP large font

  // Clear registers, stack, keys
  memset(V, 0, sizeof(V));
  memset(stack, 0, sizeof(stack));
  memset(keys, 0, sizeof(keys));
  memset(rpl, 0, sizeof(rpl));

  I = 0;
  pc = 0x200;  // Programs start at 0x200
  sp = 0;

  delayTimer = 0;
  soundTimer = 0;

  // Start in standard CHIP-8 mode
  extendedMode = false;
  displayWidth = CHIP8_DISPLAY_WIDTH;
  displayHeight = CHIP8_DISPLAY_HEIGHT;

  // Clear display
  memset(display, 0, sizeof(display));
  drawFlag = true;

  running = false;
}

bool Chip8::loadROM(const char* filename) {
  File file = SD.open(filename, FILE_READ);
  if (!file) {
    return false;
  }

  // Read ROM into memory starting at 0x200
  size_t size = file.size();
  if (size > (CHIP8_RAM_SIZE - 0x200)) {
    file.close();
    return false;
  }

  file.read(&memory[0x200], size);
  file.close();

  running = true;
  drawFlag = true;

  return true;
}

void Chip8::setKey(uint8_t key, bool pressed) {
  if (key < CHIP8_KEYS) {
    keys[key] = pressed;
  }
}

void Chip8::cycle() {
  if (!running) return;

  // Fetch opcode (big endian)
  uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];

  // Execute
  executeOpcode(opcode);
}

void Chip8::executeOpcode(uint16_t opcode) {
  uint8_t firstNibble = (opcode & 0xF000) >> 12;

  switch (firstNibble) {
    case 0x0:
      opcode_0(opcode);
      break;
    case 0x1:  // 1NNN - Jump to NNN
      pc = opcode & 0x0FFF;
      return;
    case 0x2:  // 2NNN - Call subroutine at NNN
      stack[sp++] = pc;
      pc = opcode & 0x0FFF;
      return;
    case 0x3:  // 3XNN - Skip next if VX == NN
      if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
        pc += 2;
      }
      break;
    case 0x4:  // 4XNN - Skip next if VX != NN
      if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
        pc += 2;
      }
      break;
    case 0x5:  // 5XY0 - Skip next if VX == VY
      if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
        pc += 2;
      }
      break;
    case 0x6:  // 6XNN - Set VX = NN
      V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
      break;
    case 0x7:  // 7XNN - Add NN to VX
      V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
      break;
    case 0x8:
      opcode_8(opcode);
      break;
    case 0x9:  // 9XY0 - Skip next if VX != VY
      if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
        pc += 2;
      }
      break;
    case 0xA:  // ANNN - Set I = NNN
      I = opcode & 0x0FFF;
      break;
    case 0xB:  // BNNN - Jump to NNN + V0
      pc = (opcode & 0x0FFF) + V[0];
      return;
    case 0xC:  // CXNN - Set VX = random & NN
      V[(opcode & 0x0F00) >> 8] = (random(256)) & (opcode & 0x00FF);
      break;
    case 0xD:  // DXYN - Draw sprite
      {
        uint8_t x = V[(opcode & 0x0F00) >> 8];
        uint8_t y = V[(opcode & 0x00F0) >> 4];
        uint8_t height = opcode & 0x000F;

        V[0xF] = 0;  // Reset collision flag

        // DXY0 in SCHIP = 16x16 sprite
        if (extendedMode && height == 0) {
          height = 16;
          for (int yline = 0; yline < 16; yline++) {
            uint16_t pixel = (memory[I + (yline * 2)] << 8) | memory[I + (yline * 2) + 1];
            for (int xline = 0; xline < 16; xline++) {
              if ((pixel & (0x8000 >> xline)) != 0) {
                int px = (x + xline) % displayWidth;
                int py = (y + yline) % displayHeight;

                if (display[px][py]) {
                  V[0xF] = 1;  // Collision detected
                }
                display[px][py] ^= true;
              }
            }
          }
        } else {
          // Standard 8xN sprite
          for (int yline = 0; yline < height; yline++) {
            uint8_t pixel = memory[I + yline];
            for (int xline = 0; xline < 8; xline++) {
              if ((pixel & (0x80 >> xline)) != 0) {
                int px = (x + xline) % displayWidth;
                int py = (y + yline) % displayHeight;

                if (display[px][py]) {
                  V[0xF] = 1;  // Collision detected
                }
                display[px][py] ^= true;
              }
            }
          }
        }
        drawFlag = true;
      }
      break;
    case 0xE:
      opcode_E(opcode);
      break;
    case 0xF:
      opcode_F(opcode);
      break;
  }

  pc += 2;
}

void Chip8::opcode_0(uint16_t opcode) {
  if (opcode == 0x00E0) {
    // 00E0 - Clear screen
    memset(display, 0, sizeof(display));
    drawFlag = true;
  } else if (opcode == 0x00EE) {
    // 00EE - Return from subroutine
    pc = stack[--sp];
  } else if ((opcode & 0x00F0) == 0x00C0) {
    // 00CN - Scroll down N pixels (SCHIP)
    uint8_t n = opcode & 0x000F;
    for (int y = displayHeight - 1; y >= n; y--) {
      for (int x = 0; x < displayWidth; x++) {
        display[x][y] = display[x][y - n];
      }
    }
    for (int y = 0; y < n; y++) {
      for (int x = 0; x < displayWidth; x++) {
        display[x][y] = false;
      }
    }
    drawFlag = true;
  } else if (opcode == 0x00FB) {
    // 00FB - Scroll right 4 pixels (SCHIP)
    for (int y = 0; y < displayHeight; y++) {
      for (int x = displayWidth - 1; x >= 4; x--) {
        display[x][y] = display[x - 4][y];
      }
      for (int x = 0; x < 4; x++) {
        display[x][y] = false;
      }
    }
    drawFlag = true;
  } else if (opcode == 0x00FC) {
    // 00FC - Scroll left 4 pixels (SCHIP)
    for (int y = 0; y < displayHeight; y++) {
      for (int x = 0; x < displayWidth - 4; x++) {
        display[x][y] = display[x + 4][y];
      }
      for (int x = displayWidth - 4; x < displayWidth; x++) {
        display[x][y] = false;
      }
    }
    drawFlag = true;
  } else if (opcode == 0x00FD) {
    // 00FD - Exit interpreter (SCHIP) - just stop running
    running = false;
  } else if (opcode == 0x00FE) {
    // 00FE - Disable extended mode (back to 64x32)
    extendedMode = false;
    displayWidth = CHIP8_DISPLAY_WIDTH;
    displayHeight = CHIP8_DISPLAY_HEIGHT;
    memset(display, 0, sizeof(display));
    drawFlag = true;
  } else if (opcode == 0x00FF) {
    // 00FF - Enable extended mode (128x64)
    extendedMode = true;
    displayWidth = SCHIP_DISPLAY_WIDTH;
    displayHeight = SCHIP_DISPLAY_HEIGHT;
    memset(display, 0, sizeof(display));
    drawFlag = true;
  }
}

void Chip8::opcode_8(uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  switch (opcode & 0x000F) {
    case 0x0:  // 8XY0 - Set VX = VY
      V[x] = V[y];
      break;
    case 0x1:  // 8XY1 - Set VX = VX OR VY
      V[x] |= V[y];
      break;
    case 0x2:  // 8XY2 - Set VX = VX AND VY
      V[x] &= V[y];
      break;
    case 0x3:  // 8XY3 - Set VX = VX XOR VY
      V[x] ^= V[y];
      break;
    case 0x4:  // 8XY4 - Add VY to VX, set VF = carry
      {
        uint16_t sum = V[x] + V[y];
        V[0xF] = (sum > 255) ? 1 : 0;
        V[x] = sum & 0xFF;
      }
      break;
    case 0x5:  // 8XY5 - Sub VY from VX, set VF = NOT borrow
      V[0xF] = (V[x] > V[y]) ? 1 : 0;
      V[x] -= V[y];
      break;
    case 0x6:  // 8XY6 - Shift VX right by 1, VF = LSB
      V[0xF] = V[x] & 0x1;
      V[x] >>= 1;
      break;
    case 0x7:  // 8XY7 - Set VX = VY - VX, VF = NOT borrow
      V[0xF] = (V[y] > V[x]) ? 1 : 0;
      V[x] = V[y] - V[x];
      break;
    case 0xE:  // 8XYE - Shift VX left by 1, VF = MSB
      V[0xF] = (V[x] & 0x80) >> 7;
      V[x] <<= 1;
      break;
  }
}

void Chip8::opcode_E(uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;

  if ((opcode & 0x00FF) == 0x9E) {
    // EX9E - Skip next if key VX is pressed
    if (keys[V[x]]) {
      pc += 2;
    }
  } else if ((opcode & 0x00FF) == 0xA1) {
    // EXA1 - Skip next if key VX is NOT pressed
    if (!keys[V[x]]) {
      pc += 2;
    }
  }
}

void Chip8::opcode_F(uint16_t opcode) {
  uint8_t x = (opcode & 0x0F00) >> 8;

  switch (opcode & 0x00FF) {
    case 0x07:  // FX07 - Set VX = delay timer
      V[x] = delayTimer;
      break;
    case 0x0A:  // FX0A - Wait for key press, store in VX
      {
        bool keyPressed = false;
        for (int i = 0; i < CHIP8_KEYS; i++) {
          if (keys[i]) {
            V[x] = i;
            keyPressed = true;
            break;
          }
        }
        if (!keyPressed) {
          pc -= 2;  // Repeat this instruction
        }
      }
      break;
    case 0x15:  // FX15 - Set delay timer = VX
      delayTimer = V[x];
      break;
    case 0x18:  // FX18 - Set sound timer = VX
      soundTimer = V[x];
      break;
    case 0x1E:  // FX1E - Set I = I + VX
      I += V[x];
      break;
    case 0x29:  // FX29 - Set I = location of sprite for digit VX
      I = (V[x] & 0x0F) * 5;
      break;
    case 0x30:  // FX30 - Set I = location of large sprite for digit VX (SCHIP)
      I = 0x50 + ((V[x] & 0x0F) * 10);
      break;
    case 0x33:  // FX33 - Store BCD of VX in I, I+1, I+2
      memory[I] = V[x] / 100;
      memory[I + 1] = (V[x] / 10) % 10;
      memory[I + 2] = V[x] % 10;
      break;
    case 0x55:  // FX55 - Store V0 to VX in memory starting at I
      for (int i = 0; i <= x; i++) {
        memory[I + i] = V[i];
      }
      break;
    case 0x65:  // FX65 - Read V0 to VX from memory starting at I
      for (int i = 0; i <= x; i++) {
        V[i] = memory[I + i];
      }
      break;
    case 0x75:  // FX75 - Store V0-VX in RPL user flags (SCHIP)
      for (int i = 0; i <= x && i < SCHIP_RPL_SIZE; i++) {
        rpl[i] = V[i];
      }
      break;
    case 0x85:  // FX85 - Read V0-VX from RPL user flags (SCHIP)
      for (int i = 0; i <= x && i < SCHIP_RPL_SIZE; i++) {
        V[i] = rpl[i];
      }
      break;
  }
}

// ==== UI FUNCTIONS ====

void scanForROMs() {
  numRoms = 0;
  selectedRomIndex = 0;

  File root = SD.open("/roms/chip8");
  if (!root) {
    // Try creating the directory
    SD.mkdir("/roms");
    SD.mkdir("/roms/chip8");
    return;
  }

  File file = root.openNextFile();
  while (file && numRoms < 50) {
    if (!file.isDirectory()) {
      String name = file.name();

      // Skip hidden files and index files
      if (name.startsWith(".") || name == "index.txt" || name.endsWith("index.txt")) {
        file = root.openNextFile();
        continue;
      }

      // Accept any file (with or without extension) - most CHIP-8 ROMs have no extension
      romFiles[numRoms++] = String("/roms/chip8/") + name;
    }
    file = root.openNextFile();
  }
  root.close();
}

static int lastSelectedRom = -1;
static bool browserFullRedraw = true;

void enterChip8() {
  chip8.reset();
  chip8Running = false;
  browserFullRedraw = true;  // Trigger full redraw
  scanForROMs();
  drawChip8ROMBrowser();
}

void drawChip8ROMBrowser() {
  // OPTIMIZED: Only full redraw when entering browser
  if (browserFullRedraw) {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    drawStatusBar(false);

    // Left side: Compact ROM list (60px wide like file manager)
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_YELLOW);
    M5Cardputer.Display.drawString("ROMS", 3, 25);
    M5Cardputer.Display.drawLine(60, 24, 60, 135, TFT_DARKGREY);  // Divider

    // Right side: Large preview/info area (180px wide)
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_CYAN);
    M5Cardputer.Display.drawString("CHIP-8 Controls:", 70, 30);
    M5Cardputer.Display.setTextColor(TFT_WHITE);
    M5Cardputer.Display.drawString("4567  ->  123C", 70, 45);
    M5Cardputer.Display.drawString("rtyu  ->  456D", 70, 55);
    M5Cardputer.Display.drawString("dfgh  ->  789E", 70, 65);
    M5Cardputer.Display.drawString("xcvb  ->  A0BF", 70, 75);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString("TAB=Show Keymap  ESC=Exit", 70, 95);
    M5Cardputer.Display.drawString("ENTER=Play  `=Back", 70, 105);

    browserFullRedraw = false;
    lastSelectedRom = -1;  // Force list redraw
  }

  if (numRoms == 0) {
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.setTextColor(TFT_RED);
    M5Cardputer.Display.drawString("No ROMs!", 60, 50);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_WHITE);
    M5Cardputer.Display.drawString("Place ROM files in:", 50, 75);
    M5Cardputer.Display.drawString("/roms/chip8/", 60, 90);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString("(any file type)", 60, 105);
    return;
  }

  // Redraw visible ROM list (LEFT NARROW COLUMN - 60px)
  // Track scroll position to detect when we need full redraw
  static int lastStartIdx = -1;
  int startIdx = max(0, selectedRomIndex - 4);
  int endIdx = min(numRoms, startIdx + 9);

  // Full redraw needed when scrolling or first draw
  bool needsFullRedraw = (lastSelectedRom == -1) || (lastStartIdx != startIdx);

  if (needsFullRedraw) {
    // Clear entire list area
    M5Cardputer.Display.fillRect(2, 36, 56, 92, TFT_BLACK);
  }

  int yPos = 38;
  for (int i = startIdx; i < endIdx; i++) {
    bool isSelected = (i == selectedRomIndex);

    // Redraw if full redraw needed, or if selection state changed
    if (needsFullRedraw || (i == lastSelectedRom) || isSelected) {
      // Extract & format filename (VERY compact - 8 chars max)
      String filename = romFiles[i];
      int lastSlash = filename.lastIndexOf('/');
      if (lastSlash >= 0) filename = filename.substring(lastSlash + 1);
      int dot = filename.lastIndexOf('.');
      if (dot >= 0) filename = filename.substring(0, dot);
      if (filename.length() > 8) filename = filename.substring(0, 7) + ".";  // Ultra compact

      // Clear and redraw row
      if (!needsFullRedraw) {
        M5Cardputer.Display.fillRect(2, yPos - 2, 56, 9, TFT_BLACK);
      }

      M5Cardputer.Display.setTextSize(1);

      if (isSelected) {
        // Selected - highlighted
        M5Cardputer.Display.fillRoundRect(2, yPos - 2, 56, 9, 1, TFT_YELLOW);
        M5Cardputer.Display.setTextColor(TFT_BLACK);
      } else {
        // Unselected
        M5Cardputer.Display.setTextColor(TFT_WHITE);
      }

      M5Cardputer.Display.drawString(filename, 4, yPos);
    }

    yPos += 10;
  }

  lastSelectedRom = selectedRomIndex;
  lastStartIdx = startIdx;
}

// Track previous display state for fast delta updates
static bool prevDisplay[SCHIP_DISPLAY_WIDTH][SCHIP_DISPLAY_HEIGHT];
static bool firstDraw = true;
static bool showKeymap = false;
static unsigned long keymapShowTime = 0;

void drawChip8Screen() {
  // GAMEPLAY ON RIGHT SIDE - maximize space (180px x 135px available)
  int displayW = chip8.displayWidth;
  int displayH = chip8.displayHeight;

  // Calculate max scale to fit right side (180px wide max, 111px tall after status bar)
  int maxWidth = 180;
  int maxHeight = 111;  // 135 - 24 status bar
  int scale = min(maxWidth / displayW, maxHeight / displayH);
  if (scale < 1) scale = 1;

  int gameWidth = displayW * scale;
  int gameHeight = displayH * scale;
  int offsetX = 60 + (maxWidth - gameWidth) / 2;  // Center in right area
  int offsetY = 24 + (maxHeight - gameHeight) / 2;  // Center vertically

  // First draw: clear screen and draw left ROM list sidebar
  if (firstDraw) {
    M5Cardputer.Display.fillScreen(TFT_BLACK);
    drawStatusBar(false);

    // Draw left sidebar with ROM list during gameplay
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_DARKGREY);
    M5Cardputer.Display.drawString("ROMS", 3, 25);
    M5Cardputer.Display.drawLine(60, 24, 60, 135, TFT_DARKGREY);

    // Draw compact ROM list in left sidebar
    int startIdx = max(0, selectedRomIndex - 4);
    int endIdx = min(numRoms, startIdx + 9);
    int yPos = 38;

    for (int i = startIdx; i < endIdx; i++) {
      bool isCurrentGame = (i == selectedRomIndex);

      // Extract & format filename
      String filename = romFiles[i];
      int lastSlash = filename.lastIndexOf('/');
      if (lastSlash >= 0) filename = filename.substring(lastSlash + 1);
      int dot = filename.lastIndexOf('.');
      if (dot >= 0) filename = filename.substring(0, dot);
      if (filename.length() > 8) filename = filename.substring(0, 7) + ".";

      M5Cardputer.Display.setTextSize(1);

      if (isCurrentGame) {
        // Currently playing - highlighted
        M5Cardputer.Display.fillRoundRect(2, yPos - 2, 56, 9, 1, TFT_DARKGREY);
        M5Cardputer.Display.setTextColor(TFT_YELLOW);
      } else {
        M5Cardputer.Display.setTextColor(TFT_DARKGREY);
      }

      M5Cardputer.Display.drawString(filename, 4, yPos);
      yPos += 10;
    }

    memset(prevDisplay, 0, sizeof(prevDisplay));
    firstDraw = false;
  }

  // DELTA RENDERING - only update changed pixels
  for (int y = 0; y < displayH; y++) {
    for (int x = 0; x < displayW; x++) {
      bool current = chip8.display[x][y];
      bool previous = prevDisplay[x][y];

      if (current != previous) {
        int screenX = offsetX + (x * scale);
        int screenY = offsetY + (y * scale);
        uint16_t color = current ? TFT_WHITE : TFT_BLACK;

        if (scale > 1) {
          M5Cardputer.Display.fillRect(screenX, screenY, scale, scale, color);
        } else {
          M5Cardputer.Display.drawPixel(screenX, screenY, color);
        }

        prevDisplay[x][y] = current;
      }
    }
  }

  chip8.drawFlag = false;

  // Draw keymap overlay if enabled (positioned in right gameplay area)
  if (showKeymap) {
    // Draw compact keymap overlay in bottom-right of gameplay area
    int x = 145;
    int y = 95;

    // Semi-transparent background
    M5Cardputer.Display.fillRoundRect(x-2, y-2, 88, 38, 3, TFT_DARKGREY);
    M5Cardputer.Display.drawRoundRect(x-2, y-2, 88, 38, 3, TFT_YELLOW);

    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(TFT_YELLOW);

    // Keymap (compact format)
    M5Cardputer.Display.drawString("4567->123C", x, y);
    M5Cardputer.Display.drawString("rtyu->456D", x, y+9);
    M5Cardputer.Display.drawString("dfgh->789E", x, y+18);
    M5Cardputer.Display.drawString("xcvb->A0BF", x, y+27);

    // Auto-hide after 3 seconds
    if (millis() - keymapShowTime > 3000) {
      showKeymap = false;
      firstDraw = true;  // Force full redraw to clear keymap
    }
  }
}

void handleChip8Input() {
  // CHIP-8 has 16 keys (0-F), map to M5 keyboard
  // Keymap: 4567/rtyu/dfgh/xcvb

  // DON'T call update() - main loop already does it
  Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

  // Check for ESC to exit game back to ROM browser
  if (status.del) {
    chip8Running = false;
    firstDraw = true;  // Reset for next game
    showKeymap = false;  // Hide keymap
    drawChip8ROMBrowser();
    return;
  }

  // Check for Tab to toggle keymap overlay
  static bool lastTabState = false;
  if (status.tab && !lastTabState) {
    showKeymap = !showKeymap;
    if (showKeymap) {
      keymapShowTime = millis();
    } else {
      firstDraw = true;  // Force redraw to clear keymap
    }
  }
  lastTabState = status.tab;

  // Track key states (DON'T reset every frame - that causes lag!)
  static bool keyStates[16] = {false};
  bool currentKeys[16] = {false};

  // Map pressed keys (4567/rtyu/dfgh/xcvb)
  for (auto key : status.word) {
    switch (key) {
      case '4': currentKeys[0x1] = true; break;
      case '5': currentKeys[0x2] = true; break;
      case '6': currentKeys[0x3] = true; break;
      case '7': currentKeys[0xC] = true; break;
      case 'r': currentKeys[0x4] = true; break;
      case 't': currentKeys[0x5] = true; break;
      case 'y': currentKeys[0x6] = true; break;
      case 'u': currentKeys[0xD] = true; break;
      case 'd': currentKeys[0x7] = true; break;
      case 'f': currentKeys[0x8] = true; break;
      case 'g': currentKeys[0x9] = true; break;
      case 'h': currentKeys[0xE] = true; break;
      case 'x': currentKeys[0xA] = true; break;
      case 'c': currentKeys[0x0] = true; break;
      case 'v': currentKeys[0xB] = true; break;
      case 'b': currentKeys[0xF] = true; break;
    }
  }

  // Update chip8 key states (only when changed for instant response)
  for (int i = 0; i < 16; i++) {
    if (currentKeys[i] != keyStates[i]) {
      chip8.setKey(i, currentKeys[i]);
      keyStates[i] = currentKeys[i];
    }
  }
}

void handleChip8BrowserInput(Keyboard_Class::KeysState status) {
  // Enter key - load selected ROM
  if (status.enter && numRoms > 0) {
    String romPath = romFiles[selectedRomIndex];
    chip8.reset();
    if (chip8.loadROM(romPath.c_str())) {
      chip8Running = true;
      firstDraw = true;  // Trigger fullscreen redraw
      drawChip8Screen();
    }
    return;
  }

  // Handle navigation
  for (auto key : status.word) {
    if (key == ';' || key == ',') {
      // Up
      if (selectedRomIndex > 0) {
        selectedRomIndex--;
        drawChip8ROMBrowser();  // Only redraws changed rows now!
      }
    } else if (key == '.' || key == '/') {
      // Down
      if (selectedRomIndex < numRoms - 1) {
        selectedRomIndex++;
        drawChip8ROMBrowser();  // Only redraws changed rows now!
      }
    } else if (key == '`') {
      // Back to Games menu
      extern int currentScreenNumber;
      extern void drawGamesMenu();
      browserFullRedraw = true;  // Reset for next time
      currentScreenNumber = 8;
      drawGamesMenu();
    }
  }
}
