/*************************************************************************
  > File Name: flowmap.h
  > Author: dengqi
  > Mail: 906262038@qq.com 
  > Created Time: 2019年05月08日 星期三 17时01分07秒
 ************************************************************************/
#ifndef _FLOWMAP_H_
#define _FLOWMAP_H_

#include<iostream>
#include<vector>
#include<map>
#include<fstream>
#include "../common/BOBHash32.h"
using namespace std;
template<unsigned int  nhash>//nhash表示用几个hash函数
class FlowMap 
{
	public:
		FlowMap(unsigned int flowmapsize):m_counters(vector<long>(flowmapsize,0))
		{
			for(int i = 0; i<nhash;++i)
				hash[i] = new BOBHash32(i + 750);
		}
		~FlowMap()
		{
			for(int i =0;i<nhash;++i)
			{
				delete hash[i];
			}
		}
		void insert(const string &key,int count=1)
		{
			m_IDs.push_back(key);

			for(int i = 0; i < nhash; i++)
			{
				size_t index = (hash[i]->run((const char *)key.c_str(), key.size()+1)) % m_counters.size();

				m_counters[index] += count;
			}

		}
		void output_cplex(string filename)
		{			

			int r, c;//A的行数和列数
			r = m_counters.size(); 
			c = m_IDs.size();

			/*产生矩阵A*/
			vector<vector<int> >A(r,vector<int>(c,0));

			for(int j=0;j<c;++j)
			{
				for(int i = 0; i < nhash; i++)
				{
					size_t index = (hash[i]->run((const char *)m_IDs[j].c_str(), m_IDs[i].size()+1)) % m_counters.size();
					A[index][j] = 1;
				}

			}

			ofstream ofs(filename.c_str());
			if(!ofs)
			{
				cerr<<"can't open file: "<<filename<<endl;
				exit(1);
			}
			ofs << "r_range=" << r << ";" <<endl << "c_range=" << c << ";"<<endl;

			//打印Y	
			ofs << "Y=["<<m_counters[0];
			for(int i=1;i<m_counters.size();++i)
				
			{
				ofs << ","<<m_counters[i];
			}
			ofs << "];" << endl;
			//打印A
			ofs << "A=[";
			for (int i = 0; i < r;++i)
			{
				ofs << "[";
				for (int j = 0; j < c; ++j)
				{
					ofs << A[i][j];
					if (j < c - 1)ofs << ",";
				}
				ofs << "]";
				if (i < r - 1)ofs << ","<<endl<<"   ";
			}
			ofs << "];" << endl;

			ofs.close();
/*			ofs.open("cplex_data\\Y.txt");
			//打印Y	
			for (int i = 0; i < FLOWMAP_SIZE1; ++i)
			{
				for (int j = 0; j < FLOWMAP_SIZE2; ++j)
				{
					ofs << fmap[i][j] << endl;
				}		
			}
			ofs.close();
			//打印A
			ofs.open("cplex_data\\A.txt");

			for (int i = 0; i < r;++i)
			{	
				for (int j = 0; j < c; ++j)
				{
					ofs << A[i][j];
					if (j < c - 1)ofs << " ";
				}

				if (i < r - 1)ofs << " "<<endl;
			}
			ofs << endl;
*/			ofs.close();
		}

	private:
		vector<long>m_counters;//counter
		vector<string>m_IDs;//存储流ID
		BOBHash32* hash[nhash] = {NULL};
};
#endif
