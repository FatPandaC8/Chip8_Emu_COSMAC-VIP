#include "chip8def.h"

//----CONST----//
//static : one value shared by the class not per-objec
//constexpr : guarantee it's compiler time constant

const unsigned int START_ADD = 0x200; //because memory from 0x000 to 0x1FF is reserved
const unsigned int START_ADD_FONT = 0x50;
static constexpr std::size_t FONT_SIZE = 80; //16 characters each 5 bytes - This must be compiler constant 

uint8_t fontset[FONT_SIZE] = {
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

Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()){ //use sytem time to make a seed for randomness
    pc = START_ADD;   

    //load font into memory
    for(unsigned int i = 0; i < FONT_SIZE; ++i){
        memory[START_ADD_FONT + i] = fontset[i];
    }

    randByte = std::uniform_int_distribution<uint8_t>(0,255U);

    //make a fallback safety
    for (size_t i = 0; i <= 0xF; i++) table[i] = &Chip8::OP_NULL;
    for (size_t i = 0; i <= 0xE; i++){ 
		table0[i] = &Chip8::OP_NULL;
		table8[i] = &Chip8::OP_NULL;
		tableE[i] = &Chip8::OP_NULL;
	}
    for(size_t i = 0; i <= 0x65; i++) tableF[i] = &Chip8::OP_NULL; 

    //primary opcode prefixes 
    table[0x0] = &Chip8::Table0;
	table[0x1] = &Chip8::OP_1nnn;
	table[0x2] = &Chip8::OP_2nnn;
	table[0x3] = &Chip8::OP_3xkk;
	table[0x4] = &Chip8::OP_4xkk;
	table[0x5] = &Chip8::OP_5xy0;
	table[0x6] = &Chip8::OP_6xkk;
	table[0x7] = &Chip8::OP_7xkk;
	table[0x8] = &Chip8::Table8;
	table[0x9] = &Chip8::OP_9xy0;
	table[0xA] = &Chip8::OP_Annn;
	table[0xB] = &Chip8::OP_Bnnn;
	table[0xC] = &Chip8::OP_Cxkk;
	table[0xD] = &Chip8::OP_Dxyn;
	table[0xE] = &Chip8::TableE;
	table[0xF] = &Chip8::TableF;

    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    tableE[0x1] = &Chip8::OP_ExA1;
	tableE[0xE] = &Chip8::OP_Ex9E;

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;

}

void Chip8::LoadROM(char const* filename){
    std::ifstream file(filename, std::ios::binary | std::ios::ate); //read file in binary mode, and move the pointer to the end of file
    if(file.is_open()){
        //get size of file and allocate buffer to hold the contents
        std::streampos size = file.tellg(); //get size at position i, but as we already at the end of file -> the size == file size
        char* buffer = new char[size];

        //go back to the beginning, and fill the buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        //load the ROM into Chip8 , starting at 0x200
        for(long i = 0; i < size; ++i){
            memory[START_ADD + i] = buffer[i];
        }

        //free buffer
        delete[] buffer;
    }
}   

void Chip8::Cycle(){
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    pc += 2;

    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

    if(delayTimer > 0){
        --delayTimer;
    }

    if(soundTimer > 0){
        --soundTimer;
    }
}

void Chip8::OP_00E0(){ //CLS
    memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE(){ //RET
    --sp;
    pc = stack[sp];
}

void Chip8::OP_1nnn(){ //JP
    uint16_t address = opcode & 0x0FFFu; //mask to take the last 12 bits from opcode
    pc = address; 
}

void Chip8::OP_2nnn(){ //CALL 
    uint16_t address = opcode & 0x0FFFu;
    stack[sp] = pc; //store the current pc
    ++sp;
    pc = address; //jump to address that was called
}

void Chip8::OP_3xkk(){ //SE Vx, byte
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

    if(registers[Vx] == byte){
        pc += 2;
    }
}

void Chip8::OP_4xkk(){ //SNE Vx, byte
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

    if(registers[Vx] != byte){
        pc += 2;
    }
}

void Chip8::OP_5xy0(){ //SE Vx, Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vx] == registers[Vy]){
        pc += 2;
    }
}

void Chip8::OP_6xkk(){ //LD Vx, byte
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = byte;
}

void Chip8::OP_7xkk(){ //ADD Vx, byte
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

    registers[Vx] += byte;
}

void Chip8::OP_8xy0(){ //LD Vx, Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1(){ //OR Vx, Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vx] | registers[Vy];
}

void Chip8::OP_8xy2(){ //AND Vx, Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vx] & registers[Vy];
}

void Chip8::OP_8xy3(){ //XOR Vx, Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vx] ^ registers[Vy];
}

void Chip8::OP_8xy4(){ //ADD Vx, Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    uint16_t sum = registers[Vx] + registers[Vy];

    if(sum > 255U){
        registers[0xF] = 1;
    }
    else{
        registers[0xF] = 0;
    }

    registers[Vx] = sum & 0xFFu;
}

