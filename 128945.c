int __audit_mq_open(int oflag, mode_t mode, struct mq_attr __user *u_attr)
{
	struct audit_aux_data_mq_open *ax;
	struct audit_context *context = current->audit_context;

	if (!audit_enabled)
		return 0;

	if (likely(!context))
		return 0;

	ax = kmalloc(sizeof(*ax), GFP_ATOMIC);
	if (!ax)
		return -ENOMEM;

	if (u_attr != NULL) {
		if (copy_from_user(&ax->attr, u_attr, sizeof(ax->attr))) {
			kfree(ax);
			return -EFAULT;
		}
	} else
		memset(&ax->attr, 0, sizeof(ax->attr));

	ax->oflag = oflag;
	ax->mode = mode;

	ax->d.type = AUDIT_MQ_OPEN;
	ax->d.next = context->aux;
	context->aux = (void *)ax;
	return 0;
}