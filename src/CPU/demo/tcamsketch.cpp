/*
 * tcamsketch.cpp
 *
 *  Created on: 2019年5月27日
 *      Author: dengqi
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <map>
#include <algorithm>
#include "../TCAMSketch/TCAMSketch.h"
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

//		printf("Successfully read in %s, %ld packets\n", datafileName, traces[datafileCnt - 1].size());
	}
//	printf("\n");
}
bool cmp(const pair<string,uint32_t>&a,const pair<string,uint32_t>&b)
{
	return a.second>b.second;
}

int main()
{
//	ReadInTraces("../../../data/");
	ReadInTraces("/home/dengqi/eclipse-workspace/ElasticSketchCode/data/");

//#define SK_D 3
	TCAMSketch *tcamsketch = NULL;
	int theta;
	int tcamLimit;
	int cmcounter_num;//the TOTAL number of cmsketch counters
	printf("tcam cm theta ARE prec\n");
	for(int cmcounter_num=40*10000;cmcounter_num<=40*10000;cmcounter_num+=5*10000)
	for(int tcamLimit=1000;tcamLimit<=12000;tcamLimit+=1000)
	for(int theta=20;theta<=20;theta+=10)
//	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		unordered_map<string, int> Real_Freq;


//		int packet_cnt = (int)traces[datafileCnt - 1].size();
		int packet_cnt = (int)traces[1 - 1].size();
//		theta+=1;
		tcamsketch = new TCAMSketch(theta,tcamLimit,cmcounter_num);
		for(int i = 0; i < packet_cnt; ++i)
//		for(int i = 0; i < 10000; ++i)
		{
			tcamsketch->insert((uint8_t*)(traces[1 - 1][i].key));

			string str((const char*)(traces[1 - 1][i].key), 4);
			Real_Freq[str]++;
		}
//		string data = "/home/dengqi/eclipse-workspace/ElasticSketchCode/src/CPU/FlowmapSketch/data_cplex/";
//		data +=std::to_string(datafileCnt)+".dat";
//		tcamsketch->out_cplex(data);
		double ARE = 0;
		int heavyThreshold=packet_cnt/1000;
		map<string,uint32_t>realheavymap,estheavymap;
		for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
		{

			if(it->second>=heavyThreshold)
				realheavymap[it->first]=it->second;
			uint8_t key[4];
			memcpy(key, (it->first).c_str(), 4);
			int est_val = tcamsketch->query(key);
			if(est_val>=heavyThreshold)
				estheavymap[it->first]=it->second;
			int dist = std::abs(it->second - est_val);
			ARE += dist * 1.0 / (it->second);
		}
		ARE /= (int)Real_Freq.size();

		//heavy hitter


		int numDet=0;
		for(auto it=estheavymap.begin();it!=estheavymap.end();++it)
		{
			auto tmp=realheavymap.find(it->first);
			if(tmp!=realheavymap.end())
				numDet++;
		}
		double prec=1.0*numDet/estheavymap.size();

//		printf("%d.dat: ARE=%.3lf precision=%.3lf\n", datafileCnt - 1, ARE, prec);
//		printf("theta %d ARE %.3lf \n", theta, ARE);
//		tcamsketch->print();
//		cout<<"flow num:"<<Real_Freq.size()<<endl;
		cout<<tcamLimit<<" "<<cmcounter_num<<" "<<theta<<" "<<ARE<<" "<<prec<<endl;
		delete tcamsketch;
		Real_Freq.clear();
	}
}




