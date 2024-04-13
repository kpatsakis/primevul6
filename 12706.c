static int bad_option(struct superblock_security_struct *sbsec, char flag,
		      u32 old_sid, u32 new_sid)
{
	char mnt_flags = sbsec->flags & SE_MNTMASK;

	/* check if the old mount command had the same options */
	if (sbsec->flags & SE_SBINITIALIZED)
		if (!(sbsec->flags & flag) ||
		    (old_sid != new_sid))
			return 1;

	/* check if we were passed the same options twice,
	 * aka someone passed context=a,context=b
	 */
	if (!(sbsec->flags & SE_SBINITIALIZED))
		if (mnt_flags & flag)
			return 1;
	return 0;
}