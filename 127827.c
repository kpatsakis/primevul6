set_arg(void __user *b, void *val, int len)
{
	if (len <= 0)
		len = sizeof(void *);
	if (copy_to_user(b, val, len))
		return -EFAULT;
	return 0;
}