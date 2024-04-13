
int netdev_class_create_file_ns(const struct class_attribute *class_attr,
				const void *ns)
{
	return class_create_file_ns(&net_class, class_attr, ns);