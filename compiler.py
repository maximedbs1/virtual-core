def compiler(filename):
    file = open(filename + ".s", "r")
    line = file.readline()
    binary_file = open(filename + ".b", "ab")
    test_file = open("test.txt", "w")
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
        nb_str=nb_str.replace(nb_str[0:2],"")
    nb = hexaToDec(nb_str)
    nb_bin = decToBin(nb)
    while len(nb_bin)%8 != 0:
        nb_bin= "0" + nb_bin
    return nb_bin

def compileLine(line, binary_file, test_file):
    dicobcc = {"BEQ":"1001", "BNE":"1010", "BLE":"1011", "BGE":"1100", "BL ":"1101", "BG ":"1111"}
    dicocode = {"AND":"0000", "ORR":"0001", "EOR":"0010", "ADD":"0011", "ADC":"0100", 
            "CMP":"0101", "SUB":"0110", "SBC":"0111", "MOV":"1000", "LSH":"1001", "RSH":"1010"}
    dicoreg = {"r0": "0000", "r1":"0001", "r2":"0010", "r3":"0011", "r4":"0100", "r5":"0101", "r6":"0110", "r7":"0111", "r8":"1000", "r9":"1001", "r10":"1010", "r11":"1011", "r12":"1100", "r13":"1101", "r14":"1110", "r15":"1111"}
    inst = ""

    line = line.replace("\n", "")

    #Cas d'une Branch Condition
    if line[0] == "B":
        if line[1] == " ":
            inst = inst + "1000"
            line = line.replace(line[0:2],"")
        else:
            inst = inst + dicobcc[line[0:3]]
            line = line.replace(line[0:4],"")
            if line[0] == " ":
                line = line.replace(line[0],"")
        if line[0] == "-":
            inst = inst + "1"
            line = line.replace(line[0],"")
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

        #V??rification de la pr??sence d'une immediat value
        verif_iv = False
        for i in range(0, len(line)):
            if line[i] == " " and line[i+1] != "r":
                verif_iv = True
        if verif_iv:
            inst = inst + "1"
        else:
            inst = inst + "0"
        
        #Conversion du code de l'instruction
        inst = inst + dicocode[line[0:3]]
        line = line.replace(line[0:4],"")

        #R??cup??ration du registre de destination
        dest = dicoreg[line[0:2]]
        line = line.replace(line[0:4],"")

        iv=""
        while len(line) > 0:
            if line[0] == "r":
                inst = inst + dicoreg[line[0:2]]
                line = line.replace(line[0:4],"")
            else:
                inst = inst + "0000"
                j=0
                while line[j] != " " and j<len(line)-1:
                    j=j+1
                if j<len(line):
                    if line[0:2] == "0x":
                        iv = hexaToBin(line[0:j+1])
                    else:
                        iv = decToBin(line[0:j+1])
                    line = line.replace(line[0:j+1],"")
                else:
                    if line[0:2] == "0x":
                        iv = hexaToBin(line[0:j])
                    else:
                        iv = decToBin(line[0:j])
                    line = line.replace(line[0:j],"")

        #S'il manque une op??rande dans l'instruction
        if len(inst) < 20:
            inst = inst + "0000"

        #Cas d'une immediat value trop grande
        if len(iv) > 8:
            #On fait la premi??re instruction avec le premier octet de l'immediat value
            inst = inst + dest + iv[0:8]
            bin_inst = int(inst, 2)
            octet = bin_inst.to_bytes(4, byteorder='big', signed=False)
            binary_file.write(octet)
            test_file.write(inst)
            iv = iv.replace(iv[0:8],"")
            temp_iv = ""

            #On shift ?? gauche et on ajoute un octet par un octet
            while len(iv) > 0:
                if len(iv) >= 8:
                    temp_iv = iv[0:8]
                    iv = iv.replace(iv[0:8],"")
                else:
                    temp_iv = iv
                    iv = ""
                #shift
                shift = "00000001100100000000" + dest + "00001000"
                bin_shift = int(shift, 2)
                shift_octet = bin_shift.to_bytes(4, byteorder='big', signed=False)
                binary_file.write(shift_octet)
                test_file.write(shift)
                #ajout d'un octet
                add = "00000001001100000000" + dest + temp_iv
                bin_add = int(add, 2)
                add_octet = bin_add.to_bytes(4, byteorder='big', signed=False)
                binary_file.write(add_octet)
                test_file.write(add)

        #Cas d'une immediat value de taille inf??rieure ?? un octet
        else:
            inst = inst + dest + iv
            bin_inst = int(inst, 2)
            octet = bin_inst.to_bytes(4, byteorder='big', signed=False)
            binary_file.write(octet)
            test_file.write(inst)

    
        

compiler("init_test")
# b_file = open("b_test.b", "wb")
# test = bytes.fromhex('af50')
# te = b'1000'
# one_byte = int('11111000', 2)
# single_byte = one_byte.to_bytes(1, byteorder='big', signed=False) 
# b_file.write(single_byte)
# b_file.close()

with open("init_test.b", "rb") as bin_file:
    data = bin_file.read()
    print(data)

    
