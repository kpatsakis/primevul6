xfs_ioc_getfsmap(
	struct xfs_inode	*ip,
	struct fsmap_head	__user *arg)
{
	struct xfs_fsmap_head	xhead = {0};
	struct fsmap_head	head;
	struct fsmap		*recs;
	unsigned int		count;
	__u32			last_flags = 0;
	bool			done = false;
	int			error;

	if (copy_from_user(&head, arg, sizeof(struct fsmap_head)))
		return -EFAULT;
	if (memchr_inv(head.fmh_reserved, 0, sizeof(head.fmh_reserved)) ||
	    memchr_inv(head.fmh_keys[0].fmr_reserved, 0,
		       sizeof(head.fmh_keys[0].fmr_reserved)) ||
	    memchr_inv(head.fmh_keys[1].fmr_reserved, 0,
		       sizeof(head.fmh_keys[1].fmr_reserved)))
		return -EINVAL;

	/*
	 * Use an internal memory buffer so that we don't have to copy fsmap
	 * data to userspace while holding locks.  Start by trying to allocate
	 * up to 128k for the buffer, but fall back to a single page if needed.
	 */
	count = min_t(unsigned int, head.fmh_count,
			131072 / sizeof(struct fsmap));
	recs = kvcalloc(count, sizeof(struct fsmap), GFP_KERNEL);
	if (!recs) {
		count = min_t(unsigned int, head.fmh_count,
				PAGE_SIZE / sizeof(struct fsmap));
		recs = kvcalloc(count, sizeof(struct fsmap), GFP_KERNEL);
		if (!recs)
			return -ENOMEM;
	}

	xhead.fmh_iflags = head.fmh_iflags;
	xfs_fsmap_to_internal(&xhead.fmh_keys[0], &head.fmh_keys[0]);
	xfs_fsmap_to_internal(&xhead.fmh_keys[1], &head.fmh_keys[1]);

	trace_xfs_getfsmap_low_key(ip->i_mount, &xhead.fmh_keys[0]);
	trace_xfs_getfsmap_high_key(ip->i_mount, &xhead.fmh_keys[1]);

	head.fmh_entries = 0;
	do {
		struct fsmap __user	*user_recs;
		struct fsmap		*last_rec;

		user_recs = &arg->fmh_recs[head.fmh_entries];
		xhead.fmh_entries = 0;
		xhead.fmh_count = min_t(unsigned int, count,
					head.fmh_count - head.fmh_entries);

		/* Run query, record how many entries we got. */
		error = xfs_getfsmap(ip->i_mount, &xhead, recs);
		switch (error) {
		case 0:
			/*
			 * There are no more records in the result set.  Copy
			 * whatever we got to userspace and break out.
			 */
			done = true;
			break;
		case -ECANCELED:
			/*
			 * The internal memory buffer is full.  Copy whatever
			 * records we got to userspace and go again if we have
			 * not yet filled the userspace buffer.
			 */
			error = 0;
			break;
		default:
			goto out_free;
		}
		head.fmh_entries += xhead.fmh_entries;
		head.fmh_oflags = xhead.fmh_oflags;

		/*
		 * If the caller wanted a record count or there aren't any
		 * new records to return, we're done.
		 */
		if (head.fmh_count == 0 || xhead.fmh_entries == 0)
			break;

		/* Copy all the records we got out to userspace. */
		if (copy_to_user(user_recs, recs,
				 xhead.fmh_entries * sizeof(struct fsmap))) {
			error = -EFAULT;
			goto out_free;
		}

		/* Remember the last record flags we copied to userspace. */
		last_rec = &recs[xhead.fmh_entries - 1];
		last_flags = last_rec->fmr_flags;

		/* Set up the low key for the next iteration. */
		xfs_fsmap_to_internal(&xhead.fmh_keys[0], last_rec);
		trace_xfs_getfsmap_low_key(ip->i_mount, &xhead.fmh_keys[0]);
	} while (!done && head.fmh_entries < head.fmh_count);

	/*
	 * If there are no more records in the query result set and we're not
	 * in counting mode, mark the last record returned with the LAST flag.
	 */
	if (done && head.fmh_count > 0 && head.fmh_entries > 0) {
		struct fsmap __user	*user_rec;

		last_flags |= FMR_OF_LAST;
		user_rec = &arg->fmh_recs[head.fmh_entries - 1];

		if (copy_to_user(&user_rec->fmr_flags, &last_flags,
					sizeof(last_flags))) {
			error = -EFAULT;
			goto out_free;
		}
	}

	/* copy back header */
	if (copy_to_user(arg, &head, sizeof(struct fsmap_head))) {
		error = -EFAULT;
		goto out_free;
	}

out_free:
	kmem_free(recs);
	return error;
}