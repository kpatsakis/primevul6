static int aac_send_reset_adapter(struct aac_dev *dev, void __user *arg)
{
	struct aac_reset_iop reset;
	int retval;

	if (copy_from_user((void *)&reset, arg, sizeof(struct aac_reset_iop)))
		return -EFAULT;

	retval = aac_reset_adapter(dev, 0, reset.reset_type);
	return retval;

}