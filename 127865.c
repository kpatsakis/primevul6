static __net_exit void ppp_exit_net(struct net *net)
{
	struct ppp_net *pn = net_generic(net, ppp_net_id);
	struct net_device *dev;
	struct net_device *aux;
	struct ppp *ppp;
	LIST_HEAD(list);
	int id;

	rtnl_lock();
	for_each_netdev_safe(net, dev, aux) {
		if (dev->netdev_ops == &ppp_netdev_ops)
			unregister_netdevice_queue(dev, &list);
	}

	idr_for_each_entry(&pn->units_idr, ppp, id)
		/* Skip devices already unregistered by previous loop */
		if (!net_eq(dev_net(ppp->dev), net))
			unregister_netdevice_queue(ppp->dev, &list);

	unregister_netdevice_many(&list);
	rtnl_unlock();

	idr_destroy(&pn->units_idr);
}