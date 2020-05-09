import os
import sys
if __name__ == '__main__':
    lines=[]
    with open("elastic-3.cpp",'r') as f:
        for line in f:
            lines.append(line)

    lines[147] = "for(test=1;test<=1;test++)\n"
    lines[148] = "//for(test=3000;test<=3000;test+=1000)//这是为了跑所有节点容量设置为一样，这里test是cm的宽度\n"

    with open("elastic-3.cpp",'w') as f:
        f.writelines(lines)
    os.system("make")
    os.system("./elastic-3.out")

    lines[147] = "//for(test=1;test<=1;test++)\n"
    lines[148] = "for(test=3000;test<=3000;test+=1000)//这是为了跑所有节点容量设置为一样，这里test是cm的宽度\n"

    with open("elastic-3.cpp",'w') as f:
        f.writelines(lines)
    os.system("make")
    os.system("./elastic-3.out")
'''
    for heavy in range(500,3000,250):
        print("#######################"+str(heavy)+"_"+"1"+"##########################")
        s="#define COUNTER_NUM "+str(heavy)+"\n"
        lines[145]=s
        with open("elastic-3.cpp",'w') as f:
            f.writelines(lines)
        os.system("make")
        os.system("./elastic-3.out")

    # 500_3000 3000_3000
    lines[147] = "//for(test=1;test<=1;test++)\n"
    lines[148] = "for(test=3000;test<=3000;test+=1000)//这是为了跑所有节点容量设置为一样，这里test是cm的宽度\n"
    for heavy in range(500,3000,250):
        print("#########################"+str(heavy)+"_"+"3000"+"########################")
        s="#define COUNTER_NUM "+str(heavy)+"\n"
        lines[145]=s
        with open("elastic-3.cpp",'w') as f:
            f.writelines(lines)
        os.system("make")
        os.system("./elastic-3.out")
'''





