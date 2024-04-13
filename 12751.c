static int smack_key_permission(key_ref_t key_ref,
				const struct cred *cred,
				enum key_need_perm need_perm)
{
	struct key *keyp;
	struct smk_audit_info ad;
	struct smack_known *tkp = smk_of_task(smack_cred(cred));
	int request = 0;
	int rc;

	/*
	 * Validate requested permissions
	 */
	switch (need_perm) {
	case KEY_NEED_READ:
	case KEY_NEED_SEARCH:
	case KEY_NEED_VIEW:
		request |= MAY_READ;
		break;
	case KEY_NEED_WRITE:
	case KEY_NEED_LINK:
	case KEY_NEED_SETATTR:
		request |= MAY_WRITE;
		break;
	case KEY_NEED_UNSPECIFIED:
	case KEY_NEED_UNLINK:
	case KEY_SYSADMIN_OVERRIDE:
	case KEY_AUTHTOKEN_OVERRIDE:
	case KEY_DEFER_PERM_CHECK:
		return 0;
	default:
		return -EINVAL;
	}

	keyp = key_ref_to_ptr(key_ref);
	if (keyp == NULL)
		return -EINVAL;
	/*
	 * If the key hasn't been initialized give it access so that
	 * it may do so.
	 */
	if (keyp->security == NULL)
		return 0;
	/*
	 * This should not occur
	 */
	if (tkp == NULL)
		return -EACCES;

	if (smack_privileged(CAP_MAC_OVERRIDE))
		return 0;

#ifdef CONFIG_AUDIT
	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_KEY);
	ad.a.u.key_struct.key = keyp->serial;
	ad.a.u.key_struct.key_desc = keyp->description;
#endif
	rc = smk_access(tkp, keyp->security, request, &ad);
	rc = smk_bu_note("key access", tkp, keyp->security, request, rc);
	return rc;
}