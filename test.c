// TODO :
// Voir jump qui ne marche pas au moment de la construction de hex_instruction
// Makefile

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

// Structure d'une instruction basique
struct instruction {
    bool ivf;
    int opcode;
    int ope1;
    int ope2;
    int dest;
    int iv;
};

// Structure d'une instruction de type jump
struct j_instruction {
    int bcc;
    bool sign;
    int new_pc;
};

// Variables ProgramCounter et valeur de Comparaison
int PC = 0;
int CMP = 0;
bool CARRY = 0;
bool SUB_CARRY = 0;

unsigned long long* init_registers(char* state, unsigned long long* registers);
unsigned long fetch(char* file);
struct instruction decode(unsigned long operation);
void execute(struct instruction i, unsigned long long* registers);

int main(int argc, char **argv) {
    // On vérifie qu'on a bien récupéré 2 arguments lors de l'execution du programme
    if (argc != 3) {
        printf("Erreur : nombre d'arguments incorrect. Usage : %s fichier_binaire init_registres\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Création des registres et allocation mémoire
    unsigned long long* registers;
    registers = (unsigned long long*) malloc(16 * sizeof(unsigned long long));
    
    // Initialisation des registres suivant le fichier d'initialisation donné
    registers = init_registers(argv[2], registers);
    for(int i = 0; i<16; i++){
        printf("R%d = %llx, ", i, registers[i]);
    }
    printf("\n");

    bool stop = 1;
    // On fait tourner le programme en boucle jusqu'a ce que la fonction fetch renvoie "0x0" (indique la fin du fichier)
    while(stop){
        printf("---------- NEW INSTRUCTION ----------\n");
        struct instruction inst;
        unsigned long hex_instruction;

        printf("...fetching the instruction...\n");
        hex_instruction = fetch(argv[1]);

        // Vérification de fin de fichier
        if(hex_instruction == 0x0){
            printf("Fin du programme\n");
            return 0;
        }

        printf("...decoding the instruction...\n");
        inst = decode(hex_instruction);

        printf("...executing the instruction...\n");
        execute(inst, registers);
    }
}

// Fonction d'initialisation des registres en lisant le fichier passé en paramètres
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
        // Utilisation d'une regex pour récupérer dynamiquement les valeurs des différents registres
        if(sscanf(buffer, "R%d=0x%llx", &i, &val) == 2){
            registers[i] = val;
        }
    }

    fclose(file);
    return registers;
}

// Fonction de lecture d'instruction et calcul du nouveau PC
unsigned long fetch(char* instruction_file){
    FILE *file = NULL;
    int i;

    // Création de la variable de stockage de l'instruction + allocation mémoire
    unsigned char* instruction;
    instruction = malloc(4);
    unsigned long hex_instruction = 0;

    file = fopen(instruction_file, "rb");

    if(file==NULL){
        printf("Erreur dans l'ouverture du fichier instructions\n");
        exit(1);   
    }

    // On place le curseur au bon endroit dans le fichier d'instruction en fonction du PC avant de la lire
    fseek(file, 4*PC, SEEK_SET);
    int n = fread(instruction, 4, 1, file);

    // Si on a pas lu un bloc entier avec fread -> fin du fichier
    if (n != 1) {
        printf("Erreur lors de la lecture du fichier d'instructions\n");
    }

    // On vérifie si on est bien à la fin du fichier
    if (feof(file)) {
        printf("Fin du fichier atteinte\n");
        return 0x0;
    }

    // On affiche l'instruction lue (pour du debug)
    printf("fetched instruction : ");
    for(i=0;i<4;i++){
        printf("%02x ", instruction[i]);
    }
    printf("\n");

    // On reconstruit l'instruction en hexadécimal
    hex_instruction = (instruction[0]<<24) | (instruction[1]<<16) | (instruction[2]<<8) | instruction[3];

    printf("hex instruction : %lx\n", hex_instruction);

    // S'il s'agit d'une instruction de type jump :
    if(((hex_instruction && 0xf0000000) >> 28) != 0x0){
        printf("Branch Instruction\n");
        struct j_instruction inst;

        // On décode l'instruction jump directement
        inst.bcc = (hex_instruction && 0xf0000000) >> 28;
        inst.sign = (hex_instruction && 0x08000000) >> 27;
        inst.new_pc = (hex_instruction && 0x07ffffff);

        printf("bcc : %d / sign : %d / new_pc : %d\n", inst.bcc, inst.sign, inst.new_pc);

        // On calcule la nouvelle valeur du PC
        PC = inst.new_pc;

        return 0;
    }

    // Sinon on incrémente simplement le PC et on retourne l'instruction en hexa
    PC = PC+1;

    fclose(file);

    return hex_instruction;
}

