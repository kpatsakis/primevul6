#ifdef CONFIG_OF_NET
static int of_dev_node_match(struct device *dev, const void *data)
{
	int ret = 0;

	if (dev->parent)
		ret = dev->parent->of_node == data;

	return ret == 0 ? dev->of_node == data : ret;