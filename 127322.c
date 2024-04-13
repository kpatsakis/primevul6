void amd_e400_c1e_apic_setup(void)
{
	if (boot_cpu_has_bug(X86_BUG_AMD_APIC_C1E)) {
		pr_info("Switch to broadcast mode on CPU%d\n", smp_processor_id());
		local_irq_disable();
		tick_broadcast_force();
		local_irq_enable();
	}
}