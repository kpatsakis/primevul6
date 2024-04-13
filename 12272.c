int rose_rt_ioctl(unsigned int cmd, void __user *arg)
{
	struct rose_route_struct rose_route;
	struct net_device *dev;
	int err;

	switch (cmd) {
	case SIOCADDRT:
		if (copy_from_user(&rose_route, arg, sizeof(struct rose_route_struct)))
			return -EFAULT;
		if ((dev = rose_ax25_dev_find(rose_route.device)) == NULL)
			return -EINVAL;
		if (rose_dev_exists(&rose_route.address)) /* Can't add routes to ourself */
			return -EINVAL;
		if (rose_route.mask > 10) /* Mask can't be more than 10 digits */
			return -EINVAL;
		if (rose_route.ndigis > AX25_MAX_DIGIS)
			return -EINVAL;
		err = rose_add_node(&rose_route, dev);
		return err;

	case SIOCDELRT:
		if (copy_from_user(&rose_route, arg, sizeof(struct rose_route_struct)))
			return -EFAULT;
		if ((dev = rose_ax25_dev_find(rose_route.device)) == NULL)
			return -EINVAL;
		err = rose_del_node(&rose_route, dev);
		return err;

	case SIOCRSCLRRT:
		return rose_clear_routes();

	default:
		return -EINVAL;
	}

	return 0;
}