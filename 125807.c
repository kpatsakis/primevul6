static void nft_unregister_flowtable_net_hooks(struct net *net,
					       struct list_head *hook_list)
{
	__nft_unregister_flowtable_net_hooks(net, hook_list, false);
}