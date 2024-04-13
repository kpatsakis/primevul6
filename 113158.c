static inline int cpupid_to_nid(int cpupid)
{
	return cpu_to_node(cpupid_to_cpu(cpupid));
}