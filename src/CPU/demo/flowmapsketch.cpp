/*
 * flowmapsketch.cpp
 *
 *  Created on: 2019年5月9日
 *      Author: dengqi
 */
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>

#include "../FlowmapSketch/flowmapsketch.h"
using namespace std;

#define START_FILE_NO 1
#define END_FILE_NO 10


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

		printf("Successfully read in %s, %ld packets\n", datafileName, traces[datafileCnt - 1].size());
	}
	printf("\n");
}

int main()
{
//	ReadInTraces("../../../data/");
	ReadInTraces("/home/dengqi/eclipse-workspace/ElasticSketchCode/data/");

//#define SK_D 3
	FlowMapSketch *flowmapsketch = NULL;


	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		unordered_map<string, int> Real_Freq;
		flowmapsketch = new FlowMapSketch();

		int packet_cnt = (int)traces[datafileCnt - 1].size();
//		for(int i = 0; i < packet_cnt; ++i)
		for(int i = 0; i < 10000; ++i)
		{
			flowmapsketch->insert((char*)(traces[datafileCnt - 1][i].key));

			string str((const char*)(traces[datafileCnt - 1][i].key), 4);
			Real_Freq[str]++;
		}
		string data = "/home/dengqi/eclipse-workspace/ElasticSketchCode/src/CPU/FlowmapSketch/data_cplex/";
		data +=std::to_string(datafileCnt)+".dat";
		flowmapsketch->out_cplex(data);
//		double ARE = 0;
//		for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
//		{
//			uint8_t key[4];
//			memcpy(key, (it->first).c_str(), 4);
//			int est_val = flowmapsketch->query(key);
//			int dist = std::abs(it->second - est_val);
//			ARE += dist * 1.0 / (it->second);
//		}
//		ARE /= (int)Real_Freq.size();
//
//
//		printf("%d.dat: ARE=%.3lf\n", datafileCnt - 1, ARE);


		delete flowmapsketch;
		Real_Freq.clear();
	}
}



