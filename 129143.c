int __audit_fd_pair(int fd1, int fd2)
{
	struct audit_context *context = current->audit_context;
	struct audit_aux_data_fd_pair *ax;

	if (likely(!context)) {
		return 0;
	}

	ax = kmalloc(sizeof(*ax), GFP_KERNEL);
	if (!ax) {
		return -ENOMEM;
	}

	ax->fd[0] = fd1;
	ax->fd[1] = fd2;

	ax->d.type = AUDIT_FD_PAIR;
	ax->d.next = context->aux;
	context->aux = (void *)ax;
	return 0;
}