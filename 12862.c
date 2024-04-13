static struct smack_known *smack_ipv6host_label(struct sockaddr_in6 *sip)
{
	struct smk_net6addr *snp;
	struct in6_addr *sap = &sip->sin6_addr;
	int i;
	int found = 0;

	/*
	 * It's local. Don't look for a host label.
	 */
	if (smk_ipv6_localhost(sip))
		return NULL;

	list_for_each_entry_rcu(snp, &smk_net6addr_list, list) {
		/*
		 * If the label is NULL the entry has
		 * been renounced. Ignore it.
		 */
		if (snp->smk_label == NULL)
			continue;
		/*
		* we break after finding the first match because
		* the list is sorted from longest to shortest mask
		* so we have found the most specific match
		*/
		for (found = 1, i = 0; i < 8; i++) {
			if ((sap->s6_addr16[i] & snp->smk_mask.s6_addr16[i]) !=
			    snp->smk_host.s6_addr16[i]) {
				found = 0;
				break;
			}
		}
		if (found)
			return snp->smk_label;
	}

	return NULL;
}