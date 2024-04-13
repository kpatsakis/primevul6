
static ssize_t format_name_assign_type(const struct net_device *dev, char *buf)
{
	return sprintf(buf, fmt_dec, dev->name_assign_type);