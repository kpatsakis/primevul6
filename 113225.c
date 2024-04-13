static inline struct mem_cgroup *page_memcg(struct page *page)
{
	return page->mem_cgroup;
}