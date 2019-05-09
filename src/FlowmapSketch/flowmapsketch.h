/*
 * flowmapsketch.h
 *
 *  Created on: 2019年5月9日
 *      Author: dengqi
 */

#ifndef SRC_FLOWMAPSKETCH_FLOWMAPSKETCH_H_
#define SRC_FLOWMAPSKETCH_FLOWMAPSKETCH_H_

#include<iostream>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <memory>

#include "../CMSketch/CM.h"
#include "../BloomFilter/bloomfilter.h"
#include "../FlowMap/flowmap.h"

#define FLOWMAP_SIZE 1000
#define FLOWMAP_NHASH 3
#define BLOOMFILTER_SIZE 1000
#define BLOOMFILTER_NHASH 3
#define CMSKETCH_D 4
#define CMSKETCH_KEY_LEN 4
#define CMSKETCH_MEMORY 600 * 1024
using namespace std;
class FlowMapSketch
{
private:
	shared_ptr<FlowMap<FLOWMAP_NHASH>>m_flowMap;
	shared_ptr<BloomFilter<BLOOMFILTER_NHASH>>m_bloomFilter;
	shared_ptr<CMSketch<CMSKETCH_KEY_LEN,CMSKETCH_D>>m_cmSketch;
	long m_theta;
public:
	FlowMapSketch():
		m_flowMap(new FlowMap<FLOWMAP_NHASH>(FLOWMAP_SIZE)),
		m_bloomFilter(new BloomFilter<BLOOMFILTER_NHASH>(BLOOMFILTER_NHASH)),
		m_cmSketch(new CMSketch<CMSKETCH_KEY_LEN,CMSKETCH_D>(CMSKETCH_MEMORY)),
		m_theta(1000)
	{}
	~FlowMapSketch()
	{}
	void settheta(long theta)
	{
		m_theta=theta;
	}
	void insert(string key)
	{
		if(m_bloomFilter->Check(key))
		{
			m_flowMap->insert(key);
		}
		else
		{
			m_cmSketch->insert(key.c_str());
			int count=m_cmSketch->query(key.c_str());
			if(count>m_theta)
			{
				m_flowMap->insert(key,count);
				m_bloomFilter->Update(key);
			}

		}
	}
	void out_cplex()
	{
		m_flowMap->output_cplex();
	}
};


#endif /* SRC_FLOWMAPSKETCH_FLOWMAPSKETCH_H_ */
