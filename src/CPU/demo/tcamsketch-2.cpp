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
void ReadInTraces(const char *trace_prefix)
{
	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		char datafileName[100];
		sprintf(datafileName, "%s%d.dat", trace_prefix, datafileCnt - 1);
		FILE *fin = fopen(datafileName, "rb");

		FIVE_TUPLE tmp_five_tuple;
		traces[datafileCnt - 1].clear();
		while(fread(&tmp_five_tuple, 1, 13, fin) == 13)
		{
			traces[datafileCnt - 1].push_back(tmp_five_tuple);
		}
		fclose(fin);

		printf("Successfully read in %s, %ld packets\n", datafileName, traces[datafileCnt - 1].size());
	}
	printf("\n");
}

void MyReadInTraces(string traceDir,vector<vector<string> >&traces_origin,vector<vector<string> >&traces_balanced)
{

	for(int node=0;node<numNode;node++)
	{
		vector<string>packets;
		string filename=traceDir+topoName+"_"+to_string(node)+"_origional_packets.txt";
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

}
bool cmp(const pair<string,uint32_t>&a,const pair<string,uint32_t>&b)
{
	return a.second>b.second;
}
int main()
{
//	ReadInTraces("../../../data/");
	vector<vector<string> >traces_origin,traces_balanced;
	string dir="/home/dengqi/eclipse-workspace/ElasticSketchCode/data/最大流匹配/";
	MyReadInTraces(dir,traces_origin,traces_balanced);

#define HEAVY_MEM (150*1024)
#define BUCKET_NUM (HEAVY_MEM / 64)
#define TOT_MEM_IN_BYTES (600 * 1024)
		constexpr int k=1000;
		constexpr int tot_men_in_byte=k*1024;
		constexpr int bucket_num=tot_men_in_byte/4/64;
		TCAMSketch *tcamsketch = NULL;



		for(int node = 0; node < numNode; ++node)
		{
			unordered_map<string, int> Real_Freq;
			int packet_cnt = traces_origin[node].size();
			tcamsketch = new TCAMSketch(packet_cnt/10000);

			for(int i = 0; i < packet_cnt; ++i)
			{
				tcamsketch->insert((uint8_t*)traces_origin[node][i].c_str());
				int est=tcamsketch->query((uint8_t*)traces_origin[node][i].c_str());
				Real_Freq[traces_origin[node][i]]++;
			}
			string filename=dir+"tcam/"+topoName+"_"+to_string(node)+"_original_measure.txt";
			ofstream ofs(filename);
			double ARE = 0;
			for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
			{
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
			cout << to_string(node)+" original ARE:"<<ARE<<endl;
			ofs.close();
			delete tcamsketch;
			Real_Freq.clear();
		}
	/************************************************************/
		for(int node = 0; node < numNode; ++node)
		{
			unordered_map<string, int> Real_Freq;
			int packet_cnt = traces_origin[node].size();
			tcamsketch = new TCAMSketch(packet_cnt/10000);

			for(int i = 0; i < packet_cnt; ++i)
			{
				tcamsketch->insert((uint8_t*)traces_origin[node][i].c_str());
				Real_Freq[traces_origin[node][i]]++;
			}
			string filename=dir+"tcam/"+topoName+"_"+to_string(node)+"_balanced_measure.txt";
			ofstream ofs(filename);
			double ARE = 0;
			for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
			{
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
			cout << to_string(node)+" balanced ARE:"<<ARE<<endl;
			ofs.close();
			delete tcamsketch;
			Real_Freq.clear();
		}

}



