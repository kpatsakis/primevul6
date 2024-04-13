static inline int rdmsrl_safe(unsigned msr, unsigned long long *p)
{
	int err;

	*p = paravirt_read_msr_safe(msr, &err);
	return err;
}