
static void *net_grab_current_ns(void)
{
	struct net *ns = current->nsproxy->net_ns;
#ifdef CONFIG_NET_NS
	if (ns)
		refcount_inc(&ns->passive);
#endif
	return ns;