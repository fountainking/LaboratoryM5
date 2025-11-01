#!/usr/bin/env python3
"""
Create a CHIP-8 ROM that displays scrolling credits
CHIP-8 specs:
- 64x32 monochrome display
- 16 general purpose registers (V0-VF)
- 4KB memory starting at 0x200
- Draw sprite instruction: Dxyn
"""

# CHIP-8 opcodes we'll use:
# 00E0 - Clear screen
# 6xnn - Set Vx = nn
# 7xnn - Add nn to Vx
# Annn - Set I = nnn
# Dxyn - Draw sprite at (Vx, Vy) height n
# 1nnn - Jump to nnn
# Fx29 - Set I = location of sprite for digit Vx

def create_credits_rom():
    """Create CHIP-8 ROM with scrolling credits"""

    rom = bytearray()

    # Program starts at 0x200
    # We'll store sprite data after the code

    # Initialize
    rom.extend([
        0x00, 0xE0,  # 0x200: Clear screen
        0x60, 0x00,  # 0x202: V0 = 0 (X position)
        0x61, 0x00,  # 0x204: V1 = 0 (Y start position)
    ])

    # Main loop: Draw "LAB" at top
    rom.extend([
        # Draw 'L'
        0xA2, 0x50,  # 0x206: I = 0x250 (sprite location for 'L')
        0x60, 0x08,  # 0x208: V0 = 8 (X)
        0x61, 0x04,  # 0x20A: V1 = 4 (Y)
        0xD0, 0x15,  # 0x20C: Draw sprite at (V0, V1) height 5

        # Draw 'A'
        0xA2, 0x55,  # 0x20E: I = 0x255 (sprite for 'A')
        0x60, 0x10,  # 0x210: V0 = 16 (X)
        0xD0, 0x15,  # 0x212: Draw sprite

        # Draw 'B'
        0xA2, 0x5A,  # 0x214: I = 0x25A (sprite for 'B')
        0x60, 0x18,  # 0x216: V0 = 24 (X)
        0xD0, 0x15,  # 0x218: Draw sprite

        # Draw "M5"
        0xA2, 0x5F,  # 0x21A: I = 0x25F (sprite for 'M')
        0x60, 0x22,  # 0x21C: V0 = 34 (X)
        0xD0, 0x15,  # 0x21E: Draw sprite

        0xA2, 0x64,  # 0x220: I = 0x264 (sprite for '5')
        0x60, 0x2A,  # 0x222: V0 = 42 (X)
        0xD0, 0x15,  # 0x224: Draw sprite

        # Draw "BY JAMES" below
        0xA2, 0x5A,  # 0x226: I = sprite for 'B'
        0x60, 0x08,  # 0x228: V0 = 8
        0x61, 0x10,  # 0x22A: V1 = 16 (Y)
        0xD0, 0x15,  # 0x22C: Draw

        0xA2, 0x69,  # 0x22E: I = sprite for 'Y'
        0x60, 0x10,  # 0x230: V0 = 16
        0xD0, 0x15,  # 0x232: Draw

        # Draw "2025" at bottom
        0xA2, 0x6E,  # 0x234: I = sprite for '2'
        0x60, 0x10,  # 0x236: V0 = 16
        0x61, 0x1A,  # 0x238: V1 = 26 (Y)
        0xD0, 0x15,  # 0x23A: Draw

        0xA2, 0x73,  # 0x23C: I = sprite for '0'
        0x60, 0x18,  # 0x23E: V0 = 24
        0xD0, 0x15,  # 0x240: Draw

        0xA2, 0x6E,  # 0x242: I = sprite for '2'
        0x60, 0x20,  # 0x244: V0 = 32
        0xD0, 0x15,  # 0x246: Draw

        0xA2, 0x64,  # 0x248: I = sprite for '5'
        0x60, 0x28,  # 0x24A: V0 = 40
        0xD0, 0x15,  # 0x24C: Draw

        # Infinite loop (halt)
        0x12, 0x4E,  # 0x24E: Jump to 0x24E (infinite loop)
    ])

    # Pad to sprite data location (0x250)
    while len(rom) < 0x50:
        rom.append(0x00)

    # Sprite data (5 bytes each, 8 pixels wide)
    # Each byte is a row of pixels (1 = on, 0 = off)

    # 'L' sprite (0x250)
    rom.extend([
        0xF0,  # ████░░░░
        0x80,  # █░░░░░░░
        0x80,  # █░░░░░░░
        0x80,  # █░░░░░░░
        0xF0,  # ████░░░░
    ])

    # 'A' sprite (0x255)
    rom.extend([
        0xF0,  # ████░░░░
        0x90,  # █░░█░░░░
        0xF0,  # ████░░░░
        0x90,  # █░░█░░░░
        0x90,  # █░░█░░░░
    ])

    # 'B' sprite (0x25A)
    rom.extend([
        0xE0,  # ███░░░░░
        0x90,  # █░░█░░░░
        0xE0,  # ███░░░░░
        0x90,  # █░░█░░░░
        0xE0,  # ███░░░░░
    ])

    # 'M' sprite (0x25F)
    rom.extend([
        0x90,  # █░░█░░░░
        0xF0,  # ████░░░░
        0x90,  # █░░█░░░░
        0x90,  # █░░█░░░░
        0x90,  # █░░█░░░░
    ])

    # '5' sprite (0x264)
    rom.extend([
        0xF0,  # ████░░░░
        0x80,  # █░░░░░░░
        0xF0,  # ████░░░░
        0x10,  # ░░░█░░░░
        0xF0,  # ████░░░░
    ])

    # 'Y' sprite (0x269)
    rom.extend([
        0x90,  # █░░█░░░░
        0x90,  # █░░█░░░░
        0x60,  # ░██░░░░░
        0x20,  # ░░█░░░░░
        0x20,  # ░░█░░░░░
    ])

    # '2' sprite (0x26E)
    rom.extend([
        0xF0,  # ████░░░░
        0x10,  # ░░░█░░░░
        0xF0,  # ████░░░░
        0x80,  # █░░░░░░░
        0xF0,  # ████░░░░
    ])

    # '0' sprite (0x273)
    rom.extend([
        0xF0,  # ████░░░░
        0x90,  # █░░█░░░░
        0x90,  # █░░█░░░░
        0x90,  # █░░█░░░░
        0xF0,  # ████░░░░
    ])

    return rom

def main():
    rom = create_credits_rom()

    # Save ROM
    output_path = "/Users/jamesfauntleroy/Documents/PlatformIO/Projects/LaboratoryM5/data/chip8/credits.ch8"
    with open(output_path, 'wb') as f:
        f.write(rom)

    print(f"✅ CHIP-8 credits ROM created: {output_path}")
    print(f"   Size: {len(rom)} bytes")
    print(f"   Displays: LAB M5")
    print(f"             BY")
    print(f"             2025")

if __name__ == "__main__":
    main()
