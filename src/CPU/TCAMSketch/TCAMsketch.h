/*
 * TCAMsketch.h
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
#define CMSKETCH_MEMORY 600 * 1024
using namespace std;
class TCAMSketch
{
private:

	map<string,unsigned int>m_tcam;
	CMSketch<CMSKETCH_KEY_LEN,CMSKETCH_D>*m_cmSketch;
	long m_theta;
public:
	TCAMSketch():

		m_cmSketch(new CMSketch<CMSKETCH_KEY_LEN,CMSKETCH_D>(CMSKETCH_MEMORY)),
		m_theta(100)
	{}
	~TCAMSketch()
	{

		delete m_cmSketch;
	}
	void settheta(long theta)
	{
		m_theta=theta;
	}
	bool insert(string key)
	{
		bool ret=false;//true,if insert to cmsketch;false otherwise
		auto it=m_tcam.find(key);
		if(it!=m_tcam.end())//found
		{
			it->second++;
		}
		else//no found
		{
			ret=true;
			int count=m_cmSketch->insert(key.c_str());
			
			if(count>m_theta)
			{
				m_tcam[key]=count;

			}

		}
		return ret;
	}
	void insert(const char* key,bool b)//only for throughput
	{
		if(b)
			m_cmSketch->insert(key);
	}
	unsigned int query(string key)
	{
		auto it=m_tcam.find(key);
		if(it!=m_tcam.end())//found
		{
			return it->second;
		}
		return m_cmSketch->query(key.c_str());
	}

};


#endif /* SRC_TCAMSKETCH_FLOWMAPSKETCH_H_ */
