
void netdev_class_remove_file_ns(const struct class_attribute *class_attr,
				 const void *ns)
{
	class_remove_file_ns(&net_class, class_attr, ns);