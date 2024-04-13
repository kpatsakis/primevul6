static int smack_post_notification(const struct cred *w_cred,
				   const struct cred *cred,
				   struct watch_notification *n)
{
	struct smk_audit_info ad;
	struct smack_known *subj, *obj;
	int rc;

	/* Always let maintenance notifications through. */
	if (n->type == WATCH_TYPE_META)
		return 0;

	if (!cred)
		return 0;
	subj = smk_of_task(smack_cred(cred));
	obj = smk_of_task(smack_cred(w_cred));

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_NOTIFICATION);
	rc = smk_access(subj, obj, MAY_WRITE, &ad);
	rc = smk_bu_note("notification", subj, obj, MAY_WRITE, rc);
	return rc;
}