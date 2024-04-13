static inline int cpu_pid_to_cpupid(int cpu, int pid)
{
	return ((cpu & LAST__CPU_MASK) << LAST__PID_SHIFT) | (pid & LAST__PID_MASK);
}