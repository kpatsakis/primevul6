static inline void *kvmalloc(size_t size, gfp_t flags)
{
	return kvmalloc_node(size, flags, NUMA_NO_NODE);
}