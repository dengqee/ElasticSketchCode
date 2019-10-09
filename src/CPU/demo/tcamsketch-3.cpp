/*
 * tcamsketch-3.cpp
 *
 *  Created on: 2019年9月16日
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
#include "../TCAMSketch/TCAMSketch.h"
using namespace std;
#define START_FILE_NO 1
#define END_FILE_NO 10
#define E 2.71//自然对数的底数
bool original=false;
bool balanced=false;
bool ran=true;

struct FIVE_TUPLE{	char key[13];	};
typedef vector<FIVE_TUPLE> TRACE;
TRACE traces[END_FILE_NO - START_FILE_NO + 1];
int numNode=23;
string topoName="Geant";


void MyReadInTraces(string traceDir,vector<vector<string> >&traces_origin,vector<vector<string> >&traces_balanced,vector<vector<string> >&traces_random)
{

	if(original){
	for(int node=0;node<numNode;node++)
	{
		vector<string>packets;
		string filename=traceDir+topoName+"_"+to_string(node)+"_origional_packets.txt";
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
		traces_origin.push_back(packets);
		ifs.close();
	}
	}//endif original
	if(balanced){
	for(int node=0;node<numNode;node++)
	{
		vector<string>packets;
		string filename=traceDir+"packets_subbalanced/subbalanced_"+to_string(node)+"_packets.txt";
		ifstream ifs(filename.c_str());
		string line;
		istringstream lineBuffer;
		uint32_t s,d;
		uint32_t num;
		uint8_t key[5];
		while(getline(ifs,line))
		{
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
		traces_balanced.push_back(packets);
		ifs.close();
	}
	}//endif balanced

	if(ran){
	for(int node=0;node<numNode;node++)
	{
		vector<string>packets;
		string filename=traceDir+topoName+"_"+to_string(node)+"_random_packets.txt";
		ifstream ifs(filename.c_str());
		string line;
		istringstream lineBuffer;
		uint32_t s,d;
		uint32_t num;
		uint8_t key[5];
		while(getline(ifs,line))
		{
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
		traces_random.push_back(packets);
		ifs.close();
	}
	}//endif random

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
	vector<vector<string> >traces_origin,traces_balanced,traces_random;
	string dir="/home/dengqi/eclipse-workspace/ElasticSketchCode/data/最大流匹配/";
	MyReadInTraces(dir,traces_origin,traces_balanced,traces_random);

//	string nodecapfile="";
//	ifstream ifs(nodecapfile.c_str());
	//读取测量节点的容量
	vector<int>cap;
	ifstream ifs("/home/dengqi/project5/Thesis/multiswitch/cplex/problem3/Nmax_phy_load_x.txt");
	stringstream linebuf;
	string line;
	getline(ifs,line);
	linebuf.str(line);
	int Nmax;
	while(linebuf>>Nmax)
		cap.push_back(Nmax);
	ifs.close();
	int measureNodeNum=cap.size();
	vector<int>w(measureNodeNum);
	float e_percent=0.1;//10%的误差作为容量
	for(int i=0;i<cap.size();i++)
	{
		w[i]=ceil(cap[i]/(1+e_percent));
	}

//	int theta=atoi(argv[1]);
//	cout<<theta<<endl;
	int theta=20;
	int tcamLimit=250;
	int cmcounter_num=12000;//the TOTAL number of cmsketch counters
	for(tcamLimit=1000;tcamLimit<=1000;tcamLimit+=250)
		for(cmcounter_num=0;cmcounter_num<=0;cmcounter_num+=4000)
	{
		cout<<"************"<<tcamLimit<<"_"<<cmcounter_num<<"****************"<<endl;
		TCAMSketch *tcamsketch = NULL;

		string outdir=dir+"packets_subbalanced/tcam_4/"+to_string(tcamLimit);
		string md="mkdir "+outdir;
		system(md.c_str());
		map<string,int>allReal,allEST;//记录所有节点的流的真实值和测量值
		if(original){
		for(int node = 0; node < numNode; ++node)
		{
			cmcounter_num=w[node]*4;
			unordered_map<string, int> Real_Freq;
			int packet_cnt = traces_origin[node].size();
			tcamsketch = new TCAMSketch(theta,tcamLimit,cmcounter_num);

			for(int i = 0; i < packet_cnt; ++i)
			{
				tcamsketch->insert((uint8_t*)traces_origin[node][i].c_str());
				int est=tcamsketch->query((uint8_t*)traces_origin[node][i].c_str());
				Real_Freq[traces_origin[node][i]]++;
				allReal[traces_origin[node][i]]++;

			}

			string filename=outdir+"/"+topoName+"_"+to_string(node)+"_original_measure.txt";
//			string filename=dir+"tcam/"+tmp+"/"+topoName+"_measure.txt";

			ofstream ofs(filename);
			double ARE = 0;
			map<string,uint32_t>realheavymap;
			for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
			{
				if(it->second>=theta)
					realheavymap[it->first]=it->second;
				uint8_t key[13];
				memcpy(key, (it->first).c_str(), 13);

				int est_val=tcamsketch->query(key);
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
				ofs<<s<<" "<<d<<" "<<num<<" "<<est_val<<endl;

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
			}
			double prec=1.0*numDet/tcam.size();

			cout << to_string(node)+" original ARE:"<<ARE<<" precision:"<<prec<<endl;
//			tcamsketch->print();
//			cout<<"flow num:"<<Real_Freq.size()<<endl;
			ofs.close();
			delete tcamsketch;
			Real_Freq.clear();
		}
		double RRMSE=CalRRMSE(allReal,allEST);
		cout<<"original RRMSE="<<RRMSE<<endl;
		cout<<"=================================================================="<<endl;
		}//endif original
/******************************************** balanced ***************************************/
		if(balanced){
		allReal.clear();
		allEST.clear();
		for(int node = 0; node < numNode; ++node)
		{
			cmcounter_num=w[node]*4;
			unordered_map<string, int> Real_Freq;
			int packet_cnt = traces_balanced[node].size();
			tcamsketch = new TCAMSketch(theta,tcamLimit,cmcounter_num);

			for(int i = 0; i < packet_cnt; ++i)
			{
				tcamsketch->insert((uint8_t*)traces_balanced[node][i].c_str());
				Real_Freq[traces_balanced[node][i]]++;
				allReal[traces_balanced[node][i]]++;
			}

			string filename=outdir+"/"+topoName+"_"+to_string(node)+"_subbalanced_measure.txt";
			ofstream ofs(filename);
			double ARE = 0;
			map<string,uint32_t>realheavymap;
			for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
			{
				if(it->second>=theta)
					realheavymap[it->first]=it->second;
				uint8_t key[13];
				memcpy(key, (it->first).c_str(), 13);

				int est_val=tcamsketch->query(key);
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
				ofs<<s<<" "<<d<<" "<<num<<" "<<est_val<<endl;
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
			}
			double prec=1.0*numDet/tcam.size();


			cout << to_string(node)+" balanced ARE:"<<ARE<<" precision:"<<prec<<endl;
//			tcamsketch->print();
//			cout<<"flow num:"<<Real_Freq.size()<<endl;
			ofs.close();
			delete tcamsketch;
			Real_Freq.clear();
		}
		double RRMSE=CalRRMSE(allReal,allEST);
		cout<<"balanced RRMSE="<<RRMSE<<endl;
		cout<<"========================================================"<<endl;
		}//endif balanced

