void __audit_ptrace(struct task_struct *t)
{
	struct audit_context *context = current->audit_context;

	context->target_pid = t->pid;
	selinux_get_task_sid(t, &context->target_sid);
}