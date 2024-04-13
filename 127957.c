
static void netdev_queue_get_ownership(struct kobject *kobj,
				       kuid_t *uid, kgid_t *gid)
{
	const struct net *net = netdev_queue_namespace(kobj);

	net_ns_get_ownership(net, uid, gid);