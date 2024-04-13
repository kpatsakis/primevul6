static int smack_secid_to_secctx(u32 secid, char **secdata, u32 *seclen)
{
	struct smack_known *skp = smack_from_secid(secid);

	if (secdata)
		*secdata = skp->smk_known;
	*seclen = strlen(skp->smk_known);
	return 0;
}