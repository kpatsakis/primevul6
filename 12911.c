static inline unsigned int smk_ptrace_mode(unsigned int mode)
{
	if (mode & PTRACE_MODE_ATTACH)
		return MAY_READWRITE;
	if (mode & PTRACE_MODE_READ)
		return MAY_READ;

	return 0;
}