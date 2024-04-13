static __net_init int ppp_init_net(struct net *net)
{
	struct ppp_net *pn = net_generic(net, ppp_net_id);

	idr_init(&pn->units_idr);
	mutex_init(&pn->all_ppp_mutex);

	INIT_LIST_HEAD(&pn->all_channels);
	INIT_LIST_HEAD(&pn->new_channels);

	spin_lock_init(&pn->all_channels_lock);

	return 0;
}