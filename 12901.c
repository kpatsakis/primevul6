static int smack_secctx_to_secid(const char *secdata, u32 seclen, u32 *secid)
{
	struct smack_known *skp = smk_find_entry(secdata);

	if (skp)
		*secid = skp->smk_secid;
	else
		*secid = 0;
	return 0;
}