/*
 * spacesaving-3.cpp
 *
 *  Created on: 2019年11月20日
 *      Author: dengqi
 *      多点，分段函数负载均衡
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
#include <iostream>
#include "../SpaceSaving/SpaceSaving.h"

using namespace std;
#define START_FILE_NO 1
#define END_FILE_NO 10
#define E 2.71//自然对数的底数
bool original=true;
bool balanced=true;
bool ran=true;
bool subbalanced=true;

struct FIVE_TUPLE{	char key[13];	};
typedef vector<FIVE_TUPLE> TRACE;
TRACE traces[END_FILE_NO - START_FILE_NO + 1];
int numNode=23;
string topoName="Geant";
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


for(numNode=13;numNode<=23;numNode++){

	measureNode.clear();
	vector<vector<string> >traces_origin,traces_balanced,traces_random,traces_subbalanced;

	string dir="/home/dengqi/eclipse-workspace/ElasticSketchCode/data/multiswitch/packet/"+to_string(numNode)+"/";
	string outdir="/home/dengqi/eclipse-workspace/ElasticSketchCode/data/multiswitch/spacesaving/"+to_string(numNode)+"/";
	string measureNodeFile=dir+"measureNode.txt";
	ifstream ifs(measureNodeFile.c_str());
	int tmp;
	while(ifs>>tmp)
		measureNode.push_back(tmp);
	ifs.close();


	int test;//实验次数，对应RAND_SEED:1到10
	string tracedir;
	if(original)
	{
		tracedir=dir+"packets_original/";
		MyReadInTraces(tracedir,traces_origin);
	}
	if(balanced)
	{
		tracedir=dir+"packets_balanced/";
		MyReadInTraces(tracedir,traces_balanced);
	}
	if(ran)
	{
		tracedir=dir+"packets_random/";
		MyReadInTraces(tracedir,traces_random);
	}


//	for(test=1;test<=1;test++)
	for(test=3000;test<=3000;test+=1000)//这是为了跑所有节点容量设置为一样，这里test是cm的宽度
	{


	//	string nodecapfile="";
	//	ifstream ifs(nodecapfile.c_str());
		//读取测量节点的容量
		vector<int>cap;

		string capfile=dir+"packets_subbalanced/"+to_string(test)+"/capacity.txt";

		ifstream ifs(capfile.c_str());
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
		cout <<"*************"<<test<<"***************"<<endl;
#define HEAVY 1000
#define TOT_MEM_IN_BYTES (HEAVY*44)
		SpaceSaving<4> *ss = NULL;
		map<string,int>allReal,allEST;//记录所有节点的流的真实值和测量值

	//	int theta=atoi(argv[1]);
	//	cout<<theta<<endl;
/*********************************original*************************************************/

		if(original){
		allReal.clear();//original 的real不可信，因为他统计了一条流进过的所有节点上的包，重复统计
		allEST.clear();
		for(int node = 0; node < numNode; ++node)
		{
			int cmcounter_num=w[node]*4;
			unordered_map<string, int> Real_Freq;
			int packet_cnt = traces_origin[node].size();
			ss = new SpaceSaving<4>(TOT_MEM_IN_BYTES);
			//测量
			for(int i = 0; i < packet_cnt; ++i)
			{
				ss->insert((uint8_t*)traces_origin[node][i].c_str());
				int est=ss->query((uint8_t*)traces_origin[node][i].c_str());
				Real_Freq[traces_origin[node][i]]++;
				allReal[traces_origin[node][i]]++;

			}




			double ARE = 0;
			//统计
			for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
			{
				uint8_t key[13];
				memcpy(key, (it->first).c_str(), 13);

				int est_val=ss->query(key);
				if(allEST.find(it->first)!=allEST.end())
				{
					allEST[it->first]=min(est_val,allEST[it->first]);
				}
				else
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
			cout << to_string(node)+" original ARE:"<<ARE<<endl;

			delete ss;
			Real_Freq.clear();
		}
		string md="mkdir -p "+outdir+"est_original/";
		system(md.c_str());
		string outfile=outdir+"est_original/"+
				to_string(HEAVY)+"_"+to_string(test)+"_est.txt";
		OutPutTM(outfile,allEST);
		outfile=outdir+"est_original/real.txt";
		OutPutTM(outfile,allReal);
		double RRMSE=CalRRMSE(allReal,allEST);
		cout<<"original RRMSE="<<RRMSE<<endl;
//		cout<<"=================================================================="<<endl;
		}//endif original
/******************************************** balanced ***************************************/
		if(balanced){
		allReal.clear();//original 的real不可信，因为他统计了一条流进过的所有节点上的包，重复统计
		allEST.clear();
		for(int node = 0; node < numNode; ++node)
		{
			int cmcounter_num=w[node]*4;
			unordered_map<string, int> Real_Freq;
			int packet_cnt = traces_balanced[node].size();
			ss = new SpaceSaving<4>(TOT_MEM_IN_BYTES);
			//测量
			for(int i = 0; i < packet_cnt; ++i)
			{
				ss->insert((uint8_t*)traces_balanced[node][i].c_str());
				int est=ss->query((uint8_t*)traces_balanced[node][i].c_str());
				Real_Freq[traces_balanced[node][i]]++;
				allReal[traces_balanced[node][i]]++;

			}




			double ARE = 0;

			//统计
			for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
			{

				uint8_t key[13];
				memcpy(key, (it->first).c_str(), 13);

				int est_val=ss->query(key);
				if(allEST.find(it->first)!=allEST.end())
				{
					allEST[it->first]=min(est_val,allEST[it->first]);
				}
				else
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
			cout << to_string(node)+" balanced ARE:"<<ARE<<endl;

			delete ss;
			Real_Freq.clear();
		}
		string md="mkdir -p "+outdir+"est_balanced/";
		system(md.c_str());
		string outfile=outdir+"est_balanced/"+
				to_string(HEAVY)+"_"+to_string(test)+"_est.txt";
		OutPutTM(outfile,allEST);
		outfile=outdir+"est_balanced/real.txt";
		OutPutTM(outfile,allReal);
		double RRMSE=CalRRMSE(allReal,allEST);
		cout<<"balanced RRMSE="<<RRMSE<<endl;
		//		cout<<"=================================================================="<<endl;
		}//endif balanced

/****************************************random**********************************************/
		if(ran){
		allReal.clear();//original 的real不可信，因为他统计了一条流进过的所有节点上的包，重复统计
		allEST.clear();
		for(int node = 0; node < numNode; ++node)
		{
			int cmcounter_num=w[node]*4;
			unordered_map<string, int> Real_Freq;
			int packet_cnt = traces_random[node].size();
			ss = new SpaceSaving<4>(TOT_MEM_IN_BYTES);
			//测量
			for(int i = 0; i < packet_cnt; ++i)
			{
				ss->insert((uint8_t*)traces_random[node][i].c_str());
				int est=ss->query((uint8_t*)traces_random[node][i].c_str());
				Real_Freq[traces_random[node][i]]++;
				allReal[traces_random[node][i]]++;

			}




			double ARE = 0;

			//统计
			for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
			{

				uint8_t key[13];
				memcpy(key, (it->first).c_str(), 13);

				int est_val=ss->query(key);
				if(allEST.find(it->first)!=allEST.end())
				{
					allEST[it->first]=min(est_val,allEST[it->first]);
				}
				else
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
			cout << to_string(node)+" random ARE:"<<ARE<<endl;

			delete ss;
			Real_Freq.clear();
		}
		string md="mkdir -p "+outdir+"est_random/";
		system(md.c_str());
		string outfile=outdir+"est_random/"+
				to_string(HEAVY)+"_"+to_string(test)+"_est.txt";
		OutPutTM(outfile,allEST);
		outfile=outdir+"est_random/real.txt";
		OutPutTM(outfile,allReal);
		double RRMSE=CalRRMSE(allReal,allEST);
		cout<<"random RRMSE="<<RRMSE<<endl;
		//		cout<<"=================================================================="<<endl;
		}//endif random
/***************************************subbalanced*****************************************/
		if(subbalanced){
		allReal.clear();
		allEST.clear();

		tracedir=dir+"packets_subbalanced/"+to_string(test)+"/";
		MyReadInTraces(tracedir,traces_subbalanced);
		for(int node = 0; node < numNode; ++node)
		{
			int cmcounter_num=w[node]*4;
			unordered_map<string, int> Real_Freq;
			int packet_cnt = traces_subbalanced[node].size();
			ss = new SpaceSaving<4>(TOT_MEM_IN_BYTES);
			//测量
			for(int i = 0; i < packet_cnt; ++i)
			{
				ss->insert((uint8_t*)traces_subbalanced[node][i].c_str());
				int est=ss->query((uint8_t*)traces_subbalanced[node][i].c_str());
				Real_Freq[traces_subbalanced[node][i]]++;
				allReal[traces_subbalanced[node][i]]++;

			}




			double ARE = 0;

			//统计
			for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
			{

				uint8_t key[13];
				memcpy(key, (it->first).c_str(), 13);

				int est_val=ss->query(key);
				if(allEST.find(it->first)!=allEST.end())
				{
					allEST[it->first]=min(est_val,allEST[it->first]);
				}
				else
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
			cout << to_string(node)+" subbalanced ARE:"<<ARE<<endl;

			delete ss;
			Real_Freq.clear();
		}
		string md="mkdir -p "+outdir+"est_subbalanced/";
		system(md.c_str());
		string outfile=outdir+"est_subbalanced/"+
				to_string(HEAVY)+"_"+to_string(test)+"_est.txt";
		OutPutTM(outfile,allEST);
		outfile=outdir+"est_subbalanced/real.txt";
		OutPutTM(outfile,allReal);
		double RRMSE=CalRRMSE(allReal,allEST);
		cout<<"subbalanced RRMSE="<<RRMSE<<endl;
		//		cout<<"=================================================================="<<endl;
		}//endif subbalanced

	}//end for(test=1;test<=1;test++)
}//end for(numNode=13;numNode<=22;numNode++)
}//end main





