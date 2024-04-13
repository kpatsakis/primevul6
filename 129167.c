int __audit_ipc_set_perm(unsigned long qbytes, uid_t uid, gid_t gid, mode_t mode)
{
	struct audit_aux_data_ipcctl *ax;
	struct audit_context *context = current->audit_context;

	ax = kmalloc(sizeof(*ax), GFP_ATOMIC);
	if (!ax)
		return -ENOMEM;

	ax->qbytes = qbytes;
	ax->uid = uid;
	ax->gid = gid;
	ax->mode = mode;

	ax->d.type = AUDIT_IPC_SET_PERM;
	ax->d.next = context->aux;
	context->aux = (void *)ax;
	return 0;
}