// Parsing de l'instruction hexa
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

// Execution de l'instruction en fonction de l'opcode
void execute(struct instruction inst, unsigned long long* registers){
    switch(inst.opcode){
        case 0:
            printf("AND\n");
            if(inst.ivf){
                registers[inst.dest] = registers[inst.ope1] && inst.iv;
            } else {
                registers[inst.dest] = registers[inst.ope1] && registers[inst.ope2];
            }
            break;
        case 1:
            printf("OR\n");
            if(inst.ivf){
                registers[inst.dest] = registers[inst.ope1] || inst.iv;
            } else {
                registers[inst.dest] = registers[inst.ope1] || registers[inst.ope2];
            }
            break;
        case 2:
            printf("XOR\n");
            if(inst.ivf){
                registers[inst.dest] = registers[inst.ope1] != inst.iv;
            } else {
                registers[inst.dest] = registers[inst.ope1] != registers[inst.ope2];
            }
            break;
        case 3:
            // On vérifie qu'il n'y aura pas de dépassement
            // Si dépassement, carry = 1 et on fait le calcul normalement (le résultat sera la partie droite du nombre final)
            printf("ADD\n");
            if(inst.ivf){
                // On check quel est le max des 2 opérandes
                if(registers[inst.ope1]>=inst.iv){
                    // Un calcul + comparaison pour déterminer s'il y aura dépassement
                    if((ULLONG_MAX-registers[inst.ope1])<inst.iv){
                        CARRY = 1;
                    }
                } else {
                    if((ULLONG_MAX-inst.iv)<registers[inst.ope1]){
                        CARRY = 1;
                    }
                }
                registers[inst.dest] = registers[inst.ope1] + inst.iv;
            } else {
                if(registers[inst.ope1]>=registers[inst.ope2]){
                    if((ULLONG_MAX-registers[inst.ope1])<registers[inst.ope2]){
                        CARRY = 1;
                    }
                } else {
                    if((ULLONG_MAX-registers[inst.ope2])<registers[inst.ope1]){
                        CARRY = 1;
                    }
                }
                registers[inst.dest] = registers[inst.ope1] + registers[inst.ope2];
            }
            break;
        case 4:
            // On ajoute toujours la carry, qui vaut 0 de toute façon s'il n'y en a pas
            printf("ADC\n");
            if(inst.ivf){
                registers[inst.dest] = registers[inst.ope1] + inst.iv + (int)CARRY;
            } else {
                registers[inst.dest] = registers[inst.ope1] + registers[inst.ope2] + (int)CARRY;
            }
            CARRY = 0;
            break;
        case 6:
            // Même procédé que pour ADD
            printf("SUB\n");
            if(inst.ivf){
                if(registers[inst.ope1]>=inst.iv){
                    if((ULLONG_MAX-registers[inst.ope1])>inst.iv){
                        SUB_CARRY = 1;
                    }
                } else {
                    if((ULLONG_MAX-inst.iv)>registers[inst.ope1]){
                        SUB_CARRY = 1;
                    }
                }
                registers[inst.dest] = registers[inst.ope1] - inst.iv;
            } else {
                if(registers[inst.ope1]>=registers[inst.ope2]){
                    if((ULLONG_MAX-registers[inst.ope1])>registers[inst.ope2]){
                        SUB_CARRY = 1;
                    }
                } else {
                    if((ULLONG_MAX-registers[inst.ope2])>registers[inst.ope1]){
                        SUB_CARRY = 1;
                    }
                }
                registers[inst.dest] = registers[inst.ope1] - registers[inst.ope2];
            }
            break;
        case 7:
            printf("SBC\n");
            if(inst.ivf){
                registers[inst.dest] = registers[inst.ope1] - inst.iv + (int)CARRY;
            } else {
                registers[inst.dest] = registers[inst.ope1] - registers[inst.ope2] - (int)CARRY;
            }
            SUB_CARRY = 0;
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
        default:
            printf("default case reached in execution switch\n");
    }
    // Affichage de la valeur des registres en fin d'instruction
    for(int i = 0; i<16; i++){
        printf("R%d = %llx, ", i, registers[i]);
    }
    printf("\n");
}