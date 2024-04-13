void audit_free(struct task_struct *tsk)
{
	struct audit_context *context;

	context = audit_get_context(tsk, 0, 0);
	if (likely(!context))
		return;

	/* Check for system calls that do not go through the exit
	 * function (e.g., exit_group), then free context block. 
	 * We use GFP_ATOMIC here because we might be doing this 
	 * in the context of the idle thread */
	/* that can happen only if we are called from do_exit() */
	if (context->in_syscall && context->auditable)
		audit_log_exit(context, tsk);

	audit_free_context(context);
}