#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

unsigned long long* init_registers(char* state);
unsigned long fetch(char* file, int pc);
int decode(unsigned long operation);
int execute(bool ivf, int opcode, int ope1, int ope2, int dest, int iv, unsigned long long **registers);

int main(int argc, char **argv) {
    unsigned long long* registers;
    registers = (unsigned long long*) malloc(16 * sizeof(unsigned long long));
    //TODO: if argc != 2 -> erreur nb d'arg invalide, exit
    
    registers = init_registers(argv[2]);
    for(int i = 0; i<16; i++){
        printf("R%d = %llu\n", i, registers[i]);
    }

    //printf("fetching the instruction ...\n");
    //hex_instruction = fetch("init_test.b", 2);

    //printf("decoding the instruction ...\n");
    //decode(hex_instruction);
}

unsigned long long* init_registers(char* state_file){
    unsigned long long* registers;
    registers = (unsigned long long*) malloc(16 * sizeof(unsigned long long));

    FILE *file = NULL;
    file = fopen(state_file, "r");

    if(file==NULL){
        printf("Erreur dans l'ouverture du fichier des registres\n");
        exit(1);
    }

    char buffer[256];
    int i=0;

    while(fgets(buffer, 256, file) && i<16){
        unsigned long long val;
        if(sscanf(buffer, "R%d=0x%llx", &i, &val) == 2){
            registers[i] = val;
        }
    }

    fclose(file);
    return registers;
}

unsigned long fetch(char* instruction_file, int pc){
    FILE *file = NULL;
    int c, i;

    unsigned char* instruction;
    instruction = malloc(4);
    unsigned long hex_instruction = 0;

    file = fopen(instruction_file, "rb");

    if(file==NULL){
        printf("Erreur dans l'ouverture du fichier instructions\n");
        exit(1);   
    }

    fseek(file, 4*pc, SEEK_SET);
    fread(instruction, 4, 1, file);

    printf("fetched instruction : ");
    for(i=0;i<4;i++){
        printf("%02x ", instruction[i]);
    }
    printf("\n");

    hex_instruction = (instruction[0]<<24) | (instruction[1]<<16) | (instruction[2]<<8) | instruction[3];

    printf("hex instruction : %lx\n", hex_instruction);

    fclose(file);

    return hex_instruction;

    //TODO: read instruction 32*pc in file
    //TODO: if last 4bit != 0000 -> decode this specific structure of operation
    //TODO: compute the new value of the pc (+1 or specific number)
}

int decode(unsigned long operation){
    bool ivf = (operation & 0x0f000000) >> 24;
    int opcode = (operation & 0x00f00000) >> 20;
    int ope1 = (operation & 0x000f0000) >> 16;
    int ope2 = (operation & 0x0000f000) >> 12;
    int dest = (operation & 0x00000f00) >> 8;
    int iv = (operation & 0x000000ff);

    printf("ivf : %d / opcode : %x / ope1 : %x / ope2 : %x / dest : %x / iv : %x\n", ivf, opcode, ope1, ope2, dest, iv);

    return 0;
}

int execute(bool ivf, int opcode, int ope1, int ope2, int dest, int iv, unsigned long long **registers){
    switch(opcode){
        //AND
        case 0:
            if(ivf){
                dest = registers[ope1] && iv;
            } else {
                dest = registers[ope1] && registers[ope2];
            }
            break;
        //OR
        case 1:
            if(ivf){
                dest = registers[ope1] || iv;
            } else {
                dest = registers[ope1] || registers[ope2];
            }
            break;
        //XOR
        case 2:
            if(ivf){
                dest = registers[ope1] != iv;
            } else {
                dest = registers[ope1] != registers[ope2];
            }
            break;
        //TODO: continuer pour tous les opcodes existants
    return 0;
    }
}