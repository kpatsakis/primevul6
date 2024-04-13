static void nf_tables_unregister_hook(struct net *net,
				      const struct nft_table *table,
				      struct nft_chain *chain)
{
	return __nf_tables_unregister_hook(net, table, chain, false);
}