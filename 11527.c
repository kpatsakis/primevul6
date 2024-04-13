static int fastrpc_invoke(struct fastrpc_user *fl, char __user *argp)
{
	struct fastrpc_invoke_args *args = NULL;
	struct fastrpc_invoke inv;
	u32 nscalars;
	int err;

	if (copy_from_user(&inv, argp, sizeof(inv)))
		return -EFAULT;

	/* nscalars is truncated here to max supported value */
	nscalars = REMOTE_SCALARS_LENGTH(inv.sc);
	if (nscalars) {
		args = kcalloc(nscalars, sizeof(*args), GFP_KERNEL);
		if (!args)
			return -ENOMEM;

		if (copy_from_user(args, (void __user *)(uintptr_t)inv.args,
				   nscalars * sizeof(*args))) {
			kfree(args);
			return -EFAULT;
		}
	}

	err = fastrpc_internal_invoke(fl, false, inv.handle, inv.sc, args);
	kfree(args);

	return err;
}