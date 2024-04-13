int audit_set_loginuid(struct task_struct *task, uid_t loginuid)
{
	struct audit_context *context = task->audit_context;

	if (context) {
		/* Only log if audit is enabled */
		if (context->in_syscall) {
			struct audit_buffer *ab;

			ab = audit_log_start(NULL, GFP_KERNEL, AUDIT_LOGIN);
			if (ab) {
				audit_log_format(ab, "login pid=%d uid=%u "
					"old auid=%u new auid=%u",
					task->pid, task->uid,
					context->loginuid, loginuid);
				audit_log_end(ab);
			}
		}
		context->loginuid = loginuid;
	}
	return 0;
}