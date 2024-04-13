/* use same locking rules as GIFHWADDR ioctl's */
static ssize_t address_show(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	struct net_device *ndev = to_net_dev(dev);
	ssize_t ret = -EINVAL;

	read_lock(&dev_base_lock);
	if (dev_isalive(ndev))
		ret = sysfs_format_mac(buf, ndev->dev_addr, ndev->addr_len);
	read_unlock(&dev_base_lock);
	return ret;