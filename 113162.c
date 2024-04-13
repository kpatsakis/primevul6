static inline unsigned long totalram_pages(void)
{
	return (unsigned long)atomic_long_read(&_totalram_pages);
}