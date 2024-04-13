get_arg(void __user *b, void *val, int len)
{
	if (len <= 0)
		len = sizeof(void *);
	if (copy_from_user(val, b, len))
		return -EFAULT;
	return 0;
}