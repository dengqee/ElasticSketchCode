/*
 * TCAMSketch.h
 *
 *  Created on: 2019年5月9日
 *      Author: dengqi
 */
#ifndef SRC_TCAMSKETCH_FLOWMAPSKETCH_H_
#define SRC_TCAMSKETCH_FLOWMAPSKETCH_H_

#include<iostream>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include <map>

#include "../CMSketch/CM.h"




#define CMSKETCH_D 4
#define CMSKETCH_KEY_LEN 4

using namespace std;
class TCAMSketch
{
private:

	map<string,unsigned int>m_tcam;
	CMSketch<CMSKETCH_KEY_LEN,CMSKETCH_D>*m_cmSketch;
	long m_theta;
	int tcam_cnt;
	int m_tcamLimit;
	int m_total;//the total number of packets
public:
	TCAMSketch(int theta,int tcamLimit,int cmcounter_num):

		m_cmSketch(new CMSketch<CMSKETCH_KEY_LEN,CMSKETCH_D>(cmcounter_num*4)),
		m_theta(theta),tcam_cnt(0),m_tcamLimit(tcamLimit),m_total(0)
	{}
	~TCAMSketch()
	{

		delete m_cmSketch;
	}
	void settheta(long theta)
	{
		m_theta=theta;
	}
	bool insert(uint8_t* key)
	{
		m_total++;
//		m_theta=tcam_cnt<=m_theta?m_theta:tcam_cnt/m_tcam.size();
//		m_theta=(m_total-tcam_cnt)/(5*(m_tcamLimit-m_tcam.size()+1))<m_theta?m_theta:(m_total-tcam_cnt)/(m_tcamLimit-m_tcam.size()+1);
		bool ret=false;//true,if insert to cmsketch;false otherwise
		string str((const char*)(key), 4);
		auto it=m_tcam.find(str);
		if(it!=m_tcam.end())//found
		{
			it->second++;
			tcam_cnt++;
		}
		else//no found
		{
			ret=true;
			int count=m_cmSketch->insert_query(key);

			if(count>m_theta&&m_tcam.size()<m_tcamLimit)
			{
				m_tcam[str]=count;
//				m_cmSketch->pop(key,count);
				tcam_cnt+=count;
			}

		}
		return ret;
	}
	void print()
	{
		cout<<"tcam count:"<<tcam_cnt<<" tcam.size="<<m_tcam.size()<<endl;
	}
	void insert(uint8_t* key,bool b)//only for throughput
	{
		if(b)
			m_cmSketch->insert(key);
	}
	unsigned int query(uint8_t* key)
	{
		string str((const char*)(key), 4);
		auto it=m_tcam.find(str);
		if(it!=m_tcam.end())//found
		{
			return it->second;
		}
		return m_cmSketch->query(key);
	}
	map<string,unsigned int>GetTCAM()
	{
		return m_tcam;
	}

};


#endif /* SRC_TCAMSKETCH_FLOWMAPSKETCH_H_ */



