static inline bool cpupid_pid_unset(int cpupid)
{
	return cpupid_to_pid(cpupid) == (-1 & LAST__PID_MASK);
}