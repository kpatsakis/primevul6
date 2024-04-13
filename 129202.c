int __audit_mq_timedreceive(mqd_t mqdes, size_t msg_len,
				unsigned int __user *u_msg_prio,
				const struct timespec __user *u_abs_timeout)
{
	struct audit_aux_data_mq_sendrecv *ax;
	struct audit_context *context = current->audit_context;

	if (!audit_enabled)
		return 0;

	if (likely(!context))
		return 0;

	ax = kmalloc(sizeof(*ax), GFP_ATOMIC);
	if (!ax)
		return -ENOMEM;

	if (u_msg_prio != NULL) {
		if (get_user(ax->msg_prio, u_msg_prio)) {
			kfree(ax);
			return -EFAULT;
		}
	} else
		ax->msg_prio = 0;

	if (u_abs_timeout != NULL) {
		if (copy_from_user(&ax->abs_timeout, u_abs_timeout, sizeof(ax->abs_timeout))) {
			kfree(ax);
			return -EFAULT;
		}
	} else
		memset(&ax->abs_timeout, 0, sizeof(ax->abs_timeout));

	ax->mqdes = mqdes;
	ax->msg_len = msg_len;

	ax->d.type = AUDIT_MQ_SENDRECV;
	ax->d.next = context->aux;
	context->aux = (void *)ax;
	return 0;
}