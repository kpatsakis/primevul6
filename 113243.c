static inline void totalram_pages_add(long count)
{
	atomic_long_add(count, &_totalram_pages);
}