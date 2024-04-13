static inline bool cpupid_cpu_unset(int cpupid)
{
	return cpupid_to_cpu(cpupid) == (-1 & LAST__CPU_MASK);
}