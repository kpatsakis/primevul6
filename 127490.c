static void set_cpuid_faulting(bool on)
{
	u64 msrval;

	msrval = this_cpu_read(msr_misc_features_shadow);
	msrval &= ~MSR_MISC_FEATURES_ENABLES_CPUID_FAULT;
	msrval |= (on << MSR_MISC_FEATURES_ENABLES_CPUID_FAULT_BIT);
	this_cpu_write(msr_misc_features_shadow, msrval);
	wrmsrl(MSR_MISC_FEATURES_ENABLES, msrval);
}