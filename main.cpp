#include "cache.cpp"

void direct_asso(char *trace)
{
	// 1KB 8B direct write_through
	{
		CacheInit(1, 8, 1, 1, 1, 0);
		CacheExec(trace);
		PrintOutput();
	}
	// 1KB 8B direct write_back
	{
		CacheInit(1, 8, 1, 1, 2, 0);
		CacheExec(trace);
		PrintOutput();
	}
	// 2KB 8B direct write_through
	{
		CacheInit(2, 8, 1, 1, 1, 0);
		CacheExec(trace);
		PrintOutput();
	}
	// 4KB 8B direct write_through
	{
		CacheInit(4, 8, 1, 1, 1, 0);
		CacheExec(trace);
		PrintOutput();
	}
}
void set_asso(char *trace)
{
	// 1KB 4B 2 way LRU write_through
	{
		CacheInit(1, 4, 2, 2, 1, 2);
		CacheExec(trace);
		PrintOutput();
	}
	// 1KB 4B 2 way random write_through
	{
		CacheInit(1, 4, 2, 2, 1, 4);
		CacheExec(trace);
		PrintOutput();
	}
	// 2KB 4B 2 way LRU write_back
	{
		CacheInit(2, 4, 2, 2, 2, 2);
		CacheExec(trace);
		PrintOutput();
	}
	// 2KB 4B 2 way random write_back
	{
		CacheInit(2, 4, 2, 2, 2, 4);
		CacheExec(trace);
		PrintOutput();
	}
	// 2KB 4B 4 way FIFO write_through
	{
		CacheInit(2, 4, 2, 4, 1, 1);
		CacheExec(trace);
		PrintOutput();
	}
	// 2KB 4B 4 way random write_through
	{
		CacheInit(2, 4, 2, 4, 1, 4);
		CacheExec(trace);
		PrintOutput();
	}
}
void full_asso(char *trace)
{
	// 1KB 4B full LRU write_through
	{
		CacheInit(1, 4, 3, 1, 1, 2);
		CacheExec(trace);
		PrintOutput();
	}
	// 1KB 4B full random write_back
	{
		CacheInit(1, 4, 3, 1, 2, 4);
		CacheExec(trace);
		PrintOutput();
	}
	// 1KB 8B full FIFO write_through
	{
		CacheInit(1, 8, 3, 1, 1, 1);
		CacheExec(trace);
		PrintOutput();
	}
	// 2KB 4B full LRU write_back
	{
		CacheInit(2, 4, 3, 1, 2, 2);
		CacheExec(trace);
		PrintOutput();
	}
	// 4KB 8B full LRU write_through
	{
		CacheInit(4, 8, 3, 1, 1, 2);
		CacheExec(trace);
		PrintOutput();
	}
}
int main(int argc, char *argv[])
{
	direct_asso(argv[1]);
	return 0;
}