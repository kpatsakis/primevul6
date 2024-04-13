int audit_avc_path(struct dentry *dentry, struct vfsmount *mnt)
{
	struct audit_aux_data_path *ax;
	struct audit_context *context = current->audit_context;

	if (likely(!context))
		return 0;

	ax = kmalloc(sizeof(*ax), GFP_ATOMIC);
	if (!ax)
		return -ENOMEM;

	ax->dentry = dget(dentry);
	ax->mnt = mntget(mnt);

	ax->d.type = AUDIT_AVC_PATH;
	ax->d.next = context->aux;
	context->aux = (void *)ax;
	return 0;
}