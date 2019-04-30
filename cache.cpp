#include "cache.h"

using namespace std;

void CacheInit(int c_size, int c_l_size, int m_method, int c_sets, int w_policy, int r_policy)
{
    for (i = 0; i < num_line; i++)
    {
        cache_item[i].reset(); // [31]:valid,[30]:hit,[29]:dirty,[28]-[0]:data

        if (t_replace == LRU)
        {
            LRU_priority[i] = 0;
        }
    }

    cache_size = 64;
    cache_line_size = 32;
#ifdef SetAssociative_Random_WriteBack
    cache_set = 4;
#endif
    cache_set = 0;
    num_line = 0;
    num_set = 0;
#ifdef DirectMapped_None_WriteBack
    t_assoc = direct_mapped;
    t_replace = none;
    t_write = write_back;
#endif
#ifdef FullAssociative_Random_WriteBack
    t_assoc = full_associative;
    t_replace = Random;
    t_write = write_back;
#endif
#ifdef SetAssociative_Random_WriteBack
    t_assoc = set_associative;
    t_replace = Random;
    t_write = write_back;
#endif
    bit_block = 0;
    bit_line = 0;
    bit_tag = 0;
    bit_set = 0;
    num_access = 0;
    num_iaccess = 0;
    num_load = 0;
    num_store = 0;
    num_space = 0;
    num_hit = 0;
    num_load_hit = 0;
    num_iload_hit = 0;
    num_store_hit = 0;
    f_ave_rate = 0.0;
    f_load_rate = 0.0;
    f_store_rate = 0.0;
    f_icache_rate = 0.0;
    current_line = 0;
    current_set = 0;
    i = 0;
    j = 0; //For loop
    cache_size = c_size;
    cache_line_size = c_l_size;
    switch (m_method)
    {
    case 1:
        t_assoc = direct_mapped;
        break;
    case 2:
        t_assoc = set_associative;
        break;
    case 3:
        t_assoc = full_associative;
        break;
    }
    if (t_assoc == direct_mapped)
    {
        t_replace = none;
        switch (w_policy)
        {
        case 1:
            t_write = write_through;
            break;
        case 2:
            t_write = write_back;
            break;
        }
    }
    else if (t_assoc == full_associative)
    {
        switch (r_policy)
        {
        case 1:
            t_replace = FIFO;
            break;
        case 2:
            t_replace = LRU;
            break;
        case 3:
            t_replace = LFU;
            break;
        case 4:
            t_replace = Random;
            break;
        }
        switch (w_policy)
        {
        case 1:
            t_write = write_through;
            break;
        case 2:
            t_write = write_back;
            break;
        }
    }
    else if (t_assoc == set_associative)
    {
        cache_set = c_sets;

        switch (r_policy)
        {
        case 1:
            t_replace = FIFO;
            break;
        case 2:
            t_replace = LRU;
            break;
        case 3:
            t_replace = LFU;
            break;
        case 4:
            t_replace = Random;
            break;
        }
        switch (w_policy)
        {
        case 1:
            t_write = write_through;
            break;
        case 2:
            t_write = write_back;
            break;
        }
    }

    assert(cache_line_size != 0);
    num_line = (cache_size << 10) / cache_line_size;
    temp = cache_line_size;

    while (temp)
    {
        temp >>= 1;
        bit_block++;
    }

    bit_block--;

    if (t_assoc == direct_mapped)
    {
        bit_set = 0;
        temp = num_line;

        while (temp)
        {
            temp >>= 1;
            bit_line++;
        }

        bit_line--;
    }
    else if (t_assoc == full_associative)
    {
        bit_line = 0;
        bit_set = 0;
    }
    else if (t_assoc == set_associative)
    {
        bit_line = 0;
        assert(cache_set != 0);
        assert(num_line > cache_set);
        num_set = num_line / cache_set;
        temp = num_set;

        while (temp)
        {
            temp >>= 1;
            bit_set++;
        }

        bit_set--;
    }

    bit_tag = 29ul - bit_block - bit_line - bit_set;
    assert(bit_tag <= 29);
}
void CacheExec(char *trace_file)
{
    temp = num_line;

    for (i = 0; i < temp; i++)
    {
        cache_item[i][31] = true;
    }

    char address[13];
    ifstream in_file;
    in_file.open(trace_file, ios::in);
    while (!in_file.eof())
    {
        in_file.getline(address, 13);
        bool __attribute__((unused)) is_success = getHitNum(address);
        assert(is_success);
    }
    in_file.close();
    getHitRate();
}

