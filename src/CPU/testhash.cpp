/*
 * testhash.cpp
 *
 *  Created on: 2019年5月17日
 *      Author: dengqi
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cmath>
#include "./CMSketch/CM.h"
#include "./common/BOBHash32.h"
using namespace std;
int main()
{
	BOBHash32* hash = new BOBHash32(750);
	timespec time1, time2;
	long long resns;
	char*key="qwe";
	clock_gettime(CLOCK_MONOTONIC, &time1);
	for(int i=0;i<10000;++i)
		for(int j=0;j<10000;++j)
			for(int k=0;k<100;++k)
				hash->run((const char *)key, 4);
	clock_gettime(CLOCK_MONOTONIC, &time2);
	resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
	cout<<resns/(10000.0*10000*100)<<endl;

	key="asd";
	clock_gettime(CLOCK_MONOTONIC, &time1);
	for(int i=0;i<10000;++i)
		for(int j=0;j<10000;++j)
			for(int k=0;k<100;++k)
				hash->run((const char *)key, 4);
	clock_gettime(CLOCK_MONOTONIC, &time2);
	resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
	cout<<resns/(10000.0*10000*100)<<endl;
	return 0;
}





