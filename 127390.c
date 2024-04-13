static void xen_write_msr(unsigned int msr, unsigned low, unsigned high)
{
	/*
	 * This will silently swallow a #GP from WRMSR.  It may be worth
	 * changing that.
	 */
	xen_write_msr_safe(msr, low, high);
}