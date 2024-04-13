int __audit_ipc_obj(struct kern_ipc_perm *ipcp)
{
	struct audit_aux_data_ipcctl *ax;
	struct audit_context *context = current->audit_context;

	ax = kmalloc(sizeof(*ax), GFP_ATOMIC);
	if (!ax)
		return -ENOMEM;

	ax->uid = ipcp->uid;
	ax->gid = ipcp->gid;
	ax->mode = ipcp->mode;
	selinux_get_ipc_sid(ipcp, &ax->osid);

	ax->d.type = AUDIT_IPC;
	ax->d.next = context->aux;
	context->aux = (void *)ax;
	return 0;
}