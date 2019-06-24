#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include "../CMSketch/CM.h"
using namespace std;

#define START_FILE_NO 1
#define END_FILE_NO 1


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
	ReadInTraces("../../../data/");

	string outdir="../../../data/cmsketch/";

#define SK_D 4
	CMSketch<4, SK_D> *cm = NULL;
	cout<<"cmsketch"<<" "<<"ARE"<<" "<<endl;
	int cmcounter_num=40*10000;//the TOTAL number of cmsketch counters

	for(cmcounter_num=30*10000;cmcounter_num<=80*10000;cmcounter_num+=5*10000)
	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		string outfile=outdir+to_string(cmcounter_num)+".txt";
		ofstream ofs(outfile.c_str());
		unordered_map<string, int> Real_Freq;
		cm = new CMSketch<4, SK_D>(cmcounter_num*4);

		int packet_cnt = (int)traces[datafileCnt - 1].size();
		for(int i = 0; i < packet_cnt; ++i)
		{
			cm->insert((uint8_t*)(traces[datafileCnt - 1][i].key));

			string str((const char*)(traces[datafileCnt - 1][i].key), 4);
			Real_Freq[str]++;
		}

		double ARE = 0,RMMAE=0;
		long sum=0;
		for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
		{
			uint8_t key[4];
			memcpy(key, (it->first).c_str(), 4);
			int est_val = cm->query(key);

			ofs<<*((uint32_t*)key)<<" "<<est_val<<endl;



			int dist = std::abs(it->second - est_val);
			ARE += dist * 1.0 / (it->second);
			RMMAE+=dist*dist;
			sum+=(it->second)*(it->second);
		}
		ofs.close();
		ARE /= (int)Real_Freq.size();
		RMMAE=sqrt(RMMAE/sum);

//		printf("%d.dat: ARE=%.3lf\n", datafileCnt - 1, ARE);
		cout<<cmcounter_num<<" "<<ARE<<" "<<RMMAE<<" "<<sum<<endl;

		delete cm;
		Real_Freq.clear();
	}
}	