void LruHitProcess()
{
    if (t_assoc == full_associative)
    {
        for (i = 0; i < num_line; i++)
        {
            if (LRU_priority[i] < LRU_priority[current_line] && cache_item[current_line][30] == true)
            {
                LRU_priority[i]++;
            }
        }

        LRU_priority[current_line] = 0;
    }
    else if (t_assoc == set_associative)
    {
        for (i = (current_set * cache_set); i < ((current_set + 1) * cache_set); i++)
        {
            if (LRU_priority[i] < LRU_priority[current_line] && cache_item[current_line][30] == true)
            {
                LRU_priority[i]++;
            }
        }

        LRU_priority[current_line] = 0;
    }
}

void LruUnhitSpace()
{
    if (t_assoc == full_associative)
    {
        for (i = 0; i < num_line; i++)
        {
            if (cache_item[current_line][30] == true)
            {
                LRU_priority[i]++;
            }
        }

        LRU_priority[current_line] = 0;
    }
    else if (t_assoc == set_associative)
    {
        for (i = (current_set * cache_set); i < ((current_set + 1) * cache_set); i++)
        {
            if (cache_item[current_line][30] == true)
            {
                LRU_priority[i]++;
            }
        }

        LRU_priority[current_line] = 0;
    }
}

void LruUnhitUnspace()
{
    if (t_assoc == full_associative)
    {
        temp = LRU_priority[0];

        for (i = 0; i < num_line; i++)
        {
            if (LRU_priority[i] >= temp)
            {
                temp = LRU_priority[i];
                j = i;
            }
        }

        current_line = j;
    }

    if (t_assoc == set_associative)
    {
        temp = LRU_priority[current_set * cache_set];

        for (i = (current_set * cache_set); i < ((current_set + 1) * cache_set); i++)
        {
            if (LRU_priority[i] >= temp)
            {
                temp = LRU_priority[i];
                j = i;
            }
        }

        current_line = j;
    }
}
bool getHitNum(char *address)
{
    bool is_store = false;
    bool is_load = false;
    bool is_iload = false;
    bool is_space = false;
    bool hit = false;

    switch (address[0])
    {
    case '0':
        is_iload = true;
        break;
    case '1':
        is_load = true;
        break;
    case '2':
        is_store = true;
        break;

    case '\0':
        is_space = true;
        break; //In case of space lines

    default:
        cout << "The address[0] is:" << address[0] << endl;
        cout << "ERROR IN JUDGE!" << endl;
        return false;
    }

    temp = strtoul(address + 2, NULL, 16);
    bitset<32> flags(temp); // flags if the binary of address
    hit = IsHit(flags);
    if (hit && is_iload) // 命中，指令读操作
    {
        num_iaccess++;
        num_iload_hit++;
        if (t_replace == LRU)
        {
            LruHitProcess();
        }
        else if (t_replace == FIFO)
        {
        }
    }
    else if (hit && is_load) // 命中，数据读操作
    {
        num_access++;
        num_load++;
        num_load_hit++;
        num_hit++;

        if (t_replace == LRU)
        {
            LruHitProcess();
        }
    }
    else if (hit && is_store) // 命中，写操作
    {
        num_access++;
        num_store++;
        num_store_hit++;
        num_hit++;
        cache_item[current_line][29] = true; //设置dirty为true

        if (t_replace == LRU)
        {
            LruHitProcess();
        }
        else if (t_replace == FIFO)
        {
        }
    }
    else if ((!hit) && is_iload) // 没命中，指令读操作
    {
        num_iaccess++;
        getRead(flags); // read data from memory

        if (t_replace == LRU)
        {
            LruUnhitSpace();
        }
        else if (t_replace == FIFO)
        {
        }
    }
    else if ((!hit) && is_load) // 没命中，读操作
    {
        num_access++;
        num_load++;
        getRead(flags); // read data from memory

        if (t_replace == LRU)
        {
            LruUnhitSpace();
        }
        else if (t_replace == FIFO)
        {
        }
    }
    else if ((!hit) && is_store) // 没命中，写操作
    {
        num_access++;
        num_store++;
        getRead(flags);                      // read data from memory
        cache_item[current_line][29] = true; //设置dirty为true

        if (t_replace == LRU)
        {
            LruUnhitSpace();
        }
        else if (t_replace == FIFO)
        {
        }
    }
    else if (is_space)
    {
        num_space++;
    }
    else
    {
        cerr << "Something ERROR" << endl;
        return false;
    }
    return true;
}

