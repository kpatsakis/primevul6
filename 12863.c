static int smack_socket_getpeersec_dgram(struct socket *sock,
					 struct sk_buff *skb, u32 *secid)

{
	struct socket_smack *ssp = NULL;
	struct smack_known *skp;
	struct sock *sk = NULL;
	int family = PF_UNSPEC;
	u32 s = 0;	/* 0 is the invalid secid */

	if (skb != NULL) {
		if (skb->protocol == htons(ETH_P_IP))
			family = PF_INET;
#if IS_ENABLED(CONFIG_IPV6)
		else if (skb->protocol == htons(ETH_P_IPV6))
			family = PF_INET6;
#endif /* CONFIG_IPV6 */
	}
	if (family == PF_UNSPEC && sock != NULL)
		family = sock->sk->sk_family;

	switch (family) {
	case PF_UNIX:
		ssp = sock->sk->sk_security;
		s = ssp->smk_out->smk_secid;
		break;
	case PF_INET:
		skp = smack_from_skb(skb);
		if (skp) {
			s = skp->smk_secid;
			break;
		}
		/*
		 * Translate what netlabel gave us.
		 */
		if (sock != NULL)
			sk = sock->sk;
		skp = smack_from_netlbl(sk, family, skb);
		if (skp != NULL)
			s = skp->smk_secid;
		break;
	case PF_INET6:
#ifdef SMACK_IPV6_SECMARK_LABELING
		skp = smack_from_skb(skb);
		if (skp)
			s = skp->smk_secid;
#endif
		break;
	}
	*secid = s;
	if (s == 0)
		return -EINVAL;
	return 0;
}