/*
 * tcamsketch-throughput.cpp
 *
 *  Created on: 2019年5月27日
 *      Author: dengqi
 */
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <set>
#include <iostream>
#include "../TCAMSketch/TCAMSketch.h"
//#include "../MultiCore/setup.h"
using namespace std;

#define START_FILE_NO 1
#define END_FILE_NO 1
#define test_cycles 10


struct FIVE_TUPLE{	char key[13];	};
typedef vector<FIVE_TUPLE> TRACE;
TRACE traces[END_FILE_NO - START_FILE_NO + 1];

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

//		printf("Successfully read in %s, %ld packets\n", datafileName, traces[datafileCnt - 1].size());
	}
//	printf("\n");
}

int main()
{
	ReadInTraces("/home/dengqi/eclipse-workspace/ElasticSketchCode/data/");


#define HEAVY_MEM (150 * 1024)
#define BUCKET_NUM (HEAVY_MEM / 64)
#define TOT_MEM_IN_BYTES (600 * 1024)
//	ElasticSketch<BUCKET_NUM, TOT_MEM_IN_BYTES> *elastic = NULL;
	int theta;
	int tcamLimit;
	int cmcounter_num;//the TOTAL number of cmsketch counters



//	printf("tcam cm theta th\n");
	for(cmcounter_num=40*10000;cmcounter_num<=40*10000;cmcounter_num+=5*10000)
	for(tcamLimit=2000;tcamLimit<=20000;tcamLimit+=2000)
	for(theta=15;theta<=15;theta+=10){
		TCAMSketch *tcamsketch = NULL;
	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{

		set<string>flowID;


		tcamsketch = NULL;

		timespec time1, time2;
		long long resns;
		int packet_cnt = (int)traces[datafileCnt - 1].size();

		uint8_t **keys = new uint8_t*[(int)traces[datafileCnt - 1].size()];
		for(int i = 0; i < (int)traces[datafileCnt - 1].size(); ++i)
		{
			keys[i] = new uint8_t[13];
			memcpy(keys[i], traces[datafileCnt - 1][i].key, 13);
			string key_str((const char*)(traces[datafileCnt - 1][i].key), 4);
			flowID.insert(key_str);
		}
		int flow_cnt=flowID.size();
		set<string>::iterator flow_it=flowID.begin();
		set<string>flow_ins;
		int flow_ins_num=0;

//		for(int sample=1;sample<=1;sample++)
//		{
//			flow_ins_num+=flow_cnt/10;
//			//抽样
//			for(;flow_ins.size()<=flow_ins_num;flow_it++)
//			{
//				if(flow_it==flowID.end())
//					break;
//				flow_ins.insert(*flow_it);
//			}
//			bool tag[3000000];
//			for(int i=0;i<packet_cnt;i++)
//			{
//				string key_str((const char*)(traces[datafileCnt - 1][i].key), 4);
//				if(flow_ins.find(key_str)!=flow_ins.end())//如果找到
//					tag[i]=1;
//				else
//					tag[i]=0;
//			}



			//测量预处理

			int* heavy=new int[packet_cnt];

			int packet_insert=0;
			tcamsketch = new TCAMSketch(theta,tcamLimit,cmcounter_num);
			for(int i = 0; i < packet_cnt; ++i)
//					if(tag[i])
				if(1)
				{
					heavy[i]=tcamsketch->insert(keys[i]);
					if(heavy[i]){
						packet_insert++;
					}
				}
				else
					heavy[i]=0;
			delete tcamsketch;
			uint8_t **ins_keys = new uint8_t*[packet_insert];
			int j=0;
			for(int i = 0; i < (int)traces[datafileCnt - 1].size(); ++i)
			{
				if(heavy[i])
				{
					ins_keys[j]= new uint8_t[13];
					memcpy(ins_keys[j], traces[datafileCnt - 1][i].key, 13);
					j++;
				}

			}

			//true measure
			tcamsketch = new TCAMSketch(theta,tcamLimit,cmcounter_num);

			clock_gettime(CLOCK_MONOTONIC, &time1);
			for(int t = 0; t < test_cycles; ++t)
			{

				for(int i = 0; i < packet_insert; ++i)
//					tcamsketch->insert(keys[i],heavy[i]);
//					if(heavy[i])
					tcamsketch->insert(ins_keys[i],1);
			}
			clock_gettime(CLOCK_MONOTONIC, &time2);
			delete tcamsketch;
			delete[] heavy;
			resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
			double th = (double)1000.0 * test_cycles * packet_cnt / resns;
//			printf("%d.dat sampling rate:%d throughput is %lf mbps\n",datafileCnt,sample,th);

//		}
		cout<<tcamLimit<<" "<<cmcounter_num<<" "<<theta<<" "<<th<<endl;
			/* free memory */
			for(int i = 0; i < (int)traces[datafileCnt - 1].size(); ++i)
				delete[] keys[i];
			delete[] keys;



	}
	}
}



