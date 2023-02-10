def compiler(filename):
    file = open(filename + ".s", "r")
    line = file.readline()
    print(line)
    print(len(line))
    line = line.replace(line[0],"")
    print(line)
    line = line.replace(line[0:3],"")
    print(line)
    #binary_file = open(filename + ".b", "w")
    #compileLine(line, binary_file)

compiler("init_test")

def conv_bin(nb):
    if nb == 0:
        return "0"
    else:
        ret = ""
        while nb != 0:
            ret = (nb%2) + ret
            nb = nb // 2
        return ret

def compileLine(line, binary_file):
    dicobcc = {"BEQ":"1001", "BNE":"1010", "BLE":"1011", "BGE":"1100", "BL ":"1101", "BG ":"1111"}
    dicocode = {"AND":"0000", "ORR":"0001", "EOR":"0010", "ADD":"0011", "ADC":"0100", 
            "CMP":"0101", "SUB":"0110", "SBC":"0111", "MOV":"1000", "LSH":"1001", "RSH":"1010"}
    dicoreg = {"r0": "0000", "r1":"0001", "r2":"0010", "r3":"0011", "r4":"0100", "r5":"0101", "r6":"0110", "r7":"0111", "r8":"1000", "r9":"1001", "r10":"1010", "r11":"1011", "r12":"1100", "r13":"1101", "r14":"1110", "r15":"1111"}
    inst = ""
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
        binval = conv_bin(val)
        while len(binval) < 26:
            binval = "0" + binval
        inst = inst + binval

    else:
        inst = inst + "0000000"
        i=0
        verif = False
        while (i<len(line)):
            while (line[i] != " "):
                i=i+1
            if line[i+1] != "r":
                verif = True
        if verif:
            inst = inst + "1"
        else:
            inst = inst + "0"
        inst = inst + dicocode[line[0:3]]
        line = line.replace(line[0:4],"")
        dest = dicoreg[line[0:2]]
        line = line.replace(line[0:4],"")
        iv=""
        while len(line) > 0:
            if line[0] == "r":
                inst = inst + dicoreg[line[0:2]]
                line = line.replace(line[0:4],"")
            else:
                j=0
                while line[j] != " ":
                    j=j+1
                iv = line[0:j]
                line = line.replace(line[0:j],"")
        inst = inst + dest + iv
        


         

    
