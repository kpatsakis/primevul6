static void __net_exit nf_tables_pre_exit_net(struct net *net)
{
	struct nftables_pernet *nft_net = nft_pernet(net);

	mutex_lock(&nft_net->commit_mutex);
	__nft_release_hooks(net);
	mutex_unlock(&nft_net->commit_mutex);
}