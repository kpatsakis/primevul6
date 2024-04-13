void audit_syscall_exit(int valid, long return_code)
{
	struct task_struct *tsk = current;
	struct audit_context *context;

	context = audit_get_context(tsk, valid, return_code);

	if (likely(!context))
		return;

	if (context->in_syscall && context->auditable)
		audit_log_exit(context, tsk);

	context->in_syscall = 0;
	context->auditable  = 0;

	if (context->previous) {
		struct audit_context *new_context = context->previous;
		context->previous  = NULL;
		audit_free_context(context);
		tsk->audit_context = new_context;
	} else {
		audit_free_names(context);
		audit_free_aux(context);
		context->aux = NULL;
		context->aux_pids = NULL;
		context->target_pid = 0;
		context->target_sid = 0;
		kfree(context->filterkey);
		context->filterkey = NULL;
		tsk->audit_context = context;
	}
}