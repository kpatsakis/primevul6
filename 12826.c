static int smack_watch_key(struct key *key)
{
	struct smk_audit_info ad;
	struct smack_known *tkp = smk_of_current();
	int rc;

	if (key == NULL)
		return -EINVAL;
	/*
	 * If the key hasn't been initialized give it access so that
	 * it may do so.
	 */
	if (key->security == NULL)
		return 0;
	/*
	 * This should not occur
	 */
	if (tkp == NULL)
		return -EACCES;

	if (smack_privileged_cred(CAP_MAC_OVERRIDE, current_cred()))
		return 0;

#ifdef CONFIG_AUDIT
	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_KEY);
	ad.a.u.key_struct.key = key->serial;
	ad.a.u.key_struct.key_desc = key->description;
#endif
	rc = smk_access(tkp, key->security, MAY_READ, &ad);
	rc = smk_bu_note("key watch", tkp, key->security, MAY_READ, rc);
	return rc;
}