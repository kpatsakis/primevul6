static int smack_audit_rule_match(u32 secid, u32 field, u32 op, void *vrule)
{
	struct smack_known *skp;
	char *rule = vrule;

	if (unlikely(!rule)) {
		WARN_ONCE(1, "Smack: missing rule\n");
		return -ENOENT;
	}

	if (field != AUDIT_SUBJ_USER && field != AUDIT_OBJ_USER)
		return 0;

	skp = smack_from_secid(secid);

	/*
	 * No need to do string comparisons. If a match occurs,
	 * both pointers will point to the same smack_known
	 * label.
	 */
	if (op == Audit_equal)
		return (rule == skp->smk_known);
	if (op == Audit_not_equal)
		return (rule != skp->smk_known);

	return 0;
}