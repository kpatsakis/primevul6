isdn_ppp_hangup_slave(char *name)
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

		if (mlp->slave) { /* find last connected link in chain */
			isdn_net_local *nlp = ISDN_SLAVE_PRIV(mlp);

			if (!(nlp->flags & ISDN_NET_CONNECTED))
				break;
		} else if (mlp->flags & ISDN_NET_CONNECTED)
			break;

		sdev = mlp->slave;
	}
	if (!sdev)
		return 2;

	isdn_net_hangup(sdev);
	return 0;
#else
	return -1;
#endif
}