xfs_ioc_attrmulti_one(
	struct file		*parfilp,
	struct inode		*inode,
	uint32_t		opcode,
	void __user		*uname,
	void __user		*value,
	uint32_t		*len,
	uint32_t		flags)
{
	unsigned char		*name;
	int			error;

	if ((flags & XFS_IOC_ATTR_ROOT) && (flags & XFS_IOC_ATTR_SECURE))
		return -EINVAL;

	name = strndup_user(uname, MAXNAMELEN);
	if (IS_ERR(name))
		return PTR_ERR(name);

	switch (opcode) {
	case ATTR_OP_GET:
		error = xfs_attrmulti_attr_get(inode, name, value, len, flags);
		break;
	case ATTR_OP_REMOVE:
		value = NULL;
		*len = 0;
		fallthrough;
	case ATTR_OP_SET:
		error = mnt_want_write_file(parfilp);
		if (error)
			break;
		error = xfs_attrmulti_attr_set(inode, name, value, *len, flags);
		mnt_drop_write_file(parfilp);
		break;
	default:
		error = -EINVAL;
		break;
	}

	kfree(name);
	return error;
}