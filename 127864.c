isdn_ppp_dial_slave(char *name)
{
#ifdef CONFIG_ISDN_MPP
	isdn_net_dev *ndev;
	isdn_net_local *lp;
	struct net_device *sdev;

	if (!(ndev = isdn_net_findif(name)))
		return 1;
	lp = ndev->local;
	if (!(lp->flags & ISDN_NET_CONNECTED))
		return 5;

	sdev = lp->slave;
	while (sdev) {
		isdn_net_local *mlp = netdev_priv(sdev);
		if (!(mlp->flags & ISDN_NET_CONNECTED))
			break;
		sdev = mlp->slave;
	}
	if (!sdev)
		return 2;

	isdn_net_dial_req(netdev_priv(sdev));
	return 0;
#else
	return -1;
#endif
}