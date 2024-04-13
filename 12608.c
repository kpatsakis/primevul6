static int smack_audit_rule_init(u32 field, u32 op, char *rulestr, void **vrule)
{
	struct smack_known *skp;
	char **rule = (char **)vrule;
	*rule = NULL;

	if (field != AUDIT_SUBJ_USER && field != AUDIT_OBJ_USER)
		return -EINVAL;

	if (op != Audit_equal && op != Audit_not_equal)
		return -EINVAL;

	skp = smk_import_entry(rulestr, 0);
	if (IS_ERR(skp))
		return PTR_ERR(skp);

	*rule = skp->smk_known;

	return 0;
}