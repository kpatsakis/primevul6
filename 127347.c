void __init arch_post_acpi_subsys_init(void)
{
	u32 lo, hi;

	if (!boot_cpu_has_bug(X86_BUG_AMD_E400))
		return;

	/*
	 * AMD E400 detection needs to happen after ACPI has been enabled. If
	 * the machine is affected K8_INTP_C1E_ACTIVE_MASK bits are set in
	 * MSR_K8_INT_PENDING_MSG.
	 */
	rdmsr(MSR_K8_INT_PENDING_MSG, lo, hi);
	if (!(lo & K8_INTP_C1E_ACTIVE_MASK))
		return;

	boot_cpu_set_bug(X86_BUG_AMD_APIC_C1E);

	if (!boot_cpu_has(X86_FEATURE_NONSTOP_TSC))
		mark_tsc_unstable("TSC halt in AMD C1E");
	pr_info("System has AMD C1E enabled\n");
}