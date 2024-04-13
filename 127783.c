isdn_ppp_dev_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	int error = 0;
	int len;
	isdn_net_local *lp = netdev_priv(dev);


	if (lp->p_encap != ISDN_NET_ENCAP_SYNCPPP)
		return -EINVAL;

	switch (cmd) {
#define PPP_VERSION "2.3.7"
	case SIOCGPPPVER:
		len = strlen(PPP_VERSION) + 1;
		if (copy_to_user(ifr->ifr_data, PPP_VERSION, len))
			error = -EFAULT;
		break;

	case SIOCGPPPSTATS:
		error = isdn_ppp_dev_ioctl_stats(lp->ppp_slot, ifr, dev);
		break;
	default:
		error = -EINVAL;
		break;
	}
	return error;
}