static void __init xen_init_capabilities(void)
{
	setup_force_cpu_cap(X86_FEATURE_XENPV);
	setup_clear_cpu_cap(X86_FEATURE_DCA);
	setup_clear_cpu_cap(X86_FEATURE_APERFMPERF);
	setup_clear_cpu_cap(X86_FEATURE_MTRR);
	setup_clear_cpu_cap(X86_FEATURE_ACC);
	setup_clear_cpu_cap(X86_FEATURE_X2APIC);
	setup_clear_cpu_cap(X86_FEATURE_SME);

	/*
	 * Xen PV would need some work to support PCID: CR3 handling as well
	 * as xen_flush_tlb_others() would need updating.
	 */
	setup_clear_cpu_cap(X86_FEATURE_PCID);

	if (!xen_initial_domain())
		setup_clear_cpu_cap(X86_FEATURE_ACPI);

	if (xen_check_mwait())
		setup_force_cpu_cap(X86_FEATURE_MWAIT);
	else
		setup_clear_cpu_cap(X86_FEATURE_MWAIT);

	if (!xen_check_xsave()) {
		setup_clear_cpu_cap(X86_FEATURE_XSAVE);
		setup_clear_cpu_cap(X86_FEATURE_OSXSAVE);
	}
}