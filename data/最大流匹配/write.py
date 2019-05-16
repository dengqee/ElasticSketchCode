from random import *
from readtopo import readtopo
from readtopo import generatePathNode

def get_subnum(id):
    return int(id.strip().split()[-1])

def get_src(id):
    return int(id.strip().split()[1])

def get_sd_pair(id):
    return id.strip().split()[0]+' '+id.strip().split()[1]

if __name__ == "__main__":
    seed(2017220,10334)
    toponame='Geant'
    flow_node,littleFlow,flow_paths,pat,path_list=generatePathNode(toponame)
    nodes, links = readtopo()
    with open(toponame + '_LittleFlow.txt','w') as f:
        for key,value in littleFlow.items():
            f.write(str(key)+'\n')
            f.write(str(value)+'\n')
    nodesvol={}
    with open(toponame+'_FlowVolume.txt') as f:
        for line in f.readlines():
            temp=line.strip().split()
            if float(temp[-1])<5000:
                temp[-1]=5000
            else:
                temp[-1]=int(float(temp[-1]))
            nodesvol[str(int(temp[0]))+' '+str(int(temp[1]))]=temp[-1]

    flow_assign={}
    for i,item in enumerate(flow_node):
        for j in item:
            flow_assign[j]=i

    filelist=[]
    for i in range(nodes):
        f=open(toponame+'_'+str(i)+'_balanced_packets.txt','w')
        filelist.append(f)

    orifilelist=[]
    for i in range(nodes):
        f=open(toponame+'_'+str(i)+'_origional_packets.txt','w')
        orifilelist.append(f)

    packets_list=[]
    print('111111111')

    for key, value in littleFlow.items():
        sum = 0
        offset = []
        for i in range(value):
            sum += (1 / (value - i))
        offset.append((1 / value))
        offset[0] = offset[0] / sum
        for i in range(1, value):
            offset.append(offset[i - 1] + (1 / (value - i)) / sum)
        dictionary = list(range(value))
        shuffle(dictionary)
        for k in range(nodesvol[key]):
            x = random()
            for i in range(value):
                if offset[i] > x:
                    id = key + ' ' + str(dictionary[i])
                    packets_list.append(id)
                    break
    print('222222222222')

    shuffle(packets_list)

    total_packets_file=open(toponame+'_packets.txt','w')

    for packet in packets_list:

        if get_subnum(packet) <= 200:
            filelist[get_src(packet)].write(packet+'\n')
        else:
            ttt = get_subnum(packet) - 200
            aaa = pat[get_sd_pair(packet)+ ' ' + str(ttt)]
            bbb = flow_assign[aaa]
            filelist[bbb].write(packet+'\n')

        total_packets_file.write(packet)

        for node in path_list[get_sd_pair(packet)]:
            orifilelist[node].write(packet+'\n')

    pass