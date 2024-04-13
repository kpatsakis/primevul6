static int smack_setprocattr(const char *name, void *value, size_t size)
{
	struct task_smack *tsp = smack_cred(current_cred());
	struct cred *new;
	struct smack_known *skp;
	struct smack_known_list_elem *sklep;
	int rc;

	if (!smack_privileged(CAP_MAC_ADMIN) && list_empty(&tsp->smk_relabel))
		return -EPERM;

	if (value == NULL || size == 0 || size >= SMK_LONGLABEL)
		return -EINVAL;

	if (strcmp(name, "current") != 0)
		return -EINVAL;

	skp = smk_import_entry(value, size);
	if (IS_ERR(skp))
		return PTR_ERR(skp);

	/*
	 * No process is ever allowed the web ("@") label
	 * and the star ("*") label.
	 */
	if (skp == &smack_known_web || skp == &smack_known_star)
		return -EINVAL;

	if (!smack_privileged(CAP_MAC_ADMIN)) {
		rc = -EPERM;
		list_for_each_entry(sklep, &tsp->smk_relabel, list)
			if (sklep->smk_label == skp) {
				rc = 0;
				break;
			}
		if (rc)
			return rc;
	}

	new = prepare_creds();
	if (new == NULL)
		return -ENOMEM;

	tsp = smack_cred(new);
	tsp->smk_task = skp;
	/*
	 * process can change its label only once
	 */
	smk_destroy_label_list(&tsp->smk_relabel);

	commit_creds(new);
	return size;
}