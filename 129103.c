int __audit_mq_notify(mqd_t mqdes, const struct sigevent __user *u_notification)
{
	struct audit_aux_data_mq_notify *ax;
	struct audit_context *context = current->audit_context;

	if (!audit_enabled)
		return 0;

	if (likely(!context))
		return 0;

	ax = kmalloc(sizeof(*ax), GFP_ATOMIC);
	if (!ax)
		return -ENOMEM;

	if (u_notification != NULL) {
		if (copy_from_user(&ax->notification, u_notification, sizeof(ax->notification))) {
			kfree(ax);
			return -EFAULT;
		}
	} else
		memset(&ax->notification, 0, sizeof(ax->notification));

	ax->mqdes = mqdes;

	ax->d.type = AUDIT_MQ_NOTIFY;
	ax->d.next = context->aux;
	context->aux = (void *)ax;
	return 0;
}