static void __nft_unregister_flowtable_net_hooks(struct net *net,
						 struct list_head *hook_list,
					         bool release_netdev)
{
	struct nft_hook *hook, *next;

	list_for_each_entry_safe(hook, next, hook_list, list) {
		nf_unregister_net_hook(net, &hook->ops);
		if (release_netdev) {
			list_del(&hook->list);
			kfree_rcu(hook, rcu);
		}
	}
}