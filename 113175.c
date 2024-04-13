static inline void pgtable_init(void)
{
	ptlock_cache_init();
	pgtable_cache_init();
}