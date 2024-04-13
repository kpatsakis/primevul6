static struct smack_known *smack_from_secattr(struct netlbl_lsm_secattr *sap,
						struct socket_smack *ssp)
{
	struct smack_known *skp;
	int found = 0;
	int acat;
	int kcat;

	/*
	 * Netlabel found it in the cache.
	 */
	if ((sap->flags & NETLBL_SECATTR_CACHE) != 0)
		return (struct smack_known *)sap->cache->data;

	if ((sap->flags & NETLBL_SECATTR_SECID) != 0)
		/*
		 * Looks like a fallback, which gives us a secid.
		 */
		return smack_from_secid(sap->attr.secid);

	if ((sap->flags & NETLBL_SECATTR_MLS_LVL) != 0) {
		/*
		 * Looks like a CIPSO packet.
		 * If there are flags but no level netlabel isn't
		 * behaving the way we expect it to.
		 *
		 * Look it up in the label table
		 * Without guidance regarding the smack value
		 * for the packet fall back on the network
		 * ambient value.
		 */
		rcu_read_lock();
		list_for_each_entry_rcu(skp, &smack_known_list, list) {
			if (sap->attr.mls.lvl != skp->smk_netlabel.attr.mls.lvl)
				continue;
			/*
			 * Compare the catsets. Use the netlbl APIs.
			 */
			if ((sap->flags & NETLBL_SECATTR_MLS_CAT) == 0) {
				if ((skp->smk_netlabel.flags &
				     NETLBL_SECATTR_MLS_CAT) == 0)
					found = 1;
				break;
			}
			for (acat = -1, kcat = -1; acat == kcat; ) {
				acat = netlbl_catmap_walk(sap->attr.mls.cat,
							  acat + 1);
				kcat = netlbl_catmap_walk(
					skp->smk_netlabel.attr.mls.cat,
					kcat + 1);
				if (acat < 0 || kcat < 0)
					break;
			}
			if (acat == kcat) {
				found = 1;
				break;
			}
		}
		rcu_read_unlock();

		if (found)
			return skp;

		if (ssp != NULL && ssp->smk_in == &smack_known_star)
			return &smack_known_web;
		return &smack_known_star;
	}
	/*
	 * Without guidance regarding the smack value
	 * for the packet fall back on the network
	 * ambient value.
	 */
	return smack_net_ambient;
}