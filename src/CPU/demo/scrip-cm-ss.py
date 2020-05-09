import os
import sys
if __name__ == '__main__':
    lines=[]
    with open("cmsketch-3.cpp",'r') as f:
        for line in f:
            lines.append(line)

    lines[116] = "for(numNode=40;numNode<=40;numNode++){\n"

    lines[150]="    for(test=1;test<=10;test++)\n"
    lines[151] = "//	for(test=3000;test<=3000;test+=1000)//这是为了跑所有节点容量设置为一样，这里test是cm的宽度\n"
    with open("cmsketch-3.cpp", 'w') as f:
        f.writelines(lines)
    os.system("make")
    os.system("./cmsketch-3.out")

    lines[150] = "//    for(test=1;test<=10;test++)\n"
    lines[151] = "	for(test=1000;test<=10000;test+=1000)//这是为了跑所有节点容量设置为一样，这里test是cm的宽度\n"
    with open("cmsketch-3.cpp", 'w') as f:
        f.writelines(lines)
    os.system("make")
    os.system("./cmsketch-3.out")

    lines[116]="for(numNode=30;numNode<=40;numNode++){\n"

    lines[150] = "    for(test=1;test<=1;test++)\n"
    lines[151] = "//	for(test=3000;test<=3000;test+=1000)//这是为了跑所有节点容量设置为一样，这里test是cm的宽度\n"
    with open("cmsketch-3.cpp", 'w') as f:
        f.writelines(lines)
    os.system("make")
    os.system("./cmsketch-3.out")

    lines[150] = "//    for(test=1;test<=1;test++)\n"
    lines[151] = "	for(test=3000;test<=3000;test+=1000)//这是为了跑所有节点容量设置为一样，这里test是cm的宽度\n"
    with open("cmsketch-3.cpp", 'w') as f:
        f.writelines(lines)
    os.system("make")
    os.system("./cmsketch-3.out")




