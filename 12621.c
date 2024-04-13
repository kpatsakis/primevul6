static int smack_sb_alloc_security(struct super_block *sb)
{
	struct superblock_smack *sbsp = smack_superblock(sb);

	sbsp->smk_root = &smack_known_floor;
	sbsp->smk_default = &smack_known_floor;
	sbsp->smk_floor = &smack_known_floor;
	sbsp->smk_hat = &smack_known_hat;
	/*
	 * SMK_SB_INITIALIZED will be zero from kzalloc.
	 */

	return 0;
}