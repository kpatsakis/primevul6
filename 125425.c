xfs_ioc_fsgeometry(
	struct xfs_mount	*mp,
	void			__user *arg,
	int			struct_version)
{
	struct xfs_fsop_geom	fsgeo;
	size_t			len;

	xfs_fs_geometry(mp, &fsgeo, struct_version);

	if (struct_version <= 3)
		len = sizeof(struct xfs_fsop_geom_v1);
	else if (struct_version == 4)
		len = sizeof(struct xfs_fsop_geom_v4);
	else {
		xfs_fsop_geom_health(mp, &fsgeo);
		len = sizeof(fsgeo);
	}

	if (copy_to_user(arg, &fsgeo, len))
		return -EFAULT;
	return 0;
}