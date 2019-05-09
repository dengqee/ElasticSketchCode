/*************************************************************************
  > File Name: bloomfilter.h
  > Author: dengqi
  > Mail: 906262038@qq.com 
  > Created Time: 2019年05月08日 星期三 16时16分36秒
 ************************************************************************/
#ifndef _BLOOMFILTER_H_
#define _BLOOMFILTER_H_

#include<iostream>
#include<vector>
#include<string>
#include "../common/BOBHash32.h"
using namespace std;
template<unsigned int nhash>//nhash表示几个hash函数
class BloomFilter
{
	private:
		vector<int>m_bitVec;//bloomflilter的位数组
		BOBHash32* hash[nhash] = {NULL};
	public:
		/*
		 * 构造函数 
		 * 参数 bloom_size 为bit_Vec的大小 
		 */
		BloomFilter(int bloom_size):m_bitVec(vector<int>(bloom_size,0))
	{
			for(int i = 0; i<nhash;++i)
				hash[i] = new BOBHash32(i + 50);
	}
		~BloomFilter()
		{
			for(int i =0;i<nhash;++i)
			{
				delete hash[i];
			}
		}
		/*
		 * 更新
		 * 参数 流ID
		 */
		void Update(string key)
		{
			for(int i = 0; i<nhash;i++)
			{
				size_t index = (hash[i]->run((const char *)key.c_str(), key.size()+1)) % m_bitVec.size();

				m_bitVec[index] = 1;
			}

		}
		bool Check(string key)
		{
			size_t index;
			bool res=true;
			for(int i = 0; i<nhash;i++)
			{
				size_t index = (hash[i]->run((const char *)key.c_str(), key.size()+1)) % m_bitVec.size();

				res = res && m_bitVec[index];
			}
			return res;


		}

};

#endif
