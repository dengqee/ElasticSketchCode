#ifndef _CMSKETCH_H_VERSION_2
#define _CMSKETCH_H_VERSION_2

#include <sstream>
#include <cstring>
#include <algorithm>
#include <cstdint>
#include "../common/BOBHash32.h"

using namespace std;

template<int key_len, int d>
class CMSketch
{
private:
	int memory_in_bytes = 0;

	int w = 0;
	uint8_t* counters[d] = {NULL};
	BOBHash32* hash[d] = {NULL};

public:
	string name;

	CMSketch(){}
	CMSketch(int memory_in_bytes)
	{	initial(memory_in_bytes);	}
	~CMSketch(){clear();}

	void initial(int memory_in_bytes)
	{
		this->memory_in_bytes = memory_in_bytes;
		w = memory_in_bytes / 1 / d;

		for(int i = 0; i < d; ++i){
			counters[i] = new uint8_t[w];
			memset(counters[i], 0, 1 * w);

			hash[i] = new BOBHash32(i*10 + 750);
		}


		stringstream name_buffer;
        name_buffer << "CM@" << memory_in_bytes << "@" << d;
        name = name_buffer.str();
	}
	void clear()
	{
		for(int i = 0; i < d; ++i)
			delete[] counters[i];

		for (int i = 0; i < d; ++i)
			delete hash[i];
	}

	void print_basic_info()
    {
        printf("CM sketch\n");
        printf("\tCounters: %d\n", w);
        printf("\tMemory: %.6lfMB\n", w * 1.0 / 1024 / 1024);
    }

    void insert(uint8_t * key, int f = 1)
    {
        for (int i = 0; i < d; i++) {
            int index = (hash[i]->run((const char *)key, key_len)) % w;
            int old_val = (int)counters[i][index];
			int new_val = (int)counters[i][index] + f;

			new_val = new_val < 255 ? new_val : 255;
			counters[i][index] = (uint8_t)new_val;
        }
    }

    int insert_query(uint8_t * key, int f = 1)
	{
    	int ret = 1 << 30;
		for (int i = 0; i < d; i++) {
			int index = (hash[i]->run((const char *)key, key_len)) % w;
			int old_val = (int)counters[i][index];
			int new_val = (int)counters[i][index] + f;

			new_val = new_val < 255 ? new_val : 255;
			counters[i][index] = (uint8_t)new_val;
			ret = min(ret, new_val);
		}
		return ret;
	}
    void pop(uint8_t*key,int f)
    {
    	for (int i = 0; i < d; i++) {
			int index = (hash[i]->run((const char *)key, key_len)) % w;
			counters[i][index] -= f;
		}
    }
	int query(uint8_t * key)
    {
        int ret = 1 << 30;
        for (int i = 0; i < d; i++) {
        	int index = (hash[i]->run((const char *)key, key_len)) % w;
            int tmp = counters[i][index];
            ret = min(ret, tmp);
        }
        return ret;
    }
};


#endif
