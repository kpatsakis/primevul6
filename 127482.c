void flush_thread(void)
{
	struct task_struct *tsk = current;

	flush_ptrace_hw_breakpoint(tsk);
	memset(tsk->thread.tls_array, 0, sizeof(tsk->thread.tls_array));

	fpu__clear_all(&tsk->thread.fpu);
}