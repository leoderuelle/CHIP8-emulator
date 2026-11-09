#pragma once
#include <iostream>
#include <cstdint>
#include <array>
#include <fstream>

class Chip8{
    public:
        int init(){
            ram.fill(0);

            for (int i = 0; i < 80; ++i) {
                ram[0x50 + i] = fontset[i];
            }

            registre.fill(0);
            index_register = 0;
            stack.fill(0);
            program_counter = 512;
            stack_pointer = 0;
            delay_timer = 0;
            sound_timer = 0;
            ecran = {};
            clavier.fill(0);
            return 0;
        }

        bool loadROM(const std::string& filename){
            std::ifstream fichier(filename, std::ios::binary | std::ios::ate);

            if(!fichier.is_open()){
                std::cout << "impossible de lire le ficher";
                return false;
            }

            int taille = fichier.tellg();

            if(taille > ram.size() - program_counter){
                std::cout << "programme trop volumineux";
                return false;
            }
            
            fichier.seekg(0, std::ios::beg);

            fichier.read(reinterpret_cast<char*>(ram.data()) + program_counter, taille);
            return true;
        }

        int getRam(int nombre){
            return ram.at(nombre);
        }

        bool getEcran(int x, int y){
            return ecran[y][x];
        }

        void setKey(int key, bool isPressed){
            clavier[key] = isPressed;
        }

        void decrementTimers() {
        if (delay_timer > 0) {
            delay_timer--;
        }
        if (sound_timer > 0) {
            sound_timer--;
        }
}

