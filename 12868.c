static void selinux_key_free(struct key *k)
{
	struct key_security_struct *ksec = k->security;

	k->security = NULL;
	kfree(ksec);
}