#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct instruction {
    bool ivf;
    int opcode;
    int ope1;
    int ope2;
    int dest;
    int iv;
};

unsigned long long* init_registers(char* state, unsigned long long* registers);
unsigned long fetch(char* file, int pc);
struct instruction decode(unsigned long operation);
void execute(struct instruction i, unsigned long long* registers);

int main(int argc, char **argv) {
    //TODO: if argc != 2 -> erreur nb d'arg invalide, exit

    unsigned long long* registers;
    registers = (unsigned long long*) malloc(16 * sizeof(unsigned long long));
    
    registers = init_registers(argv[2], registers);
    for(int i = 0; i<16; i++){
        printf("R%d = %llx, ", i, registers[i]);
    }
    printf("\n");

    for(int i=0; i<45; i++){
        printf("---------- NEW INSTRUCTION ----------\n");
        struct instruction inst;
        unsigned long hex_instruction;

        printf("...fetching the instruction...\n");
        hex_instruction = fetch(argv[1], i);

        printf("...decoding the instruction...\n");
        inst = decode(hex_instruction);

        printf("...executing the instruction...\n");
        execute(inst, registers);
    }
}

unsigned long long* init_registers(char* state_file, unsigned long long* registers){
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
    int i;

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

    //TODO: if last 4bit != 0000 -> decode this specific structure of operation
    //TODO: compute the new value of the pc (+1 or specific number)

    return hex_instruction;
}

struct instruction decode(unsigned long operation){
    struct instruction inst;

    inst.ivf = (operation & 0x0f000000) >> 24;
    inst.opcode = (operation & 0x00f00000) >> 20;
    inst.ope1 = (operation & 0x000f0000) >> 16;
    inst.ope2 = (operation & 0x0000f000) >> 12;
    inst.dest = (operation & 0x00000f00) >> 8;
    inst.iv = (operation & 0x000000ff);

    printf("ivf : %d / opcode : %x / ope1 : %x / ope2 : %x / dest : %x / iv : %x\n", inst.ivf, inst.opcode, inst.ope1, inst.ope2, inst.dest, inst.iv);

    return inst;
}

void execute(struct instruction inst, unsigned long long* registers){
    switch(inst.opcode){
        //AND
        // case 0:
        //     printf("AND\n");
        //     if(i.ivf){
        //         registers[i.dest] = registers[i.ope1] && i.iv;
        //     } else {
        //         registers[i.dest] = registers[i.ope1] && registers[i.ope2];
        //     }
        //     break;
        // //OR
        // case 1:
        //     printf("OR\n");
        //     if(i.ivf){
        //         registers[i.dest] = registers[i.ope1] || i.iv;
        //     } else {
        //         registers[i.dest] = registers[i.ope1] || registers[i.ope2];
        //     }
        //     break;
        // //XOR
        // case 2:
        //     printf("XOR\n");
        //     if(i.ivf){
        //         registers[i.dest] = registers[i.ope1] != i.iv;
        //     } else {
        //         registers[i.dest] = registers[i.ope1] != registers[i.ope2];
        //     }
        //     break;
        case 3:
            printf("ADD\n");
            if(inst.ivf){
                registers[inst.dest] = registers[inst.ope1] + inst.iv;
            } else {
                registers[inst.dest] = registers[inst.ope1] + registers[inst.ope2];
            }
            break;
        case 6:
            printf("SUB\n");
            if(inst.ivf){
                registers[inst.dest] = registers[inst.ope1] - inst.iv;
            } else {
                registers[inst.dest] = registers[inst.ope1] - registers[inst.ope2];
            }
            break;
        case 8:
            printf("MOVE\n");
            if(inst.ivf){
                registers[inst.dest] = inst.iv;
            } else {
                registers[inst.dest] = registers[inst.ope2];
            }
            break;
        case 9:
            printf("LEFT SHIFT\n");
            if(inst.ivf){
                registers[inst.dest] = registers[inst.ope1] << inst.iv;
            } else {
                registers[inst.dest] = registers[inst.ope1] << registers[inst.ope2];
            }
            break;
        case 10:
            printf("RIGHT SHIFT\n");
            if(inst.ivf){
                registers[inst.dest] = registers[inst.ope1] >> inst.iv;
            } else {
                registers[inst.dest] = registers[inst.ope1] >> registers[inst.ope2];
            }
            break;
        //TODO: continuer pour tous les opcodes existants
        default:
            printf("default case reached in execution switch\n");
    }
    for(int i = 0; i<16; i++){
        printf("R%d = %llx, ", i, registers[i]);
    }
    printf("\n");
}