        void emulateCycle(){
            uint16_t opcode;
            uint8_t x;
            uint8_t y;
            uint8_t n;
            uint8_t nn;
            uint16_t nnn;
            int x_pos;
            int y_pos;
            uint8_t spriteByte;
            int x_pixel;
            int y_pixel;

            opcode = (ram.at(program_counter) << 8) | ram.at(program_counter+1);
            program_counter += 2;

            x = (opcode & 0x0F00) >> 8;
            y = (opcode & 0x00F0) >> 4;
            n = opcode & 0x000F;
            nn = opcode & 0x00FF;
            nnn = opcode & 0x0FFF;

            switch (opcode & 0xF000) {
                case 0x0000:
                    switch (opcode & 0x00FF) {
                        case 0x00E0:
                            for (auto& ligne : ecran) {
                                ligne.fill(false);
                            }
                            break;
                        case 0x00EE:
                        stack_pointer--;
                            program_counter = stack[stack_pointer];
                            break;
                        default:
                            std::cout << "Opcode inconnu : " << opcode;
                    }
                    break;

                case 0x1000: // JP addr (Saut à NNN)
                    program_counter = nnn;
                    break;
                case 0x2000:
                    stack[stack_pointer] = program_counter;
                    stack_pointer++;
                    program_counter = nnn;
                    break;
                case 0x3000:
                    if(registre.at(x) == nn){
                        program_counter += 2;
                    }
                    break;
                case 0x4000:
                    if(registre.at(x) != nn){
                        program_counter += 2;
                    }
                    break;
                case 0x5000:
                    if(registre.at(x) == registre.at(y)){
                        program_counter += 2;
                    }
                    break;
                case 0x6000:
                    registre[x] = nn;
                    break;
                case 0x7000:
                    registre[x] += nn;
                    break;
                case 0x8000:
                    switch (opcode & 0x000F) {
                        case 0x0000:
                            registre[x] = registre.at(y);
                            break;
                        case 0x0001:
                            registre[x] = registre.at(x) | registre.at(y);
                            break;
                        case 0x0002:
                            registre[x] = registre.at(x) & registre.at(y);
                            break;
                        case 0x0003:
                            registre[x] = registre.at(x) ^ registre.at(y);
                            break;
                        case 0x0004:
                            if(registre.at(x)+registre.at(y)>255){
                                registre[15] = 1;
                                registre[x] = registre.at(x)+registre.at(y)-256;
                            }
                            else{
                                registre[15] = 0;
                                registre[x]=registre.at(x)+registre.at(y);
                            }
                            break;
                        case 0x0005:
                            if(registre.at(x)<registre.at(y)){
                                registre[15] = 0;
                                registre[x] = (registre.at(x)+256)-registre.at(y);
                            }
                            else{
                                registre[15]=1;
                                registre[x] = registre.at(x)-registre.at(y);
                            }
                            break;
                        case 0x0006:
                            registre[15] = registre[x] & 0x1;
                            registre[x] = registre.at(x) >> 1;
                            break;
                        case 0x0007:
                            if(registre.at(x)>registre.at(y)){
                                registre[15] = 0;
                                registre[x] = (registre.at(y)+256)-registre.at(x);
                            }
                            else{
                                registre[15]=1;
                                registre[x] = registre.at(y)-registre.at(x);
                            }
                            break;
                        case 0x000E:
                            registre[15] = (registre[x] & 0x80) >> 7;
                            registre[x] = registre.at(x) << 1;
                            break;
                        default:
                            std::cout << "Opcode inconnu : " << opcode;    
                    }
                    break;
                    case 0x9000:
                        if(registre.at(x) != registre.at(y)){
                            program_counter += 2;
                        }
                        break;
                    case 0xA000:
                        index_register = nnn;
                        break;
                    case 0xB000:
                        program_counter = nnn + registre.at(0);
                        break;
                    case 0xC000:
                        registre[x] = rand() & nn;
                        break;
                    case 0xD000:
                        x_pos = registre[x]%64;
                        y_pos = registre[y]%32;
                        registre[15] = 0;
                        for(int i = 0; i<n;i++){
                            spriteByte = ram[index_register + i];
                            for(int j = 0;j<8;j++){
                                if(((0x80 >> j) & spriteByte) != 0){
                                    x_pixel = (x_pos + j)%64;
                                    y_pixel = (y_pos+i)%32;
                                    if(ecran[y_pixel][x_pixel]){
                                        registre[15] = 1;
                                        ecran[y_pixel][x_pixel] = false;
                                    }
                                    else{
                                        ecran[y_pixel][x_pixel] = true;
                                    }
                                }
                            }
                        }
                        break;
                    case 0xE000:
                        switch (opcode & 0x00FF) {
                            case 0x009E:
                                if(clavier[registre[x]]){
                                    program_counter += 2;
                                }
                                break;
                            case 0x00A1:
                                if(!clavier[registre[x]]){
                                    program_counter += 2;
                                }
                                break;
                            default:
                                std::cout << "Opcode inconnu : " << opcode;
                        }
                        break;
                    case 0xF000:
                        switch (opcode & 0x00FF) {
                            case 0x0007:
                                registre[x] = delay_timer;
                                break;
                            case 0x000A:
                            for(int i = 0; i < clavier.size();i++){
                                if(clavier[i] == true){
                                    registre[x] = i;
                                    break;
                                }
                                program_counter -= 2;
                            }
                                break;
                            case 0x0015:
                                delay_timer = registre.at(x);
                                break;
                            case 0x0018:
                                sound_timer = registre.at(x);
                                break;
                            case 0x001E:
                                index_register += registre.at(x);
                                break;
                            case 0x0029:
                                index_register = 0x50 + (5 * registre.at(x));
                                break;
                            case 0x0033:
                                ram[index_register] = registre.at(x) / 100;
                                ram[index_register+1] = (registre.at(x) / 10)%10;
                                ram[index_register+2] = registre.at(x) % 10;
                                break;
                            case 0x0055:
                                for(int i = 0; i <= x; i++){
                                    ram[index_register+i] = registre.at(i);
                                }
                                break;
                            case 0x0065:
                                for(int i = 0; i <= x; i++){
                                    registre[i] = ram.at(index_register+i);
                                }
                                break;
                                default:
                                    std::cout << "Opcode inconnu : " << opcode;
                        }
                        break;
                        default:
                            std::cout << "Opcode inconnu : " << opcode;
            }
        }

    private:
        std::array<uint8_t, 4096> ram; 
        uint16_t index_register;
        std::array<uint8_t,16> registre;
        uint16_t program_counter;
        std::array<uint16_t, 16> stack;
        uint16_t stack_pointer;
        uint8_t delay_timer;
        uint8_t sound_timer;
        std::array<std::array<bool,64>,32> ecran;
        std::array<bool,16> clavier;

        const std::array<uint8_t, 80> fontset = {
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
};