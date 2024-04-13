static inline void totalram_pages_set(long val)
{
	atomic_long_set(&_totalram_pages, val);
}