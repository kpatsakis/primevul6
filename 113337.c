static inline struct mem_cgroup *page_memcg_rcu(struct page *page)
{
	WARN_ON_ONCE(!rcu_read_lock_held());
	return NULL;
}