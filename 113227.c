static inline void totalram_pages_dec(void)
{
	atomic_long_dec(&_totalram_pages);
}