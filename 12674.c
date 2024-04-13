static struct smack_known *smack_ipv4host_label(struct sockaddr_in *sip)
{
	struct smk_net4addr *snp;
	struct in_addr *siap = &sip->sin_addr;

	if (siap->s_addr == 0)
		return NULL;

	list_for_each_entry_rcu(snp, &smk_net4addr_list, list)
		/*
		 * we break after finding the first match because
		 * the list is sorted from longest to shortest mask
		 * so we have found the most specific match
		 */
		if (snp->smk_host.s_addr ==
		    (siap->s_addr & snp->smk_mask.s_addr))
			return snp->smk_label;

	return NULL;
}