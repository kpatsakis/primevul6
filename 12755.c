static int selinux_sb_alloc_security(struct super_block *sb)
{
	struct superblock_security_struct *sbsec = selinux_superblock(sb);

	mutex_init(&sbsec->lock);
	INIT_LIST_HEAD(&sbsec->isec_head);
	spin_lock_init(&sbsec->isec_lock);
	sbsec->sid = SECINITSID_UNLABELED;
	sbsec->def_sid = SECINITSID_FILE;
	sbsec->mntpoint_sid = SECINITSID_UNLABELED;

	return 0;
}