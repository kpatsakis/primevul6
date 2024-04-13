
static bool net_current_may_mount(void)
{
	struct net *net = current->nsproxy->net_ns;

	return ns_capable(net->user_ns, CAP_SYS_ADMIN);