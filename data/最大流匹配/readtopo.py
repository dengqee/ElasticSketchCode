from random import randint
from math import floor
import array

def readtopo(toponame="Geant"):
    with open("Topology_" + toponame + ".txt", 'r') as tp:
        i = 0
        for line in tp.readlines():
            if i == 0:
                i = 1
                nodes = int(line.strip().split()[0])
                links = [[] for i in range(nodes)]
            else:
                s = [int(i) for i in line.strip().split()]
                links[s[0]].append(s[1])
                links[s[1]].append(s[0])
        return nodes, links


def dijkstara(nodes, links, nodenum=0):
    value = [[i, 100] for i in range(nodes)]
    value[nodenum] = [nodenum, 0]
    flag = [0 if i == nodenum else 1 for i in range(nodes)]
    path = [[nodenum] for i in range(nodes)]
    while sum(flag) != 0:
        value.sort(key=lambda x: x[1])
        nv = value.pop(0)
        node = nv[0]
        length = nv[1]
        flag[node] = 0
        for i in links[node]:
            for j in value:
                if j[0] == i:
                    if j[1] > length + 1:
                        j[1] = length + 1
                        path[j[0]] = path[node] + [j[0]]
    return path




def max_flow(nodes, paths, value):
    topo = make_bigraph_topo(nodes, paths, value)
    flag = 0
    while flag == 0:
        print(value)
        bi_paths = []
        itte=1
        for i in range(len(paths)):
            # if i%100==0:
            #     print(i)
            bi_path = [0] + dfs(topo, 0, [0 for j in range(len(topo))], []) + [len(topo) - 1]
            if len(bi_path) == 2:
                itte=itte+1
                value =floor(value * 1.2)+1
                topo = make_bigraph_topo(nodes, paths, value)
                break
            else:
                bi_paths.append(bi_path)
                for j in range(0, len(bi_path) - 1):
                    if bi_path[j + 1] not in topo[bi_path[j]].keys():
                        topo[bi_path[j]][bi_path[j + 1]]=0
                    topo[bi_path[j]][bi_path[j + 1]] = topo[bi_path[j]][bi_path[j + 1]] - 1
                    if bi_path[j] not in topo[bi_path[j + 1]].keys():
                        topo[bi_path[j + 1]][bi_path[j]]=0
                    topo[bi_path[j + 1]][bi_path[j]] = 1 + topo[bi_path[j + 1]][bi_path[j]]
        if len(bi_paths) == len(paths):
            flag = 1
    flows = [[] for i in range(nodes)]
    for i in range(1,len(paths)+1):
        for key,value in topo[i].items():
            if value==0:
                flows[key-len(paths)-1].append(i-1)
    return flows,itte


def make_bigraph_topo(nodes, paths, value):
    nodenum = nodes + len(paths) + 2
    linkdict={}
    for i in range(nodenum):
        linkdict[i]={}
    for i in range(len(paths)):
        linkdict[0][i+1]=1
    for i, path in enumerate(paths):
        for j in path:
            linkdict[i+1][j + len(paths) + 1]=1
    for i in range(nodes):
        linkdict[i + len(paths) + 1][len(paths)+nodes+1]=value
    return linkdict


def dfs(topo, s, flag, path=[]):
    if len(topo) - 1 in topo[s].keys():
        if topo[s][len(topo) - 1]>0:
            return path
    for i in topo[s].keys():
        if topo[s][i]>0 and flag[i] == 0:
            flag[i] = 1
            p=dfs(topo, i, flag, path + [i])
            if len(p)!=0:
                return  p
    return ([])

def generatePathNode(toponame='Geant'):
    path_list={}
    nodes, links = readtopo(toponame=toponame)
    paths =[]
    ranlow = 1
    for ranmax in range(100,101):
        flow_count={}
        pat={}
        with open(toponame+'flownum' + str(ranlow) + '_' + str(ranmax) + '.txt', 'w') as f:
            for i in range(nodes):
                t = dijkstara(nodes, links, i)
                t.pop(i)
                for path in t:
                    path_list[str(path[0])+' '+str(path[-1])]=path
                for path in t:
                    xxx=randint(ranlow,ranmax)
                    f.write(str(path[0])+' '+str(path[-1])+'\n')
                    f.write(str(xxx)+'\n')
                    flow_count[str(path[0])+' '+str(path[-1])]=xxx+200
                    for r in range(xxx):
                        pat[str(path[0])+' '+str(path[-1])+' '+str(r)]=len(paths)
                        paths.append(path)
        flow_node,itte = max_flow(nodes, paths, floor(len(paths)/nodes)+1)
        with open(toponame+'itte' + str(ranlow) + '_' + str(ranmax) + '.txt', 'w') as f:
            f.write(str(itte))
        with open(toponame+'flow_node'+str(ranlow)+'_'+str(ranmax)+'.txt','w') as f:
            for i in range(nodes):
                for j in flow_node[i]:
                    f.write(str(i)+'\n')
                    for k in paths[j]:
                        f.write(str(k)+' ')
                    f.write('\n')
        print('itte='+str(itte))
        return flow_node,flow_count,paths,pat,path_list

if __name__ == "__main__":
    generatePathNode()