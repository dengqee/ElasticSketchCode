/*
 * elastic-n.cpp
 *
 *  Created on: 2019年6月22日
 *      Author: dengqi
 */
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <fstream>
#include "../elastic/ElasticSketch-n.h"
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
	ReadInTraces("/home/dengqi/eclipse-workspace/ElasticSketchCode/data/");

	string dir="/home/dengqi/eclipse-workspace/ElasticSketchCode/data/";
#define COUNTER_NUM 2000*10
#define BUCKET_NUM COUNTER_NUM/8
	int light_num;
	for(light_num=30*10000;light_num<=80*10000;light_num+=5*10000)
	{
		string outdir=dir+"elastic_1/";
//		string md="mkdir "+outdir;
//		system(md.c_str());

		ElasticSketch<BUCKET_NUM> *elastic = NULL;

		for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
		{
			unordered_map<string, int> Real_Freq;
			elastic = new ElasticSketch<BUCKET_NUM>(light_num);

			long packet_cnt = (int)traces[datafileCnt - 1].size();
			for(int i = 0; i < packet_cnt; ++i)
			{
				elastic->insert((uint8_t*)(traces[datafileCnt - 1][i].key));
				// elastic->quick_insert((uint8_t*)(traces[datafileCnt - 1][i].key));

				string str((const char*)(traces[datafileCnt - 1][i].key), 4);
				Real_Freq[str]++;
			}

			double ARE = 0;
			double RMMAE=0;
			long sum=0;
			map<string,uint32_t>realheavymap,estheavymap;
			vector<pair<string,int> >orderRealFreq(Real_Freq.begin(),Real_Freq.end());
			vector<pair<string,int> >orderEstFreq;
//			string outfile1=outdir+to_string(COUNTER_NUM)+"_"+to_string(light_num)+"_real.txt";
			string outfile2=outdir+to_string(COUNTER_NUM)+"_"+to_string(light_num)+"_est.txt";
//			ofstream ofs1(outfile1.c_str());
			ofstream ofs2(outfile2.c_str());
			for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
			{


				uint8_t key[4];
				memcpy(key, (it->first).c_str(), 4);
				int est_val = elastic->query(key);
				orderEstFreq.push_back(make_pair(it->first,est_val));
//				ofs1<<*((uint32_t*)key)<<" "<<it->second<<endl;
				ofs2<<*((uint32_t*)key)<<" "<<est_val<<endl;

				int dist = std::abs(it->second - est_val);
				ARE += dist * 1.0 / (it->second);
				RMMAE+=1.0*dist*dist;
				sum+=(it->second)*(it->second);
			}
			ofs2.close();

			ARE /= (int)Real_Freq.size();
			RMMAE=sqrt(RMMAE/sum);
			//heavy hitter
			sort(orderRealFreq.begin(),orderRealFreq.end(),cmp);
			sort(orderEstFreq.begin(),orderEstFreq.end(),cmp);

			int numDet=0;
			for(size_t i=0;i<orderRealFreq.size()/10;i++)
			{
				for(size_t j=0;i<orderRealFreq.size()/10;j++)
				{
					if(orderRealFreq[i]==orderRealFreq[j])
					{
						numDet++;
						break;
					}

				}
			}

			double prec=1.0*numDet/(orderRealFreq.size()/10);

//			cout<<LIGHT_NUM<<" "<<ARE<<" "<<RMMAE<<" "<<prec<<endl;
			cout<<COUNTER_NUM<<" "<<light_num<<" "<<ARE<<" "<<RMMAE<<" "<<prec<<endl;
//			cout<<elastic->get_heavy_bucket_util()<<endl;


			delete elastic;
			Real_Freq.clear();
		}
	}
}




