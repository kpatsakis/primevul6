int audit_sockaddr(int len, void *a)
{
	struct audit_aux_data_sockaddr *ax;
	struct audit_context *context = current->audit_context;

	if (likely(!context || context->dummy))
		return 0;

	ax = kmalloc(sizeof(*ax) + len, GFP_KERNEL);
	if (!ax)
		return -ENOMEM;

	ax->len = len;
	memcpy(ax->a, a, len);

	ax->d.type = AUDIT_SOCKADDR;
	ax->d.next = context->aux;
	context->aux = (void *)ax;
	return 0;
}