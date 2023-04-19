def compiler(filename):
    #Ouvrir le fichier assembleyr
    file = open(filename + ".s", "r")
    line = file.readline()  

    #Ouvrir le fichier binaire
    binary_file = open(filename + ".b", "wb")
    #Fichier texte de test
    test_file = open("test.txt", "w")

    #Compilation ligne par ligne
    while line:
        compileLine(line, binary_file, test_file)
        line = file.readline()

def decToBin(nb):
    if nb == 0:
        return "0"
    else:
        ret = ""
        while nb != 0:
            ret = str(nb%2) + ret
            nb = nb // 2
        while len(ret)%8 != 0:
            ret= "0" + ret
        return ret

def hexaToDec(nb_str):
    ret = 0
    coeff = 0
    for i in reversed(nb_str):
        if i == 'a':
            ret = ret + 10 * 16**coeff
        elif i == 'b':
            ret = ret + 11 * 16**coeff
        elif i == 'c':
            ret = ret + 12 * 16**coeff
        elif i == 'd':
            ret = ret + 13 * 16**coeff
        elif i == 'e':
            ret = ret + 14 * 16**coeff
        elif i == 'f':
            ret = ret + 15 * 16**coeff
        else:
            ret = ret + int(i) * 16**coeff
        coeff += 1
    return ret


def hexaToBin(nb_str):
    if nb_str[0:2] == "0x":
        nb_str=nb_str.replace(nb_str[0:2],"",1)
    nb = hexaToDec(nb_str)
    nb_bin = decToBin(nb)
    return nb_bin

def compileLine(line, binary_file, test_file):
    dicobcc = {"BEQ":"1001", "BNE":"1010", "BLE":"1011", "BGE":"1100", "BL ":"1101", "BG ":"1111"}
    dicocode = {"AND":"0000", "ORR":"0001", "EOR":"0010", "ADD":"0011", "ADC":"0100", 
            "CMP":"0101", "SUB":"0110", "SBC":"0111", "MOV":"1000", "LSH":"1001", "RSH":"1010"}
    dicoreg = {"r0": "0000", "r1":"0001", "r2":"0010", "r3":"0011", "r4":"0100", "r5":"0101", "r6":"0110", "r7":"0111", "r8":"1000", "r9":"1001", "r10":"1010", "r11":"1011", "r12":"1100", "r13":"1101", "r14":"1110", "r15":"1111"}
    inst = ""

    line = line.replace("\n", "",1)

    #Cas d'une Branch Condition
    if line[0] == "B":
        if line[1] == " ":
            inst = inst + "1000"
            line = line.replace(line[0:2],"",1)
        else:
            inst = inst + dicobcc[line[0:3]]
            line = line.replace(line[0:4],"",1)
            if line[0] == " ":
                line = line.replace(line[0],"",1)
        if line[0] == "-":
            inst = inst + "1"
            line = line.replace(line[0],"",1)
        val = int(line)
        binval = decToBin(val)
        while len(binval) < 26:
            binval = "0" + binval
        inst = inst + binval
        bin_inst = int(inst, 2)
        octet = bin_inst.to_bytes(4, byteorder='big', signed=False)
        binary_file.write(octet)
        test_file.write(inst)

    #Cas d'une instruction classique
    else:
        inst = inst + "0000000"

        #Vérification de la présence d'une immediat value
        iv=""
        verif_iv = False
        for i in range(0, len(line)):
            if line[i] == " " and line[i+1] != "r":
                verif_iv = True
        if verif_iv:
            inst = inst + "1"
        else:
            inst = inst + "0"
            iv = "00000000"
        
        #Conversion du code de l'instruction
        inst = inst + dicocode[line[0:3]]
        line = line.replace(line[0:4],"",1)

        #Récupération du registre de destination
        dest = dicoreg[line[0:2]]
        line = line.replace(line[0:4],"",1)


        while len(line) > 0:
            if line[0] == "r":
                inst = inst + dicoreg[line[0:2]]
                line = line.replace(line[0:4],"",1)
            else:
                inst = inst + "0000"
                j=0
                while line[j] != " " and j<len(line)-1:
                    j=j+1
                if j<len(line):
                    if line[0:2] == "0x":
                        iv = hexaToBin(line[0:j+1])
                    else:
                        iv = decToBin(int(line[0:j+1]))
                    line = line.replace(line[0:j+1],"",1)
                else:
                    if line[0:2] == "0x":
                        iv = hexaToBin(line[0:j])
                    else:
                        iv = decToBin(line[0:j])
                    line = line.replace(line[0:j],"",1)

        #S'il manque une opérande dans l'instruction
        if len(inst) < 20:
            inst = inst + "0000"

        #Cas d'une immediat value trop grande
        if len(iv) > 8:
            #On fait la première instruction avec le premier octet de l'immediat value
            inst = inst + dest + iv[0:8]
            bin_inst = int(inst, 2)
            octet = bin_inst.to_bytes(4, byteorder='big', signed=False)
            binary_file.write(octet)
            test_file.write(inst)
            test_file.write("\n")
            iv = iv.replace(iv[0:8],"",1)
            temp_iv = ""

            #On shift à gauche et on ajoute un octet par un octet
            while len(iv) > 0:
                if len(iv) >= 8:
                    temp_iv = iv[0:8]
                    iv = iv.replace(iv[0:8],"",1)
                else:
                    temp_iv = iv
                    iv = ""
                #shift
                shift = "000000011001" + dest + "0000" + dest + "00001000"
                bin_shift = int(shift, 2)
                shift_octet = bin_shift.to_bytes(4, byteorder='big', signed=False)
                binary_file.write(shift_octet)
                test_file.write(shift)
                test_file.write("\n")
                #ajout d'un octet
                add = "000000010011" + dest + "0000" + dest + temp_iv
                bin_add = int(add, 2)
                add_octet = bin_add.to_bytes(4, byteorder='big', signed=False)
                binary_file.write(add_octet)
                test_file.write(add)
                test_file.write("\n")

        #Cas d'une immediat value de taille inférieure à un octet
        else:
            inst = inst + dest + iv
            bin_inst = int(inst, 2)
            octet = bin_inst.to_bytes(4, byteorder='big', signed=False)
            binary_file.write(octet)
            test_file.write(inst)

    
        

compiler("lshift64_128_test")
# b_file = open("b_test.b", "wb")
# one_byte = int('00000001001100000000000000100011', 2)
# inte = 19922979
# single_byte = inte.to_bytes(4, byteorder='big', signed=False) 
# bytetest = b'\x010\x00#'
# int_val = int.from_bytes(bytetest, "big")
# print(int_val)
# b_file.write(single_byte)
# b_file.close()

# with open("b_test.b", "rb") as bin_file:
#     data = bin_file.read()
#     print(data)

with open("lshift64_128_test.b", "rb") as bin_file:
    data = bin_file.read()
    print(data)



    
