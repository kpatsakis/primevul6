static int selinux_umount(struct vfsmount *mnt, int flags)
{
	const struct cred *cred = current_cred();

	return superblock_has_perm(cred, mnt->mnt_sb,
				   FILESYSTEM__UNMOUNT, NULL);
}