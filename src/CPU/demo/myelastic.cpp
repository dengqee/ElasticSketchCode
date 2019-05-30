/*
 * myelastic.cpp
 *
 *  Created on: 2019年5月23日
 *      Author: dengqi
 */
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include "../elastic/ElasticSketch.h"
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
		//压缩string到4个字节中
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
		ElasticSketch<bucket_num, tot_men_in_byte> *elastic = NULL;



		for(int node = 0; node < numNode; ++node)
		{
			unordered_map<string, int> Real_Freq;
			elastic = new ElasticSketch<bucket_num, tot_men_in_byte>();
			int packet_cnt = traces_origin[node].size();
			for(int i = 0; i < packet_cnt; ++i)
			{
				elastic->insert((uint8_t*)traces_origin[node][i].c_str());
				Real_Freq[traces_origin[node][i]]++;
			}
			string filename=dir+"elastic/"+topoName+"_"+to_string(node)+"_original_measure.txt";
			ofstream ofs(filename);
			double ARE = 0;
			for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
			{
				uint8_t key[13];
				memcpy(key, (it->first).c_str(), 13);
				bool flag;
				int est_val=elastic->query(key,flag);

				//解码流ID
				uint32_t s,d,num;//source,dest,number
				s=(uint32_t)(key[0]);
				d=(uint32_t)(key[1]);
				num=(uint32_t)((key[2]<<8)|key[3]);
				ofs<<s<<" "<<d<<" "<<num<<" "<<est_val<<" "<<flag<<endl;

				int dist = std::abs(it->second - est_val);
				ARE += dist * 1.0 / (it->second);
			}
			ARE /= (int)Real_Freq.size();
			cout << to_string(node)+" original ARE:"<<ARE<<endl;
			ofs.close();
			delete elastic;
			Real_Freq.clear();
		}
	/************************* balanced ***********************************/
		for(int node = 0; node < numNode; ++node)
		{
			unordered_map<string, int> Real_Freq;
			elastic = new ElasticSketch<bucket_num, tot_men_in_byte>();
			int packet_cnt = traces_balanced[node].size();
			for(int i = 0; i < packet_cnt; ++i)
			{
				elastic->insert((uint8_t*)traces_balanced[node][i].c_str());
				Real_Freq[traces_balanced[node][i]]++;
			}
			string filename=dir+"elastic/"+topoName+"_"+to_string(node)+"_balanced_measure.txt";
			ofstream ofs(filename);
			double ARE = 0;
			for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
			{
				uint8_t key[13];
				memcpy(key, (it->first).c_str(), 13);
				bool flag;
				int est_val=elastic->query(key,flag);
				//解码流ID
				uint32_t s,d,num;//source,dest,number
				s=(uint32_t)(key[0]);
				d=(uint32_t)(key[1]);
				num=(uint32_t)((key[2]<<8)|key[3]);
				ofs<<s<<" "<<d<<" "<<num<<" "<<est_val<<" "<<flag<<endl;
				int dist = std::abs(it->second - est_val);
				ARE += dist * 1.0 / (it->second);
			}
			ARE /= (int)Real_Freq.size();
			cout << to_string(node)+" balanced ARE:"<<ARE<<endl;
			ofs.close();
			delete elastic;
			Real_Freq.clear();
		}

}



