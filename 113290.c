static inline int cpupid_to_pid(int cpupid)
{
	return cpupid & LAST__PID_MASK;
}