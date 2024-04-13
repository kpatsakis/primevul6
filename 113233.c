static inline int cpupid_to_cpu(int cpupid)
{
	return (cpupid >> LAST__PID_SHIFT) & LAST__CPU_MASK;
}