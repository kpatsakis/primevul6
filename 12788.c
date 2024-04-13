static struct smack_known *smack_from_netlbl(const struct sock *sk, u16 family,
					     struct sk_buff *skb)
{
	struct netlbl_lsm_secattr secattr;
	struct socket_smack *ssp = NULL;
	struct smack_known *skp = NULL;

	netlbl_secattr_init(&secattr);

	if (sk)
		ssp = sk->sk_security;

	if (netlbl_skbuff_getattr(skb, family, &secattr) == 0) {
		skp = smack_from_secattr(&secattr, ssp);
		if (secattr.flags & NETLBL_SECATTR_CACHEABLE)
			netlbl_cache_add(skb, family, &skp->smk_netlabel);
	}

	netlbl_secattr_destroy(&secattr);

	return skp;
}