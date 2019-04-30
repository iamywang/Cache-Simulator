#include <iostream>
#include <fstream>
#include <bitset>
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <ctime>

#define SetAssociative_Random_WriteBack
#define MAX_CACHE_LINE 4096 // 4096(2^12)
#ifndef STRUCT_TYPE
#define STRUCT_TYPE

// 内存地址与Cache地址的关联方式：直接映射、组相联、全相联
enum associativity_way
{
    direct_mapped = 1,
    set_associative,
    full_associative
};

// 替换策略：none（直接替换），FIFO（先进先出算法），LRU（最近最少用算法），LFU（最不经常用算法），Random（随机替换算法）
enum replacement_way
{
    none,
    FIFO = 1,
    LRU,
    LFU,
    Random
};

// 写策略：write_through（全写法），write_back（回写法）
enum write_way
{
    write_through = 1,
    write_back
};
#endif // STRUCT_TYPE
using namespace std;

typedef enum associativity_way ASSOC;
typedef enum replacement_way REPLACE;
typedef enum write_way WRITE;

unsigned int long cache_size = 0;      //cache size
unsigned int long cache_line_size = 0; //cacheline size
unsigned int long cache_set = 0;       //cache set

unsigned int long num_line = 0; //How many lines of the cache.
unsigned int long num_set = 0;  //How many sets of the cache.

ASSOC t_assoc = direct_mapped; //associativity method,default direct_mapped
REPLACE t_replace = none;      //replacement policy,default Random
WRITE t_write = write_back;    //write policy,default write_back

short unsigned int bit_block = 0; //How many bits of the block.
short unsigned int bit_line = 0;  //How many bits of the line.
short unsigned int bit_tag = 0;   //How many bits of the tag.
short unsigned int bit_set = 0;   //How many bits of the set.

unsigned long int num_access = 0;  //Number of cache access
unsigned long int num_iaccess = 0; //Number of cache access
unsigned long int num_load = 0;    //Number of cache load
unsigned long int num_store = 0;   //Number of cache store
unsigned long int num_space = 0;   //Number of space line

unsigned long int num_hit = 0;       //Number of cache hit
unsigned long int num_load_hit = 0;  //Number of load hit
unsigned long int num_iload_hit = 0; //Number of load hit
unsigned long int num_store_hit = 0; //Number of store hit

float f_ave_rate = 0.0;    //Average cache hit rate
float f_load_rate = 0.0;   //Cache hit rate for loads
float f_store_rate = 0.0;  //Cache hit rate for stores
float f_icache_rate = 0.0; //Cache hit rate for icache

std::bitset<32> cache_item[MAX_CACHE_LINE];     // [31]:valid,[30]:hit,[29]:dirty,[28]-[0]:data
unsigned long int LRU_priority[MAX_CACHE_LINE]; //For LRU policy's priority
unsigned long int current_line = 0;             // The line num which is processing
unsigned long int current_set = 0;              // The set num which is processing
unsigned long int i = 0, j = 0;                 //For loop
unsigned long int temp = 0;                     //A temp varibale

bool getHitNum(char *address);
void getHitRate();
bool IsHit(std::bitset<32> flags);
void getReplace(std::bitset<32> flags);
void getRead(std::bitset<32> flags);
void getWrite();
void CacheInit(int c_size, int c_l_size, int m_method, int c_sets, int w_policy, int r_policy);
void CacheExec(char *trace_file);
void PrintOutput();
void LruHitProcess();
void LruUnhitSpace();
void LruUnhitUnspace();
