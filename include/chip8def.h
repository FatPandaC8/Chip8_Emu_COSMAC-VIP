#ifndef CHIP8_H
#define CHIP8_H
#include "alllib.h" //It defines exact-width types and some helper types.

class Chip8 {
    public:
    //must include every components of chip8: register, PC, Stack, SP, Delay Timer, Sound Timer, Input, Display
    //Initialize all elements with their default value with array[len] {}
    
    //----COMPONENTS----//
    uint8_t registers[16] {};
    uint8_t memory[4096] {};
    uint16_t index{};
    uint16_t pc {};
    uint16_t stack[16] {}; //16 level
    uint8_t sp {};
    uint8_t delayTimer {};
    uint8_t soundTimer {};
    uint8_t keypad[16] {};
    uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT] {};
    uint16_t opcode {}; //the place to fetch 2 bytes for pc to point 

    //----CONSTRUCTOR----//
    Chip8();

    //----FUNCTIONS----//
    void LoadROM(const char* filename);
    void Cycle();

    private:
    //----INSTRUCTIONS----//
    //BASICS
    void OP_00E0(); //clear the display
    void OP_00EE(); //return from a subroutine
    void OP_1nnn(); //jump to location nnn, set PC to nnn to execute at the location | in format 1nnn as 1 is instruction for calling JP and nnn is for memory location
    void OP_2nnn(); //call subroutine at nnn
    
    //CONDITIONS
    void OP_3xkk(); //skip next instruction if Vx = kk
    void OP_4xkk(); //skip next instruction if Vx != kk
    void OP_5xy0(); //skip next instruction if Vx = Vy
    
    //OPERATIONS
    void OP_6xkk(); //set Vx = kk
    void OP_7xkk(); //add Vx + byte
    void OP_8xy0(); //set Vx = Vy
    void OP_8xy1(); //set Vx = Vx OR Vy
    void OP_8xy2(); //set Vx = Vx AND Vy
    void OP_8xy3(); //set Vx = Vx XOR Vy
    void OP_8xy4(); //set Vx = Vx + Vy, set VF = carry, if > 8 bits -> VF = 1 else 0; only lowest 8 bits are stored in Vx
    void OP_8xy5(); //set Vx = Vx - Vy, set VF = NOT borrow; if Vx > Vy, then VF is set to 1 else 0, stored in Vx
    void OP_8xy6(); //set Vx = Vx SHR 1 (bitwise shift right)
    void OP_8xy7(); //set Vx = Vy - Vx, VF = NOT borrow
    void OP_8xyE(); //set Vx = Vx SHL 1 (left shift)
    void OP_9xy0(); //skip next instruction if Vx != Vy
    void OP_Annn(); //set I = nnn;
    void OP_Bnnn(); //Jump to location nnn + V0
    void OP_Cxkk(); //Set Vx = random byte AND kk
    void OP_Dxyn(); //display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
    
    //INPUT
    void OP_Ex9E(); //skip next instruction if key with value Vx is pressed
    void OP_ExA1(); //skip next instruction if key with value Vx is not pressed
    void OP_Fx07(); //set Vx = delay timer value
    void OP_Fx0A(); //wait for a key to be pressed, store the value of key in Vx
    void OP_Fx15(); //set delay timer = Vx
    void OP_Fx18(); //set sound timer = Vx
    void OP_Fx1E(); //set I = I + Vx
    void OP_Fx29(); //set I = location of sprite for digit Vx
    void OP_Fx33(); //store BCD (Binary Coded Decimal) -> change deciaml to bytes in memory
    void OP_Fx55(); //store registers V0 - Vx in memory starting at location I
    void OP_Fx65(); //read registers V0 - Vx from memory starting at location I

    std::default_random_engine randGen; //produces random bits
    std::uniform_int_distribution<uint8_t> randByte; //shape output from randGen to 0-255

    typedef void (Chip8::*Chip8Func)(); //function pointer
    //----HELPER----//
    /*
    After we get the first nibble it -> it matches a table, but after that, we need to identify the exact position in that table, not just the whole table
    */
    void Table0();
    void Table8();
    void TableE();
    void TableF();
    void OP_NULL();

    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];
};


#endif 