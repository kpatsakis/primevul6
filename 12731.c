static int smk_copy_relabel(struct list_head *nhead, struct list_head *ohead,
				gfp_t gfp)
{
	struct smack_known_list_elem *nklep;
	struct smack_known_list_elem *oklep;

	list_for_each_entry(oklep, ohead, list) {
		nklep = kzalloc(sizeof(struct smack_known_list_elem), gfp);
		if (nklep == NULL) {
			smk_destroy_label_list(nhead);
			return -ENOMEM;
		}
		nklep->smk_label = oklep->smk_label;
		list_add(&nklep->list, nhead);
	}

	return 0;
}