static void __nft_release_hook(struct net *net, struct nft_table *table)
{
	struct nft_flowtable *flowtable;
	struct nft_chain *chain;

	list_for_each_entry(chain, &table->chains, list)
		__nf_tables_unregister_hook(net, table, chain, true);
	list_for_each_entry(flowtable, &table->flowtables, list)
		__nft_unregister_flowtable_net_hooks(net, &flowtable->hook_list,
						     true);
}