xfs_file_ioctl(
	struct file		*filp,
	unsigned int		cmd,
	unsigned long		p)
{
	struct inode		*inode = file_inode(filp);
	struct xfs_inode	*ip = XFS_I(inode);
	struct xfs_mount	*mp = ip->i_mount;
	void			__user *arg = (void __user *)p;
	int			error;

	trace_xfs_file_ioctl(ip);

	switch (cmd) {
	case FITRIM:
		return xfs_ioc_trim(mp, arg);
	case FS_IOC_GETFSLABEL:
		return xfs_ioc_getlabel(mp, arg);
	case FS_IOC_SETFSLABEL:
		return xfs_ioc_setlabel(filp, mp, arg);
	case XFS_IOC_ALLOCSP:
	case XFS_IOC_FREESP:
	case XFS_IOC_ALLOCSP64:
	case XFS_IOC_FREESP64: {
		xfs_flock64_t		bf;

		if (copy_from_user(&bf, arg, sizeof(bf)))
			return -EFAULT;
		return xfs_ioc_space(filp, &bf);
	}
	case XFS_IOC_DIOINFO: {
		struct xfs_buftarg	*target = xfs_inode_buftarg(ip);
		struct dioattr		da;

		da.d_mem =  da.d_miniosz = target->bt_logical_sectorsize;
		da.d_maxiosz = INT_MAX & ~(da.d_miniosz - 1);

		if (copy_to_user(arg, &da, sizeof(da)))
			return -EFAULT;
		return 0;
	}

	case XFS_IOC_FSBULKSTAT_SINGLE:
	case XFS_IOC_FSBULKSTAT:
	case XFS_IOC_FSINUMBERS:
		return xfs_ioc_fsbulkstat(filp, cmd, arg);

	case XFS_IOC_BULKSTAT:
		return xfs_ioc_bulkstat(filp, cmd, arg);
	case XFS_IOC_INUMBERS:
		return xfs_ioc_inumbers(mp, cmd, arg);

	case XFS_IOC_FSGEOMETRY_V1:
		return xfs_ioc_fsgeometry(mp, arg, 3);
	case XFS_IOC_FSGEOMETRY_V4:
		return xfs_ioc_fsgeometry(mp, arg, 4);
	case XFS_IOC_FSGEOMETRY:
		return xfs_ioc_fsgeometry(mp, arg, 5);

	case XFS_IOC_AG_GEOMETRY:
		return xfs_ioc_ag_geometry(mp, arg);

	case XFS_IOC_GETVERSION:
		return put_user(inode->i_generation, (int __user *)arg);

	case XFS_IOC_FSGETXATTRA:
		return xfs_ioc_fsgetxattra(ip, arg);

	case XFS_IOC_GETBMAP:
	case XFS_IOC_GETBMAPA:
	case XFS_IOC_GETBMAPX:
		return xfs_ioc_getbmap(filp, cmd, arg);

	case FS_IOC_GETFSMAP:
		return xfs_ioc_getfsmap(ip, arg);

	case XFS_IOC_SCRUB_METADATA:
		return xfs_ioc_scrub_metadata(filp, arg);

	case XFS_IOC_FD_TO_HANDLE:
	case XFS_IOC_PATH_TO_HANDLE:
	case XFS_IOC_PATH_TO_FSHANDLE: {
		xfs_fsop_handlereq_t	hreq;

		if (copy_from_user(&hreq, arg, sizeof(hreq)))
			return -EFAULT;
		return xfs_find_handle(cmd, &hreq);
	}
	case XFS_IOC_OPEN_BY_HANDLE: {
		xfs_fsop_handlereq_t	hreq;

		if (copy_from_user(&hreq, arg, sizeof(xfs_fsop_handlereq_t)))
			return -EFAULT;
		return xfs_open_by_handle(filp, &hreq);
	}

	case XFS_IOC_READLINK_BY_HANDLE: {
		xfs_fsop_handlereq_t	hreq;

		if (copy_from_user(&hreq, arg, sizeof(xfs_fsop_handlereq_t)))
			return -EFAULT;
		return xfs_readlink_by_handle(filp, &hreq);
	}
	case XFS_IOC_ATTRLIST_BY_HANDLE:
		return xfs_attrlist_by_handle(filp, arg);

	case XFS_IOC_ATTRMULTI_BY_HANDLE:
		return xfs_attrmulti_by_handle(filp, arg);

	case XFS_IOC_SWAPEXT: {
		struct xfs_swapext	sxp;

		if (copy_from_user(&sxp, arg, sizeof(xfs_swapext_t)))
			return -EFAULT;
		error = mnt_want_write_file(filp);
		if (error)
			return error;
		error = xfs_ioc_swapext(&sxp);
		mnt_drop_write_file(filp);
		return error;
	}

	case XFS_IOC_FSCOUNTS: {
		xfs_fsop_counts_t out;

		xfs_fs_counts(mp, &out);

		if (copy_to_user(arg, &out, sizeof(out)))
			return -EFAULT;
		return 0;
	}

	case XFS_IOC_SET_RESBLKS: {
		xfs_fsop_resblks_t inout;
		uint64_t	   in;

		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;

		if (xfs_is_readonly(mp))
			return -EROFS;

		if (copy_from_user(&inout, arg, sizeof(inout)))
			return -EFAULT;

		error = mnt_want_write_file(filp);
		if (error)
			return error;

		/* input parameter is passed in resblks field of structure */
		in = inout.resblks;
		error = xfs_reserve_blocks(mp, &in, &inout);
		mnt_drop_write_file(filp);
		if (error)
			return error;

		if (copy_to_user(arg, &inout, sizeof(inout)))
			return -EFAULT;
		return 0;
	}

	case XFS_IOC_GET_RESBLKS: {
		xfs_fsop_resblks_t out;

		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;

		error = xfs_reserve_blocks(mp, NULL, &out);
		if (error)
			return error;

		if (copy_to_user(arg, &out, sizeof(out)))
			return -EFAULT;

		return 0;
	}

	case XFS_IOC_FSGROWFSDATA: {
		struct xfs_growfs_data in;

		if (copy_from_user(&in, arg, sizeof(in)))
			return -EFAULT;

		error = mnt_want_write_file(filp);
		if (error)
			return error;
		error = xfs_growfs_data(mp, &in);
		mnt_drop_write_file(filp);
		return error;
	}

	case XFS_IOC_FSGROWFSLOG: {
		struct xfs_growfs_log in;

		if (copy_from_user(&in, arg, sizeof(in)))
			return -EFAULT;

		error = mnt_want_write_file(filp);
		if (error)
			return error;
		error = xfs_growfs_log(mp, &in);
		mnt_drop_write_file(filp);
		return error;
	}

	case XFS_IOC_FSGROWFSRT: {
		xfs_growfs_rt_t in;

		if (copy_from_user(&in, arg, sizeof(in)))
			return -EFAULT;

		error = mnt_want_write_file(filp);
		if (error)
			return error;
		error = xfs_growfs_rt(mp, &in);
		mnt_drop_write_file(filp);
		return error;
	}

	case XFS_IOC_GOINGDOWN: {
		uint32_t in;

		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;

		if (get_user(in, (uint32_t __user *)arg))
			return -EFAULT;

		return xfs_fs_goingdown(mp, in);
	}

	case XFS_IOC_ERROR_INJECTION: {
		xfs_error_injection_t in;

		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;

		if (copy_from_user(&in, arg, sizeof(in)))
			return -EFAULT;

		return xfs_errortag_add(mp, in.errtag);
	}

	case XFS_IOC_ERROR_CLEARALL:
		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;

		return xfs_errortag_clearall(mp);

	case XFS_IOC_FREE_EOFBLOCKS: {
		struct xfs_fs_eofblocks	eofb;
		struct xfs_icwalk	icw;

		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;

		if (xfs_is_readonly(mp))
			return -EROFS;

		if (copy_from_user(&eofb, arg, sizeof(eofb)))
			return -EFAULT;

		error = xfs_fs_eofblocks_from_user(&eofb, &icw);
		if (error)
			return error;

		trace_xfs_ioc_free_eofblocks(mp, &icw, _RET_IP_);

		sb_start_write(mp->m_super);
		error = xfs_blockgc_free_space(mp, &icw);
		sb_end_write(mp->m_super);
		return error;
	}

	default:
		return -ENOTTY;
	}
}