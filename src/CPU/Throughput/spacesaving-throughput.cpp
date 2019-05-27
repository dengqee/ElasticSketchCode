/*
 * spacesaving-throughput.cpp
 *
 *  Created on: 2019年5月23日
 *      Author: dengqi
 */
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <set>
#include <iostream>


#include "../SpaceSaving/SpaceSaving.h"
//#include "../MultiCore/setup.h"
using namespace std;

#define START_FILE_NO 1
#define END_FILE_NO 10
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
	SpaceSaving<4> *ss = NULL;


	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
		{
			set<string>flowID;


			ss = NULL;

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
			for(int sample=1;sample<=10;sample++)
			{
				flow_ins_num+=flow_cnt/10;
				for(;flow_ins.size()<=flow_ins_num;flow_it++)
				{
					if(flow_it==flowID.end())
						break;
					flow_ins.insert(*flow_it);
				}
				bool tag[3000000];
				for(int i=0;i<packet_cnt;i++)
				{
					string key_str((const char*)(traces[datafileCnt - 1][i].key), 4);
					if(flow_ins.find(key_str)!=flow_ins.end())//如果找到
						tag[i]=1;
					else
						tag[i]=0;
				}



				//测量
				clock_gettime(CLOCK_MONOTONIC, &time1);
				for(int t = 0; t < test_cycles; ++t)
				{
					//int packet_insert=0;
					ss = new SpaceSaving<4>(TOT_MEM_IN_BYTES);
					for(int i = 0; i < packet_cnt; ++i)
	//					if(i%(int)floor(1.0*packet_cnt/(packet_cnt*sample/100))==0)
						if(tag[i])
						{
							ss->insert(keys[i]);
							//packet_insert++;
						}
					delete ss;
					//cout<<packet_insert<<endl;
				}
				clock_gettime(CLOCK_MONOTONIC, &time2);
				resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
				double th = (double)1000.0 * test_cycles * packet_cnt / resns;
				//cout<<"time:"<<resns<<endl;
				printf("%d.dat sampling rate:%d throughput is %lf mbps\n",datafileCnt,sample,th);
			}
				/* free memory */
				for(int i = 0; i < (int)traces[datafileCnt - 1].size(); ++i)
					delete[] keys[i];
				delete[] keys;



		}
}



