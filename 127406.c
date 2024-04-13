static __cpuidle void mwait_idle(void)
{
	if (!current_set_polling_and_test()) {
		trace_cpu_idle_rcuidle(1, smp_processor_id());
		if (this_cpu_has(X86_BUG_CLFLUSH_MONITOR)) {
			mb(); /* quirk */
			clflush((void *)&current_thread_info()->flags);
			mb(); /* quirk */
		}

		__monitor((void *)&current_thread_info()->flags, 0, 0);
		if (!need_resched())
			__sti_mwait(0, 0);
		else
			local_irq_enable();
		trace_cpu_idle_rcuidle(PWR_EVENT_EXIT, smp_processor_id());
	} else {
		local_irq_enable();
	}
	__current_clr_polling();
}