static void smack_cred_free(struct cred *cred)
{
	struct task_smack *tsp = smack_cred(cred);
	struct smack_rule *rp;
	struct list_head *l;
	struct list_head *n;

	smk_destroy_label_list(&tsp->smk_relabel);

	list_for_each_safe(l, n, &tsp->smk_rules) {
		rp = list_entry(l, struct smack_rule, list);
		list_del(&rp->list);
		kmem_cache_free(smack_rule_cache, rp);
	}
}