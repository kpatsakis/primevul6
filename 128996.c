int audit_bprm(struct linux_binprm *bprm)
{
	struct audit_aux_data_execve *ax;
	struct audit_context *context = current->audit_context;

	if (likely(!audit_enabled || !context || context->dummy))
		return 0;

	/*
	 * Even though the stack code doesn't limit the arg+env size any more,
	 * the audit code requires that _all_ arguments be logged in a single
	 * netlink skb. Hence cap it :-(
	 */
	if (bprm->argv_len > (audit_argv_kb << 10))
		return -E2BIG;

	ax = kmalloc(sizeof(*ax), GFP_KERNEL);
	if (!ax)
		return -ENOMEM;

	ax->argc = bprm->argc;
	ax->envc = bprm->envc;
	ax->mm = bprm->mm;
	ax->d.type = AUDIT_EXECVE;
	ax->d.next = context->aux;
	context->aux = (void *)ax;
	return 0;
}