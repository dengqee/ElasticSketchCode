/*

 *
 *  Created on: 2019年9月16日
 *      Author: dengqi
 *      多时段，多点，分段函数负载均衡
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <set>
#include "../TCAMSketch/TCAMSketch.h"
using namespace std;
#define START_FILE_NO 1
#define END_FILE_NO 10
#define E 2.71//自然对数的底数
bool subbalanced=true;

struct FIVE_TUPLE{	char key[13];	};
typedef vector<FIVE_TUPLE> TRACE;
TRACE traces[END_FILE_NO - START_FILE_NO + 1];
int numNode=40;
string topoName="BA50";
vector<int>measureNode;

void MyReadInTraces(string traceDir,vector<vector<string> >&traces)
{
	traces.clear();
	for(int node:measureNode)
	{
		vector<string>packets;
		string filename=traceDir+to_string(node)+".txt";
//		string filename=traceDir+topoName+"_"+"packets.txt";
		ifstream ifs(filename.c_str());
		string line;
		istringstream lineBuffer;
		uint32_t s,d;
		uint32_t num;
		uint8_t key[5];
		//把string line的3个字符串压缩到32位中，作为一个ID
		while(getline(ifs,line))
		{
			lineBuffer.clear();
			lineBuffer.str (line);
			lineBuffer>>s>>d>>num;
			key[0]=s;
			key[1]=d;
			key[2]=num>>8;
			key[3]=0xff&num;
			string packet;
			for(int i=0;i<4;i++)
				packet.push_back(key[i]);
			packets.push_back(packet);

		}
		traces.push_back(packets);
		ifs.close();
	}


}
void OutPutTM(string outfile,const map<string,int>&allTM)//输出测量/真实的流量矩阵
{

	ofstream ofs(outfile.c_str());
	for(auto it=allTM.begin();it!=allTM.end();it++)
	{
		uint8_t key[13];
		memcpy(key, (it->first).c_str(), 13);
		//解码流ID
		uint32_t s,d,num;//source,dest,number
		s=(uint32_t)(key[0]);
		d=(uint32_t)(key[1]);
		num=(uint32_t)((key[2]<<8)|key[3]);
		ofs<<s*100000+d*1000+num<<" "<<it->second<<endl;
	}
	ofs.close();
}
bool cmp(const pair<string,uint32_t>&a,const pair<string,uint32_t>&b)
{
	return a.second>b.second;
}
double CalRRMSE(map<string,int>&real,map<string,int>&est)
{
	double ret;
	double sum1=0,sum2=0;
	for(auto it=est.begin();it!=est.end();it++)
	{
		sum1+=(it->second-real[it->first])*(it->second-real[it->first]);
		sum2+=real[it->first]*real[it->first];
	}
	ret=sqrt(sum1/sum2);
	return ret;
}
int main(int argc,char* argv[])
{
//	if(argc<2)
//	{
//		cout<<"./tcamsketch-2.cpp theta"<<endl;
//		exit(1);
//	}
//	ReadInTraces("../../../data/");


	int tcamLimit=1000;
	int test;//实验次数，对应RAND_SEED:1到10
	for(tcamLimit=1000;tcamLimit<=1000;tcamLimit+=500)
	for(test=1;test<=1;test++)
//	for(test=3000;test<=3000;test+=1000)//这是为了跑所有节点容量设置为一样，这里test是cm的宽度
	{
for(numNode=40;numNode<=40;numNode++){
	TCAMSketch** tcamsketch_node=new TCAMSketch*[numNode];
	TCAMSketch *tcamsketch = NULL;
	map<string,int>allReal,allEST;//记录所有节点的流的真实值和测量值
	map<string,int>allTCAM;//记录所有节点中TCAM流和所在节点号
for(int day=1;day<=10;day++){
	uint64_t psum=0;//当天数据包总数

	measureNode.clear();
	vector<vector<string> >traces_subbalanced;
	string dir="/home/dengqi/project5/Thesis/multiswitch/data/"+topoName+"_days/"+to_string(numNode)+"/day"+to_string(day)+"/";

	string outdir="/home/dengqi/project5/Thesis/multiswitch/data/"+topoName+"_days/"+to_string(numNode)+"/est/"+"day"+to_string(day)+"/";
	string measureNodeFile="/home/dengqi/eclipse-workspace/ElasticSketchCode/data/multiswitch/packet/"+to_string(numNode)+"/measureNode.txt";
	ifstream ifs(measureNodeFile.c_str());
	int tmp;
	while(ifs>>tmp)
		measureNode.push_back(tmp);
	ifs.close();
	int theta=20;
	//int theta=80;
	string tracedir;


	//	string nodecapfile="";
	//	ifstream ifs(nodecapfile.c_str());
		//读取测量节点的容量
		vector<int>cap;

		string capfile=dir+"capacity.txt";

		ifs.open(capfile.c_str());
		stringstream linebuf;
		string line;
		getline(ifs,line);
		linebuf.clear();
		linebuf.str(line);
		int Nmax;
		while(linebuf>>Nmax)
			cap.push_back(Nmax);
		ifs.close();
		int measureNodeNum=cap.size();
		vector<int>w(measureNodeNum);//w为每个测量节点上sketch的宽度
		float e_percent=0.1;//10%的误差作为容量
		for(int i=0;i<cap.size();i++)
		{
			w[i]=ceil(cap[i]/(1+e_percent));
		}
		cout<<"************"<<tcamLimit<<"_"<<test<<"****************"<<endl;

	//	int theta=atoi(argv[1]);
	//	cout<<theta<<endl;

/***************************************subbalanced*****************************************/
		if(subbalanced){
		//allReal.clear();
		//allEST.clear();
		//allTCAM.clear();
		tracedir=dir;
//		tracedir=dir+"packets_subbalanced/"+to_string(test)+"/";//这是为了跑所有节点容量设置为一样，这里test是cm的宽度
		MyReadInTraces(tracedir,traces_subbalanced);
		for(int node = 0; node < numNode; ++node)
		{
			int cmcounter_num=w[node]*4;
			map<string, int> Real_Freq;
			int packet_cnt = traces_subbalanced[node].size();
			psum+=packet_cnt;
			if(day==1)
			{
				tcamsketch = new TCAMSketch(theta,tcamLimit,cmcounter_num);
				tcamsketch_node[node]=tcamsketch;
			}
			else
			{
				tcamsketch=tcamsketch_node[node];
			}
			//测量
			for(int i = 0; i < packet_cnt; ++i)
			{
				tcamsketch->insert((uint8_t*)traces_subbalanced[node][i].c_str());
				int est=tcamsketch->query((uint8_t*)traces_subbalanced[node][i].c_str());
				Real_Freq[traces_subbalanced[node][i]]++;
				allReal[traces_subbalanced[node][i]]++;

			}


			double ARE = 0;
			map<string,uint32_t>realheavymap;
			//统计
			for(map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
			{
				if(it->second>=theta)
					realheavymap[it->first]=it->second;
				uint8_t key[13];
				memcpy(key, (it->first).c_str(), 13);

				int est_val=tcamsketch->query(key);
				allEST[it->first]=est_val;
				//解码流ID
				uint32_t s,d,num;//source,dest,number
				s=(uint32_t)(key[0]);
				d=(uint32_t)(key[1]);
				num=(uint32_t)((key[2]<<8)|key[3]);


				int dist = std::abs(it->second - est_val);
				ARE += dist * 1.0 / (it->second);
			}
			ARE /= (int)Real_Freq.size();

			map<string,uint32_t>tcam=tcamsketch->GetTCAM();

			int numDet=0;
			for(auto it=tcam.begin();it!=tcam.end();++it)
			{
				auto tmp=realheavymap.find(it->first);
				if(tmp!=realheavymap.end())
					numDet++;
				allTCAM.insert(make_pair(it->first,measureNode[node]));
			}
			double prec=1.0*numDet/tcam.size();

			cout << to_string(node)+" subbalanced ARE:"<<ARE<<" precision:"<<prec<<endl;
//			tcamsketch->print();
		//	cout<<"flow num:"<<Real_Freq.size()<<endl;

			//delete tcamsketch;
			//输出单个节点的真实流量
			string outfile=dir+to_string(measureNode[node])+"_real.txt";
			OutPutTM(outfile,Real_Freq);

			Real_Freq.clear();
		}
		string md="mkdir -p "+outdir;
		system(md.c_str());
		string outfile=outdir+
				to_string(tcamLimit)+"_"+to_string(test)+"_est.txt";
		OutPutTM(outfile,allEST);
		outfile=outdir+"real.txt";
		OutPutTM(outfile,allReal);
		outfile=outdir+to_string(tcamLimit)+"_TCAM.txt";
		OutPutTM(outfile,allTCAM);
		double RRMSE=CalRRMSE(allReal,allEST);
		cout<<"subbalanced RRMSE="<<RRMSE<<endl;
		cout<<"day"<<to_string(day)<<"数据包总数:"<<psum<<endl;
//		cout<<"=================================================================="<<endl;
		}//endif subbalanced

	}//end for(int day=1;day<=10;day++)
	for(int i=0;i<numNode;i++)
		delete tcamsketch_node[i];
	delete[] tcamsketch_node;
	}//end for(numNode=13;numNode<=22;numNode++)
	}//end for(test=1;test<=1;test++)
}//end main


