static int smk_copy_rules(struct list_head *nhead, struct list_head *ohead,
				gfp_t gfp)
{
	struct smack_rule *nrp;
	struct smack_rule *orp;
	int rc = 0;

	list_for_each_entry_rcu(orp, ohead, list) {
		nrp = kmem_cache_zalloc(smack_rule_cache, gfp);
		if (nrp == NULL) {
			rc = -ENOMEM;
			break;
		}
		*nrp = *orp;
		list_add_rcu(&nrp->list, nhead);
	}
	return rc;
}