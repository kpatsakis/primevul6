static inline long get_user_pages_longterm(unsigned long start,
		unsigned long nr_pages, unsigned int gup_flags,
		struct page **pages, struct vm_area_struct **vmas)
{
	return get_user_pages(start, nr_pages, gup_flags, pages, vmas);
}