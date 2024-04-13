int __audit_mq_getsetattr(mqd_t mqdes, struct mq_attr *mqstat)
{
	struct audit_aux_data_mq_getsetattr *ax;
	struct audit_context *context = current->audit_context;

	if (!audit_enabled)
		return 0;

	if (likely(!context))
		return 0;

	ax = kmalloc(sizeof(*ax), GFP_ATOMIC);
	if (!ax)
		return -ENOMEM;

	ax->mqdes = mqdes;
	ax->mqstat = *mqstat;

	ax->d.type = AUDIT_MQ_GETSETATTR;
	ax->d.next = context->aux;
	context->aux = (void *)ax;
	return 0;
}