/****************************************random**********************************************/
		if(ran){
		allReal.clear();
		allEST.clear();
		for(int node = 0; node < numNode; ++node)
		{
			cmcounter_num=w[node]*4;
			unordered_map<string, int> Real_Freq;
			int packet_cnt = traces_random[node].size();
			tcamsketch = new TCAMSketch(theta,tcamLimit,cmcounter_num);

			for(int i = 0; i < packet_cnt; ++i)
			{
				tcamsketch->insert((uint8_t*)traces_random[node][i].c_str());
				Real_Freq[traces_random[node][i]]++;
				allReal[traces_random[node][i]]++;
			}

			string filename=outdir+"/"+topoName+"_"+to_string(node)+"_random_measure.txt";
	//			string filename=dir+"tcam/"+tmp+"/"+topoName+"_measure.txt";

			ofstream ofs(filename);
			double ARE = 0;
			map<string,uint32_t>realheavymap;
			for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
			{
				if(it->second>=theta)
					realheavymap[it->first]=it->second;
				uint8_t key[13];
				memcpy(key, (it->first).c_str(), 13);

				int est_val=tcamsketch->query(key);
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
				ofs<<s<<" "<<d<<" "<<num<<" "<<est_val<<endl;

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
			}
			double prec=1.0*numDet/tcam.size();

			cout << to_string(node)+" random ARE:"<<ARE<<" precision:"<<prec<<endl;
	//			tcamsketch->print();
	//			cout<<"flow num:"<<Real_Freq.size()<<endl;
			ofs.close();
			delete tcamsketch;
			Real_Freq.clear();
		}
		double RRMSE=CalRRMSE(allReal,allEST);
		cout<<"random RRMSE="<<RRMSE<<endl;
		cout<<"================================================"<<endl;
		}//endif random


}
}