bool IsHit(bitset<32> flags)
{
    bool ret = false;

    if (t_assoc == direct_mapped)
    {
        bitset<32> flags_line; // a temp variable

        for (j = 0, i = (bit_block); i < (bit_block + bit_line); j++, i++) //判断在cache多少行
        {
            flags_line[j] = flags[i];
        }

        current_line = flags_line.to_ulong();
        assert(cache_item[current_line][31] == true);

        if (cache_item[current_line][30] == true) //判断hit位是否为真
        {
            ret = true;

            for (i = 31, j = 28; i > (31ul - bit_tag); i--, j--) //判断标记是否相同,i:address,j:cache
            {
                if (flags[i] != cache_item[current_line][j])
                {
                    ret = false;
                    break;
                }
            }
        }
    }
    else if (t_assoc == full_associative)
    {
        for (temp = 0; temp < num_line; temp++)
        {
            if (cache_item[temp][30] == true) //判断hit位是否为真
            {
                ret = true;

                for (i = 31, j = 28; i > (31ul - bit_tag); i--, j--) //判断标记是否相同,i:address,j:cache
                {
                    if (flags[i] != cache_item[temp][j])
                    {
                        ret = false;
                        break;
                    }
                }
            }

            if (ret == true)
            {
                current_line = temp;
                break;
            }
        }
    }
    else if (t_assoc == set_associative)
    {
        bitset<32> flags_set;

        for (j = 0, i = (bit_block); i < (bit_block + bit_set); j++, i++) //判断在cache多少组
        {
            flags_set[j] = flags[i];
        }

        current_set = flags_set.to_ulong();

        for (temp = (current_set * cache_set); temp < ((current_set + 1) * cache_set); temp++)
        {
            if (cache_item[temp][30] == true) //判断hit位是否为真
            {
                ret = true;

                for (i = 31, j = 28; i > (31ul - bit_tag); i--, j--) //判断标记是否相同,i:address,j:cache
                {
                    if (flags[i] != cache_item[temp][j])
                    {
                        ret = false;
                        break;
                    }
                }
            }

            if (ret == true)
            {
                current_line = temp;
                break;
            }
        }
    }

    return ret;
}

void getRead(bitset<32> flags)
{
    if (t_assoc == direct_mapped)
    {
        if (cache_item[current_line][30] == false) //hit is false
        {
            for (i = 31, j = 28; i > (31ul - bit_tag); i--, j--) //设置标记
            {
                cache_item[current_line][j] = flags[i];
                assert(j > 0);
            }

            cache_item[current_line][30] = true; //设置hit位为true
        }
        else
        {
            getReplace(flags);
        }
    }
    else if (t_assoc == full_associative)
    {
        bool space = false;

        for (temp = 0; temp < num_line; temp++)
        {
            if (cache_item[temp][30] == false) //find a space line
            {
                space = true;
                break;
            }
        }

        if (space == true)
        {
            current_line = temp; // 此处，temp不需减1，因为一旦发现空行，上面for循环会break，此时temp尚未++

            for (i = 31, j = 28; i > (31ul - bit_tag); i--, j--) //设置标记
            {
                cache_item[current_line][j] = flags[i];
                assert(j > 0);
            }

            cache_item[current_line][30] = true; //设置hit位为true.

            if (t_replace == LRU)
            {
                LruUnhitSpace();
            }
            else if (t_replace == FIFO)
            {
            }
        }
        else
        {
            getReplace(flags);
        }
    }
    else if (t_assoc == set_associative)
    {
        bool space = false;

        for (temp = (current_set * cache_set); temp < ((current_set + 1) * cache_set); temp++)
        {
            if (cache_item[temp][30] == false) //find a space line
            {
                space = true;
                break;
            }
        }

        if (space == true)
        {
            current_line = temp; // 此处，temp不需减1，因为一旦发现空行，上面for循环会break，此时temp尚未++

            for (i = 31, j = 28; i > (31ul - bit_tag); i--, j--) //设置标记
            {
                cache_item[current_line][j] = flags[i];
                assert(j > 0);
            }

            cache_item[current_line][30] = true; //设置hit位为true.

            if (t_replace == LRU)
            {
                LruUnhitSpace();
            }
            else if (t_replace == FIFO)
            {
            }
        }
        else
        {
            getReplace(flags);
        }
    }
}

