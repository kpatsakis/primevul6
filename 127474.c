void stop_this_cpu(void *dummy)
{
	local_irq_disable();
	/*
	 * Remove this CPU:
	 */
	set_cpu_online(smp_processor_id(), false);
	disable_local_APIC();
	mcheck_cpu_clear(this_cpu_ptr(&cpu_info));

	/*
	 * Use wbinvd on processors that support SME. This provides support
	 * for performing a successful kexec when going from SME inactive
	 * to SME active (or vice-versa). The cache must be cleared so that
	 * if there are entries with the same physical address, both with and
	 * without the encryption bit, they don't race each other when flushed
	 * and potentially end up with the wrong entry being committed to
	 * memory.
	 */
	if (boot_cpu_has(X86_FEATURE_SME))
		native_wbinvd();
	for (;;) {
		/*
		 * Use native_halt() so that memory contents don't change
		 * (stack usage and variables) after possibly issuing the
		 * native_wbinvd() above.
		 */
		native_halt();
	}
}