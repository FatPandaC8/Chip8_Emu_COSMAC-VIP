# Chip8_Emu_COSMAC-VIP
  - This is a chip8 emulator that works for a screen of 64×32 and games of COSMAC‑VIP. 
  - A lightweight CHIP-8 interpreter written in C++, built from scratch to simulate an early virtual machine architecture for 1970s-era games like Pong, Space Invaders, and Tetris. Includes full opcode support, font loading, ROM execution, and pixel-accurate graphics rendering logic.

    ## Features:  
    - Full Instruction Set – 35 CHIP-8 instructions

    - Sprite Drawing – XOR-based collision detection and graphics

    - Keypad Input – Emulates original 16-key hexadecimal input system

    - Timers – Sound and delay timers decrement at 60Hz

    - Random Number Generation 

    - Fontset Loading – Hardcoded 6x8 font glyphs for 0–F

    - ROM Loading 
    
# Requirements
  - C++17 or newer
  - SDL2 (for rendering and input – not included yet)

# Build
```
chmod +x run.sh
./run.sh
```
This will make a binary named chip8
# Run
```
./chip8 [VideoScale] [DelayCycle] [NameofROMs]
```
# Memory Layout
- 0x000-0x1FF	Reserved
- 0x050-0x09F	Fontset memory
- 0x200-0xFFF	ROM and execution starts here (PC is at 0x200)

# Keypad Mapping
```
Keypad       Keyboard
---------    ---------
|1|2|3|C|    |1|2|3|4|
---------    ---------
|4|5|6|D|    |Q|W|E|R|
--------- => ---------
|7|8|9|E|    |A|S|D|F|
---------    ---------
|A|0|B|F|    |Z|X|C|V|
---------    ---------
```
# Instructions list:

  - Display: 00E0, 00EE, Dxyn

  - Control: 1nnn, 2nnn, Bnnn

  - Conditionals: 3xkk, 4xkk, 5xy0, 9xy0

  - Registers: 6xkk, 7xkk, 8xy0–8xyE

  - Input: Ex9E, ExA1, Fx0A

  - Timers: Fx07, Fx15, Fx18

  - Memory: Annn, Fx1E, Fx29, Fx33, Fx55, Fx65

  - Random: Cxkk

# References

  - Cowgod’s CHIP-8 Technical Reference
  - Austin Morlan Blog

    
