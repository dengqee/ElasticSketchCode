/*
 * tcamsketch-2.cpp
 *
 *  Created on: 2019年5月30日
 *      Author: dengqi
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
#include "../TCAMSketch/TCAMSketch.h"
using namespace std;
#define START_FILE_NO 1
#define END_FILE_NO 10


struct FIVE_TUPLE{	char key[13];	};
typedef vector<FIVE_TUPLE> TRACE;
TRACE traces[END_FILE_NO - START_FILE_NO + 1];
int numNode=23;
string topoName="Geant";


void MyReadInTraces(string traceDir,vector<vector<string> >&traces_origin,vector<vector<string> >&traces_balanced,vector<vector<string> >&traces_random)
{

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
	for(int node=0;node<numNode;node++)
	{
		vector<string>packets;
		string filename=traceDir+topoName+"_"+to_string(node)+"_balanced_packets.txt";
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

}
bool cmp(const pair<string,uint32_t>&a,const pair<string,uint32_t>&b)
{
	return a.second>b.second;
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



//	int theta=atoi(argv[1]);
//	cout<<theta<<endl;
	int theta=20;
	int tcamLimit=250;
	int cmcounter_num=12000;//the TOTAL number of cmsketch counters
	for(tcamLimit=1000;tcamLimit<=1000;tcamLimit+=250)
		for(cmcounter_num=4000;cmcounter_num<=48000;cmcounter_num+=4000)
	{
		cout<<"****************************"<<endl;
		TCAMSketch *tcamsketch = NULL;

		string outdir=dir+"tcam/"+to_string(tcamLimit)+"_"+to_string(cmcounter_num);
		string md="mkdir "+outdir;
		system(md.c_str());

//		for(int node = 0; node < numNode; ++node)
//		{
//			unordered_map<string, int> Real_Freq;
//			int packet_cnt = traces_origin[node].size();
//			tcamsketch = new TCAMSketch(theta,tcamLimit,cmcounter_num);
//
//			for(int i = 0; i < packet_cnt; ++i)
//			{
//				tcamsketch->insert((uint8_t*)traces_origin[node][i].c_str());
//				int est=tcamsketch->query((uint8_t*)traces_origin[node][i].c_str());
//				Real_Freq[traces_origin[node][i]]++;
//			}
//
//			string filename=outdir+"/"+topoName+"_"+to_string(node)+"_original_measure.txt";
////			string filename=dir+"tcam/"+tmp+"/"+topoName+"_measure.txt";
//
//			ofstream ofs(filename);
//			double ARE = 0;
//			map<string,uint32_t>realheavymap;
//			for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
//			{
//				if(it->second>=theta)
//					realheavymap[it->first]=it->second;
//				uint8_t key[13];
//				memcpy(key, (it->first).c_str(), 13);
//
//				int est_val=tcamsketch->query(key);
//
//				//解码流ID
//				uint32_t s,d,num;//source,dest,number
//				s=(uint32_t)(key[0]);
//				d=(uint32_t)(key[1]);
//				num=(uint32_t)((key[2]<<8)|key[3]);
//				ofs<<s<<" "<<d<<" "<<num<<" "<<est_val<<endl;
//
//				int dist = std::abs(it->second - est_val);
//				ARE += dist * 1.0 / (it->second);
//			}
//			ARE /= (int)Real_Freq.size();
//
//			map<string,uint32_t>tcam=tcamsketch->GetTCAM();
//
//			int numDet=0;
//			for(auto it=tcam.begin();it!=tcam.end();++it)
//			{
//				auto tmp=realheavymap.find(it->first);
//				if(tmp!=realheavymap.end())
//					numDet++;
//			}
//			double prec=1.0*numDet/tcam.size();
//
//			cout << to_string(node)+" original ARE:"<<ARE<<" precision:"<<prec<<endl;
////			tcamsketch->print();
////			cout<<"flow num:"<<Real_Freq.size()<<endl;
//			ofs.close();
//			delete tcamsketch;
//			Real_Freq.clear();
//		}
//		cout<<"========="<<endl;
//	/********************* balanced ***************************************/
//		for(int node = 0; node < numNode; ++node)
//		{
//			unordered_map<string, int> Real_Freq;
//			int packet_cnt = traces_balanced[node].size();
//			tcamsketch = new TCAMSketch(theta,tcamLimit,cmcounter_num);
//
//			for(int i = 0; i < packet_cnt; ++i)
//			{
//				tcamsketch->insert((uint8_t*)traces_balanced[node][i].c_str());
//				Real_Freq[traces_balanced[node][i]]++;
//			}
//
//			string filename=outdir+"/"+topoName+"_"+to_string(node)+"_balanced_measure.txt";
//			ofstream ofs(filename);
//			double ARE = 0;
//			map<string,uint32_t>realheavymap;
//			for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
//			{
//				if(it->second>=theta)
//					realheavymap[it->first]=it->second;
//				uint8_t key[13];
//				memcpy(key, (it->first).c_str(), 13);
//
//				int est_val=tcamsketch->query(key);
//				//解码流ID
//				uint32_t s,d,num;//source,dest,number
//				s=(uint32_t)(key[0]);
//				d=(uint32_t)(key[1]);
//				num=(uint32_t)((key[2]<<8)|key[3]);
//				ofs<<s<<" "<<d<<" "<<num<<" "<<est_val<<endl;
//				int dist = std::abs(it->second - est_val);
//				ARE += dist * 1.0 / (it->second);
//			}
//			ARE /= (int)Real_Freq.size();
//
//			map<string,uint32_t>tcam=tcamsketch->GetTCAM();
//
//			int numDet=0;
//			for(auto it=tcam.begin();it!=tcam.end();++it)
//			{
//				auto tmp=realheavymap.find(it->first);
//				if(tmp!=realheavymap.end())
//					numDet++;
//			}
//			double prec=1.0*numDet/tcam.size();
//
//
//			cout << to_string(node)+" balanced ARE:"<<ARE<<" precision:"<<prec<<endl;
//			tcamsketch->print();
//			cout<<"flow num:"<<Real_Freq.size()<<endl;
//			ofs.close();
//			delete tcamsketch;
//			Real_Freq.clear();
//		}
//		cout<<"========="<<endl;

		/************random*****************/
		for(int node = 0; node < numNode; ++node)
		{
			unordered_map<string, int> Real_Freq;
			int packet_cnt = traces_random[node].size();
			tcamsketch = new TCAMSketch(theta,tcamLimit,cmcounter_num);

			for(int i = 0; i < packet_cnt; ++i)
			{
				tcamsketch->insert((uint8_t*)traces_random[node][i].c_str());
				int est=tcamsketch->query((uint8_t*)traces_random[node][i].c_str());
				Real_Freq[traces_random[node][i]]++;
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

}
}



