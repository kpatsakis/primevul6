
int __init netdev_kobject_init(void)
{
	kobj_ns_type_register(&net_ns_type_operations);
	return class_register(&net_class);