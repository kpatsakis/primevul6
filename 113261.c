static inline void totalram_pages_inc(void)
{
	atomic_long_inc(&_totalram_pages);
}