void Chip8::OP_8xy5(){ //SUB Vx, Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vx] > registers[Vy]){
        registers[0xF] = 1;
    }
    else{
        registers[0xF] = 0;
    }

    registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6(){ //SHR Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[0xF] = (registers[Vx] & 0x1u);
    registers[Vx] >>= 1;
}

void Chip8::OP_8xy7(){ //SUBN Vx, Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vy] > registers[Vx]){
        registers[0xF] = 1;
    }
    else{
        registers[0xF] = 0;
    }

    registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE(){ //SHL Vx {,Vy}
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    //Save MSB in VF
    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

    registers[Vx] <<= 1;

}

void Chip8::OP_9xy0(){ //SNE Vx, Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(registers[Vx] != registers[Vy]){
        pc += 2;
    }
}

void Chip8::OP_Annn(){ //LD I, addr
    uint16_t address = opcode & 0x0FFFu;

    index =  address;
}

void Chip8::OP_Bnnn(){ //JP V0, addr
    uint16_t address = opcode & 0x0FFFu;

    pc = registers[0] + address;
}

void Chip8::OP_Cxkk(){ //RND Vx, byte
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn(){ //DRW Vx, Vy, nibble
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

    registers[0xF] = 0; //reset

    for(unsigned int row = 0; row < height; ++row){
        uint8_t spriteByte = memory[index + row];

        for(unsigned int col = 0; col < 8; ++col){ //each is only 8 bit across
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)]; //get the memory of the position of the pixel

            if(spritePixel){ //if the sprite is on
                if(*screenPixel == 0xFFFFFFFF){ //if the screen pixel is also on - collision
                    registers[0xF] = 1;
                }

                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

void Chip8::OP_Ex9E(){ //SKP Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    uint8_t key = registers[Vx];

    if(keypad[key]){
        pc += 2;
    }
}

void Chip8::OP_ExA1(){ //SKNP Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    uint8_t key = registers[Vx];

    if(!keypad[key]){
        pc += 2;
    }
}

void Chip8::OP_Fx07(){ //LD Vx, DT
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[Vx] = delayTimer;
}

void Chip8::OP_Fx0A(){ //LD Vx, K
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (keypad[0])
	{
		registers[Vx] = 0;
	}
	else if (keypad[1])
	{
		registers[Vx] = 1;
	}
	else if (keypad[2])
	{
		registers[Vx] = 2;
	}
	else if (keypad[3])
	{
		registers[Vx] = 3;
	}
	else if (keypad[4])
	{
		registers[Vx] = 4;
	}
	else if (keypad[5])
	{
		registers[Vx] = 5;
	}
	else if (keypad[6])
	{
		registers[Vx] = 6;
	}
	else if (keypad[7])
	{
		registers[Vx] = 7;
	}
	else if (keypad[8])
	{
		registers[Vx] = 8;
	}
	else if (keypad[9])
	{
		registers[Vx] = 9;
	}
	else if (keypad[10])
	{
		registers[Vx] = 10;
	}
	else if (keypad[11])
	{
		registers[Vx] = 11;
	}
	else if (keypad[12])
	{
		registers[Vx] = 12;
	}
	else if (keypad[13])
	{
		registers[Vx] = 13;
	}
	else if (keypad[14])
	{
		registers[Vx] = 14;
	}
	else if (keypad[15])
	{
		registers[Vx] = 15;
	}
	else
	{
		pc -= 2;
	}
}

void Chip8::OP_Fx15(){ //LD DT, Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    delayTimer = registers[Vx];
}

void Chip8::OP_Fx18(){ //LD ST, Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E(){ //ADD I, Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    index += registers[Vx];
}

void Chip8::OP_Fx29(){ //LD F, Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[Vx];

    index = START_ADD_FONT + (5 * digit);
}

void Chip8::OP_Fx33(){ //LD B, Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = registers[Vx];

    memory[index + 2] = value % 10; //ones
    value /= 10;

    memory[index + 1] = value % 10; //tens
    value /= 10;

    memory[index] = value % 10; //hundreds
}

void Chip8::OP_Fx55(){ //LD [I], Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for(uint8_t i = 0; i <= Vx; ++i){
        memory[index + i] = registers[i];
    }
}

void Chip8::OP_Fx65(){ //LD Vx, [I]
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for(uint8_t i = 0; i <= Vx; ++i){
        registers[i] = memory[index + i];
    }
}

void Chip8::OP_NULL() {}

void Chip8::Table0(){
    ((*this).*(table0[opcode & 0x000Fu]))();
}
void Chip8::Table8(){
    ((*this).*(table8[opcode & 0x000Fu]))();
}
void Chip8::TableE(){
    ((*this).*(tableE[opcode & 0x000Fu]))();
}
void Chip8::TableF(){
    ((*this).*(tableF[opcode & 0x00FFu]))();
}