static int prefer_mwait_c1_over_halt(const struct cpuinfo_x86 *c)
{
	if (c->x86_vendor != X86_VENDOR_INTEL)
		return 0;

	if (!cpu_has(c, X86_FEATURE_MWAIT) || boot_cpu_has_bug(X86_BUG_MONITOR))
		return 0;

	return 1;
}