#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include "../SpaceSaving/SpaceSaving.h"
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

	string dir="/home/dengqi/eclipse-workspace/ElasticSketchCode/data/";
	ReadInTraces(dir.c_str());
#define HEAVY 2000*10
#define TOT_MEM_IN_BYTES (HEAVY*44)
	SpaceSaving<4> *ss = NULL;



	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		string outdir=dir+"spacesaving/";
//		string md="mkdir "+outdir;
//		system(md.c_str());
		unordered_map<string, int> Real_Freq;
		ss = new SpaceSaving<4>(TOT_MEM_IN_BYTES);

		int packet_cnt = (int)traces[datafileCnt - 1].size();
		for(int i = 0; i < packet_cnt; ++i)
		{
			ss->insert((uint8_t*)(traces[datafileCnt - 1][i].key));

			string str((const char*)(traces[datafileCnt - 1][i].key), 4);
			Real_Freq[str]++;
		}

		string outfile=outdir+to_string(HEAVY)+"_est.txt";

		ofstream ofs(outfile.c_str());
		for(auto it=Real_Freq.begin();it!=Real_Freq.end();it++)
		{
			uint8_t key[4];
			memcpy(key, (it->first).c_str(), 4);
			int est_val = ss->query(key);
			ofs<<*((uint32_t*)key)<<" "<<est_val<<endl;
		}
		ofs.close();


//#define HEAVY_HITTER_THRESHOLD(total_packet) (total_packet * 1 / 1000)
//		vector< pair<string, uint32_t> > heavy_hitters;
//		ss->get_heavy_hitters(HEAVY_HITTER_THRESHOLD(packet_cnt), heavy_hitters);
//
//		printf("%d.dat: ", datafileCnt - 1);
//		printf("heavy hitters: <srcIP, count>, threshold=%d\n", HEAVY_HITTER_THRESHOLD(packet_cnt));
//		for(int i = 0, j = 0; i < (int)heavy_hitters.size(); ++i)
//		{
//			uint32_t srcIP;
//			memcpy(&srcIP, heavy_hitters[i].first.c_str(), 4);
//			printf("<%.8x, %d>", srcIP, (int)heavy_hitters[i].second);
//			if(++j % 5 == 0)
//				printf("\n");
//			else printf("\t");
//		}
//		printf("\n");


		delete ss;
		Real_Freq.clear();
	}
}	
