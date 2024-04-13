static int smack_key_alloc(struct key *key, const struct cred *cred,
			   unsigned long flags)
{
	struct smack_known *skp = smk_of_task(smack_cred(cred));

	key->security = skp;
	return 0;
}