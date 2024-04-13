
static ssize_t iflink_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct net_device *ndev = to_net_dev(dev);

	return sprintf(buf, fmt_dec, dev_get_iflink(ndev));