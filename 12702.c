static int smk_bu_note(char *note, struct smack_known *sskp,
		       struct smack_known *oskp, int mode, int rc)
{
	char acc[SMK_NUM_ACCESS_TYPE + 1];

	if (rc <= 0)
		return rc;
	if (rc > SMACK_UNCONFINED_OBJECT)
		rc = 0;

	smk_bu_mode(mode, acc);
	pr_info("Smack %s: (%s %s %s) %s\n", smk_bu_mess[rc],
		sskp->smk_known, oskp->smk_known, acc, note);
	return 0;
}