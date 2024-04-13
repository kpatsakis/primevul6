static void xen_write_cr0(unsigned long cr0)
{
	struct multicall_space mcs;

	this_cpu_write(xen_cr0_value, cr0);

	/* Only pay attention to cr0.TS; everything else is
	   ignored. */
	mcs = xen_mc_entry(0);

	MULTI_fpu_taskswitch(mcs.mc, (cr0 & X86_CR0_TS) != 0);

	xen_mc_issue(PARAVIRT_LAZY_CPU);
}