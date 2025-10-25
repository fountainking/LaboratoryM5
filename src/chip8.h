#ifndef CHIP8_H
#define CHIP8_H

#include <Arduino.h>
#include <M5Cardputer.h>

// CHIP-8 specifications
#define CHIP8_RAM_SIZE 4096
#define CHIP8_REGISTERS 16
#define CHIP8_STACK_SIZE 16
#define CHIP8_KEYS 16
#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32

// SCHIP specifications
#define SCHIP_DISPLAY_WIDTH 128
#define SCHIP_DISPLAY_HEIGHT 64
#define SCHIP_RPL_SIZE 8

class Chip8 {
public:
  Chip8();

  // Core functions
  void reset();
  bool loadROM(const char* filename);
  void cycle();  // Execute one instruction

  // Display
  bool display[SCHIP_DISPLAY_WIDTH][SCHIP_DISPLAY_HEIGHT];  // Max size for SCHIP
  bool drawFlag;  // Set when display needs redraw

  // Input
  void setKey(uint8_t key, bool pressed);
  bool keys[CHIP8_KEYS];

  // Timers
  uint8_t delayTimer;
  uint8_t soundTimer;

  // State
  bool running;
  uint16_t pc;  // Program counter

  // SCHIP extensions
  bool extendedMode;  // True = 128x64, False = 64x32
  uint8_t displayWidth;
  uint8_t displayHeight;
  uint8_t rpl[SCHIP_RPL_SIZE];  // RPL user flags

private:
  uint8_t memory[CHIP8_RAM_SIZE];
  uint8_t V[CHIP8_REGISTERS];  // Registers V0-VF
  uint16_t I;  // Index register
  uint16_t stack[CHIP8_STACK_SIZE];
  uint8_t sp;  // Stack pointer

  // Opcode handlers
  void executeOpcode(uint16_t opcode);
  void opcode_0(uint16_t opcode);
  void opcode_8(uint16_t opcode);
  void opcode_E(uint16_t opcode);
  void opcode_F(uint16_t opcode);
};

// Global instances (defined in chip8.cpp)
extern Chip8 chip8;
extern bool chip8Running;

// UI functions
void enterChip8();
void drawChip8ROMBrowser();
void drawChip8Screen();
void handleChip8Input();
void handleChip8BrowserInput(Keyboard_Class::KeysState status);

#endif

