static void enable_cpuid(void)
{
	preempt_disable();
	if (test_and_clear_thread_flag(TIF_NOCPUID)) {
		/*
		 * Must flip the CPU state synchronously with
		 * TIF_NOCPUID in the current running context.
		 */
		set_cpuid_faulting(false);
	}
	preempt_enable();
}