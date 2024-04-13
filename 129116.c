int audit_socketcall(int nargs, unsigned long *args)
{
	struct audit_aux_data_socketcall *ax;
	struct audit_context *context = current->audit_context;

	if (likely(!context || context->dummy))
		return 0;

	ax = kmalloc(sizeof(*ax) + nargs * sizeof(unsigned long), GFP_KERNEL);
	if (!ax)
		return -ENOMEM;

	ax->nargs = nargs;
	memcpy(ax->args, args, nargs * sizeof(unsigned long));

	ax->d.type = AUDIT_SOCKETCALL;
	ax->d.next = context->aux;
	context->aux = (void *)ax;
	return 0;
}