static inline struct audit_context *audit_get_context(struct task_struct *tsk,
						      int return_valid,
						      int return_code)
{
	struct audit_context *context = tsk->audit_context;

	if (likely(!context))
		return NULL;
	context->return_valid = return_valid;
	context->return_code  = return_code;

	if (context->in_syscall && !context->dummy && !context->auditable) {
		enum audit_state state;

		state = audit_filter_syscall(tsk, context, &audit_filter_list[AUDIT_FILTER_EXIT]);
		if (state == AUDIT_RECORD_CONTEXT) {
			context->auditable = 1;
			goto get_context;
		}

		state = audit_filter_inodes(tsk, context);
		if (state == AUDIT_RECORD_CONTEXT)
			context->auditable = 1;

	}

get_context:

	tsk->audit_context = NULL;
	return context;
}