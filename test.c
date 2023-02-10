#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

unsigned long long* init_registers(char* state);
char* fetch(char* file, int pc);
int decode(long operation);

int main(int argc, char **argv) {
    //TODO: if argc != 2 -> erreur nb d'arg invalide, exit
    //init_registers(argv[2]);
    long operation = 1;
    decode(operation);
}

unsigned long long* init_registers(char* state){
    unsigned long long* registers;
    
    registers = (unsigned long long*) malloc(16 * sizeof(unsigned long long));

    FILE *file = NULL;
    char c;

    file = fopen(state,"r");

    if(file != NULL){
        
        fclose(file);
    } else {
        printf("Erreur dans l'ouverture du fichier des registres\n");
    }

    for(int i=0; i<16; i++){
        printf("%llu\n", registers[i]);
    }

    return registers;
}

char* fetch(char* code, int pc){
    //TODO: read instruction 32*pc in file
    //TODO: if last 4bit != 0000 -> decode this specific structure of operation
    //TODO: compute the new value of the pc (+1 or specific number)
}

int decode(long op){
    long operation = 0x01320104;

    bool ivf = (operation & 0x0f000000) >> 24;
    int opcode = (operation & 0x00f00000) >> 20;
    int ope1 = (operation & 0x000f0000) >> 16;
    int ope2 = (operation & 0x0000f000) >> 12;
    int dest = (operation & 0x00000f00) >> 8;
    int iv = (operation & 0x000000ff);

    printf("ivf : %d\nopcode : %x\nope1 : %x\nope2 : %x\ndest : %x\niv : %x\n", ivf, opcode, ope1, ope2, dest, iv);

    return 0;
}

int execute(bool ivf, int opcode, int ope1, int ope2, int dest, int iv, unsigned long long **registers){
    switch(opcode){
        case 0:
            if(ivf){
                dest = registers[ope1] && iv;
            } else {
                dest = registers[ope1] && registers[ope2];
            }
            break;
        case 1:
            if(ivf){
                dest = registers[ope1] || iv;
            } else {
                dest = registers[ope1] || registers[ope2];
            }
            break;
        case 2:
            if(ivf){
                dest = registers[ope1] != iv;
            } else {
                dest = registers[ope1] != registers[ope2];
            }
            break;
        case 3:
            if(ivf){
                dest = registers[ope1] + iv;
            } else {
                dest = registers[ope1] + registers[ope2];
            }
            break;
    }
}