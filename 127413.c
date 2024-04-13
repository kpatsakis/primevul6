static void xen_load_sp0(unsigned long sp0)
{
	struct multicall_space mcs;

	mcs = xen_mc_entry(0);
	MULTI_stack_switch(mcs.mc, __KERNEL_DS, sp0);
	xen_mc_issue(PARAVIRT_LAZY_CPU);
	this_cpu_write(cpu_tss_rw.x86_tss.sp0, sp0);
}