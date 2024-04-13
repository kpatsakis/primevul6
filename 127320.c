static void amd_e400_idle(void)
{
	/*
	 * We cannot use static_cpu_has_bug() here because X86_BUG_AMD_APIC_C1E
	 * gets set after static_cpu_has() places have been converted via
	 * alternatives.
	 */
	if (!boot_cpu_has_bug(X86_BUG_AMD_APIC_C1E)) {
		default_idle();
		return;
	}

	tick_broadcast_enter();

	default_idle();

	/*
	 * The switch back from broadcast mode needs to be called with
	 * interrupts disabled.
	 */
	local_irq_disable();
	tick_broadcast_exit();
	local_irq_enable();
}