/*
 * Util.cpp
 *
 *  Created on: 19 Sep 2013
 *      Author: geoffrey
 */
#include "Util.hpp"



/**
 * @brief get_error_string converts Opencl error code to humanform
 * @param err
 * @return const char *
 */
const char * get_error_string(cl_int err){
    switch(err){
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";

    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    default: return "Unknown OpenCL error";
    }
}

/*
void print_16_x_n(int32_t * key, uint32_t n)
{
    if(n%16 != 0 )
        exit(3);
    for (auto i  =0 ; i < n; i+=16)
    {

        for (auto j = 0; j < 16; ++j)
        {
            std::cout << key[i+j] << ",";
        }

        std::cout << "\n";

    }

}

void random_kv_long(key_value_64 * V, const int lower , const int upper,
		             const unsigned seed, size_t num)
{
	std::default_random_engine eng(seed);
	std::uniform_int_distribution <cl_int> distr(lower, upper);

	for (auto i = 0 ; i < num; ++i)
	{
		V[i].k_v[0] = distr(eng);
		V[i].k_v[1] = distr(eng);

	}
}

void print_kv(key_value_64 * kv, int n)
{
	for (auto i  = 0; i < n ; ++i)
	{
		std::cout <<"[ " <<kv[i].k_v[0] << " : " << kv[i].k_v[1] << " ] --> "
		<< kv[i].kv << std::endl;

	}
}


void gen_device_idx(cl_uint * idxs, cl_uint num_threads )
{
	for (cl_uint  i  = 0; i < num_threads; ++i)
	{
		idxs[i] = i*32;
		std::cout << "thread indexes " << idxs[i] << std::endl;
	}
}

void random_kv_long_struct(Keys * V, const int lower , const int upper,
		             const unsigned seed, size_t num)
{
	std::default_random_engine eng(seed);
	std::uniform_int_distribution <cl_int> distr(lower, upper);


	for (auto i = 0 ; i < num; ++i)
	{
		V->kvp[i].k_v[0] = distr(eng);
		V->kvp[i].k_v[1] = distr(eng);

	}
}

void print_kv_struct(Keys * kv, int n)
{
	for (auto i  = 0; i < n ; ++i)
	{
		std::cout <<"[ " <<kv->kvp[i].k_v[0]
		          << " : " << kv->kvp[i].k_v[1]
		          << " ] --> "
		          << kv->kvp[i].kv << std::endl;

	}
}


void print_keys(Keys * kv, int num_packs, int pack_size)
{
    for (auto i  = 0; i <num_packs; ++i )
    {
        for (auto j = 0; i < pack_size; j++ )
        {
            std::cout << "[ " << kv[i].kvp[j].k_v[0] << " : " << kv[i].kvp[j].k_v[1] <<  " ] --> " << kv[i].kvp[j].kv << std::endl;

        }
    }
}

void print_keys_int2(std::vector<cl_int2> &V)
{
    for (auto i = 0 ; i < V.size() ; ++i)
    {



            cl_int * keys2 = (cl_int *) &V[i];
            std::cout << "[ " << keys2[0]<< " : " << keys2[1] <<  " ] " <<
            std::endl;
    }
}


void rnd_fill_int2(std::vector<cl_int2> &V, const int lower, const int upper,
        const unsigned int seed)
{
    //use the default random engine and an uniform distribution
    std::default_random_engine eng(seed);
    std::uniform_int_distribution <int> distr(lower, upper);


    for (auto i = 0 ; i < V.size() ; ++i)
    {



            cl_int * keys2 = (cl_int *) &V[i];

            keys2[0] = distr(eng);
            keys2[1] = distr(eng);
            std::cout << "[ " << keys2[0]<< " : " << keys2[1] <<  " ] " <<
            std::endl;
    }

    //for( auto &elem : V){
    //   elem = distr(eng);
    //}

};

void range(std::vector<cl_int> &V, int start, int stride)
{


    for (auto i  = 0; i < V.size(); ++i)
    {
        std::cout << " range :" << i  << " : " << start << "\n";
        if(i == 0) V.at(i) = (cl_int) start;
        else V.at(i) = (cl_int) start;
        start = start + stride;


    }

}

void print_keys_int3(cl_int2 * V, int num)
{
    for (auto i = 0 ; i < num ; ++i)
    {



            cl_int * keys2 = (cl_int *) &V[i];
            std::cout << "[ " << keys2[0]<< " : " << keys2[1] <<  " ] " <<
            std::endl;
    }
}


void rnd_fill_int2_array(cl_int2 *V, const int lower, const int upper,
        const unsigned int seed, const unsigned int num )
{
    //use the default random engine and an uniform distribution
    std::default_random_engine eng(seed);
    std::uniform_int_distribution <int> distr(lower, upper);


    for (auto i = 0 ; i < num ; ++i)
    {



            cl_int * keys2 = (cl_int *) &V[i];

            keys2[0] = distr(eng);
            keys2[1] = distr(eng);
            std::cout << "[ " << keys2[0]<< " : " << keys2[1] <<  " ] " <<
            std::endl;
    }

    //for( auto &elem : V){
    //   elem = distr(eng);
    //}

};

bool is_aligned(void *p, int N)
{
    return ((unsigned long)p & (ALIGN - 1)) == 0;
}*/


