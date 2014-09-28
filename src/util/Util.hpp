/*
 * Util.hpp
 *
 *  Created on: 19 Sep 2013
 *      Author: geoffrey
 */

#ifndef UTIL_HPP_
#define UTIL_HPP_
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <CL/cl.h>


#define ALIGN 64

/*typedef  union
{
	cl_long kv;
	cl_int k_v[2];
} key_value_64;

struct Keys
{
	key_value_64 kvp[16];
};*/

const char * get_error_string(cl_int err);
/*void print_16_x_n(int32_t * key, uint32_t n);
void print_kv(key_value_64 * kv, int n);
void gen_device_idx(cl_uint * idxs, cl_uint num_threads );
void random_kv_long_struct(Keys * V, const int lower , const int upper,
		             const unsigned seed, size_t num);
void print_kv_struct(Keys * kv, int n);
void print_keys(Keys * kv, int num_packs, int pack_size);


template <typename intype>
void rnd_fill(std::vector<intype> &V, const int lower, const int upper,
		const unsigned int seed)
{
    //use the default random engine and an uniform distribution
    std::default_random_engine eng(seed);
    std::uniform_int_distribution <intype> distr(lower, upper);

    for( auto &elem : V){
        elem = distr(eng);
    }

};

template <typename intype>
void uni_random_fill(intype * V, const int lower , const int upper,
		             const unsigned seed, size_t num)
{
	std::default_random_engine eng(seed);
	std::uniform_int_distribution <intype> distr(lower, upper);

	for (auto i = 0 ; i < num; ++i)
	{
		V[i] = distr(eng);
	}
}




void random_kv_long(key_value_64 * V, const int lower , const int upper,
		             const unsigned seed, size_t num);

void print_keys_int2(std::vector<cl_int2> &V);

void rnd_fill_int2(std::vector<cl_int2> &V, const int lower, const int upper,
        const unsigned int seed);

void range(std::vector<cl_int> &V, int start, int stride);

void print_keys_int3(cl_int2 * V, int num);

void rnd_fill_int2_array(cl_int2 *V, const int lower, const int upper,
        const unsigned int seed, const unsigned int num );

bool is_aligned(void *p, int N);

std::string getBoardBinaryFile(const char *prefix, cl_device_id device);*/

#endif /* UTIL_HPP_ */
