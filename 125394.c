xfs_fs_eofblocks_from_user(
	struct xfs_fs_eofblocks		*src,
	struct xfs_icwalk		*dst)
{
	if (src->eof_version != XFS_EOFBLOCKS_VERSION)
		return -EINVAL;

	if (src->eof_flags & ~XFS_EOF_FLAGS_VALID)
		return -EINVAL;

	if (memchr_inv(&src->pad32, 0, sizeof(src->pad32)) ||
	    memchr_inv(src->pad64, 0, sizeof(src->pad64)))
		return -EINVAL;

	dst->icw_flags = 0;
	if (src->eof_flags & XFS_EOF_FLAGS_SYNC)
		dst->icw_flags |= XFS_ICWALK_FLAG_SYNC;
	if (src->eof_flags & XFS_EOF_FLAGS_UID)
		dst->icw_flags |= XFS_ICWALK_FLAG_UID;
	if (src->eof_flags & XFS_EOF_FLAGS_GID)
		dst->icw_flags |= XFS_ICWALK_FLAG_GID;
	if (src->eof_flags & XFS_EOF_FLAGS_PRID)
		dst->icw_flags |= XFS_ICWALK_FLAG_PRID;
	if (src->eof_flags & XFS_EOF_FLAGS_MINFILESIZE)
		dst->icw_flags |= XFS_ICWALK_FLAG_MINFILESIZE;

	dst->icw_prid = src->eof_prid;
	dst->icw_min_file_size = src->eof_min_file_size;

	dst->icw_uid = INVALID_UID;
	if (src->eof_flags & XFS_EOF_FLAGS_UID) {
		dst->icw_uid = make_kuid(current_user_ns(), src->eof_uid);
		if (!uid_valid(dst->icw_uid))
			return -EINVAL;
	}

	dst->icw_gid = INVALID_GID;
	if (src->eof_flags & XFS_EOF_FLAGS_GID) {
		dst->icw_gid = make_kgid(current_user_ns(), src->eof_gid);
		if (!gid_valid(dst->icw_gid))
			return -EINVAL;
	}
	return 0;
}