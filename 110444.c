xfs_fs_fill_super(
	struct super_block	*sb,
	void			*data,
	int			silent)
{
	struct inode		*root;
	struct xfs_mount	*mp = NULL;
	int			flags = 0, error = -ENOMEM;

	/*
	 * allocate mp and do all low-level struct initializations before we
	 * attach it to the super
	 */
	mp = xfs_mount_alloc(sb);
	if (!mp)
		goto out;
	sb->s_fs_info = mp;

	error = xfs_parseargs(mp, (char *)data);
	if (error)
		goto out_free_fsname;

	sb_min_blocksize(sb, BBSIZE);
	sb->s_xattr = xfs_xattr_handlers;
	sb->s_export_op = &xfs_export_operations;
#ifdef CONFIG_XFS_QUOTA
	sb->s_qcop = &xfs_quotactl_operations;
	sb->s_quota_types = QTYPE_MASK_USR | QTYPE_MASK_GRP | QTYPE_MASK_PRJ;
#endif
	sb->s_op = &xfs_super_operations;

	/*
	 * Delay mount work if the debug hook is set. This is debug
	 * instrumention to coordinate simulation of xfs mount failures with
	 * VFS superblock operations
	 */
	if (xfs_globals.mount_delay) {
		xfs_notice(mp, "Delaying mount for %d seconds.",
			xfs_globals.mount_delay);
		msleep(xfs_globals.mount_delay * 1000);
	}

	if (silent)
		flags |= XFS_MFSI_QUIET;

	error = xfs_open_devices(mp);
	if (error)
		goto out_free_fsname;

	error = xfs_init_mount_workqueues(mp);
	if (error)
		goto out_close_devices;

	error = xfs_init_percpu_counters(mp);
	if (error)
		goto out_destroy_workqueues;

	/* Allocate stats memory before we do operations that might use it */
	mp->m_stats.xs_stats = alloc_percpu(struct xfsstats);
	if (!mp->m_stats.xs_stats) {
		error = -ENOMEM;
		goto out_destroy_counters;
	}

	error = xfs_readsb(mp, flags);
	if (error)
		goto out_free_stats;

	error = xfs_finish_flags(mp);
	if (error)
		goto out_free_sb;

	error = xfs_setup_devices(mp);
	if (error)
		goto out_free_sb;

	error = xfs_filestream_mount(mp);
	if (error)
		goto out_free_sb;

	/*
	 * we must configure the block size in the superblock before we run the
	 * full mount process as the mount process can lookup and cache inodes.
	 */
	sb->s_magic = XFS_SB_MAGIC;
	sb->s_blocksize = mp->m_sb.sb_blocksize;
	sb->s_blocksize_bits = ffs(sb->s_blocksize) - 1;
	sb->s_maxbytes = xfs_max_file_offset(sb->s_blocksize_bits);
	sb->s_max_links = XFS_MAXLINK;
	sb->s_time_gran = 1;
	set_posix_acl_flag(sb);

	/* version 5 superblocks support inode version counters. */
	if (XFS_SB_VERSION_NUM(&mp->m_sb) == XFS_SB_VERSION_5)
		sb->s_flags |= SB_I_VERSION;

	if (mp->m_flags & XFS_MOUNT_DAX) {
		xfs_warn(mp,
		"DAX enabled. Warning: EXPERIMENTAL, use at your own risk");

		error = bdev_dax_supported(sb, sb->s_blocksize);
		if (error) {
			xfs_alert(mp,
			"DAX unsupported by block device. Turning off DAX.");
			mp->m_flags &= ~XFS_MOUNT_DAX;
		}
		if (xfs_sb_version_hasreflink(&mp->m_sb)) {
			xfs_alert(mp,
		"DAX and reflink cannot be used together!");
			error = -EINVAL;
			goto out_filestream_unmount;
		}
	}

	if (mp->m_flags & XFS_MOUNT_DISCARD) {
		struct request_queue *q = bdev_get_queue(sb->s_bdev);

		if (!blk_queue_discard(q)) {
			xfs_warn(mp, "mounting with \"discard\" option, but "
					"the device does not support discard");
			mp->m_flags &= ~XFS_MOUNT_DISCARD;
		}
	}

	if (xfs_sb_version_hasreflink(&mp->m_sb) && mp->m_sb.sb_rblocks) {
		xfs_alert(mp,
	"reflink not compatible with realtime device!");
		error = -EINVAL;
		goto out_filestream_unmount;
	}

	if (xfs_sb_version_hasrmapbt(&mp->m_sb) && mp->m_sb.sb_rblocks) {
		xfs_alert(mp,
	"reverse mapping btree not compatible with realtime device!");
		error = -EINVAL;
		goto out_filestream_unmount;
	}

	error = xfs_mountfs(mp);
	if (error)
		goto out_filestream_unmount;

	root = igrab(VFS_I(mp->m_rootip));
	if (!root) {
		error = -ENOENT;
		goto out_unmount;
	}
	sb->s_root = d_make_root(root);
	if (!sb->s_root) {
		error = -ENOMEM;
		goto out_unmount;
	}

	return 0;

 out_filestream_unmount:
	xfs_filestream_unmount(mp);
 out_free_sb:
	xfs_freesb(mp);
 out_free_stats:
	free_percpu(mp->m_stats.xs_stats);
 out_destroy_counters:
	xfs_destroy_percpu_counters(mp);
 out_destroy_workqueues:
	xfs_destroy_mount_workqueues(mp);
 out_close_devices:
	xfs_close_devices(mp);
 out_free_fsname:
	xfs_free_fsname(mp);
	kfree(mp);
 out:
	return error;

 out_unmount:
	xfs_filestream_unmount(mp);
	xfs_unmountfs(mp);
	goto out_free_sb;
}