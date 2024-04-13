 */
struct net_device *of_find_net_device_by_node(struct device_node *np)
{
	struct device *dev;

	dev = class_find_device(&net_class, NULL, np, of_dev_node_match);
	if (!dev)
		return NULL;

	return to_net_dev(dev);