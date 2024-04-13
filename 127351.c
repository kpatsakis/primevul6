void arch_setup_new_exec(void)
{
	/* If cpuid was previously disabled for this task, re-enable it. */
	if (test_thread_flag(TIF_NOCPUID))
		enable_cpuid();

	/*
	 * Don't inherit TIF_SSBD across exec boundary when
	 * PR_SPEC_DISABLE_NOEXEC is used.
	 */
	if (test_thread_flag(TIF_SSBD) &&
	    task_spec_ssb_noexec(current)) {
		clear_thread_flag(TIF_SSBD);
		task_clear_spec_ssb_disable(current);
		task_clear_spec_ssb_noexec(current);
		speculation_ctrl_update(task_thread_info(current)->flags);
	}
}