void getReplace(bitset<32> flags)
{
    if (t_assoc == direct_mapped)
    {
    }
    else if (t_assoc == full_associative)
    {
        if (t_replace == Random)
        {
            current_line = rand() / (RAND_MAX / num_line + 1); // a random line in(0,num_line-1)
        }
        else if (t_replace == LRU)
        {
            LruUnhitUnspace();
        }
        else if (t_replace == FIFO)
        {
            current_line = rand() / (RAND_MAX / num_line + 1);
        }
    }
    else if (t_assoc == set_associative) // 从本组中任选一行，进行替换
    {
        if (t_replace == Random)
        {
            temp = rand() / (RAND_MAX / cache_set + 1);    // a random line in(0,cache_set-1)
            current_line = current_set * cache_set + temp; // a random line in current_set
        }
        else if (t_replace == LRU)
        {
            LruUnhitUnspace();
        }
        else if (t_replace == FIFO)
        {
            temp = rand() / (RAND_MAX / cache_set + 1);    // a random line in(0,cache_set-1)
            current_line = current_set * cache_set + temp; // a random line in current_set
        }
    }

    if (cache_item[current_line][29] == true) //dirty位必须为1才写入
    {
        getWrite(); //写入内存
    }

    for (i = 31, j = 28; i > (31ul - bit_tag); i--, j--) //设置标记
    {
        cache_item[current_line][j] = flags[i];
        assert(j > 0);
    }

    cache_item[current_line][30] = true; //设置hit位为true
}

void getWrite()
{
    cache_item[current_line][29] = false; //设置dirty为false
    cache_item[current_line][30] = false; //设置hit为false
}

void getHitRate()
{
    assert(num_access != 0);
    assert(num_load != 0);
    assert(num_store != 0);
    f_ave_rate = ((double)num_hit) / num_access;           //Average cache hit rate
    f_load_rate = ((double)num_load_hit) / num_load;       //Cache hit rate for loads
    f_store_rate = ((double)num_store_hit) / num_store;    //Cache hit rate for stores
    f_icache_rate = ((double)num_iload_hit) / num_iaccess; //Cache hit rate for stores
}

void PrintOutput()
{
    cout << "--------------------------------------------" << endl;
    cout << "|L1 " << cache_size << "KB|" << cache_line_size << "B|";

    switch (t_assoc)
    {
    case 1:
        cout << "直接映射|";
        break;

    case 2:
        cout << cache_set << "路组相联|";
        break;

    case 3:
        cout << "全相联|";
        break;

    default:
        break;
    }

    switch (t_replace)
    {
    case 0:
        cout << "NONE|";
        break;

    case 1:
        cout << "FIFO|";
        break;

    case 2:
        cout << "LRU|";
        break;

    case 3:
        cout << "LFU|";
        break;

    case 4:
        cout << "Random|";
        break;

    default:
        break;
    }

    switch (t_write)
    {
    case 1:
        cout << "写直达法|" << endl;
        break;

    case 2:
        cout << "写回法|" << endl;
        break;

    default:
        break;
    }
    cout << "i-cache访存总次数：" << num_iaccess << endl;
    cout << "命中率：" << f_icache_rate * 100 << " %" << endl;
    cout << "d-cache访存总次数：" << num_access << endl;
    cout << "读次数：" << num_load;
    cout << "|写次数：" << num_store << endl;
    cout << "命中率：" << f_ave_rate * 100 << " %" << endl;
    cout << "读命中率：" << f_load_rate * 100 << " %";
    cout << "|写命中率：" << f_store_rate * 100 << " %" << endl;
}
