static u64 xen_read_msr(unsigned int msr)
{
	/*
	 * This will silently swallow a #GP from RDMSR.  It may be worth
	 * changing that.
	 */
	int err;

	return xen_read_msr_safe(msr, &err);
}