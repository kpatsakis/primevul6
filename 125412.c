xfs_bulk_ireq_setup(
	struct xfs_mount	*mp,
	struct xfs_bulk_ireq	*hdr,
	struct xfs_ibulk	*breq,
	void __user		*ubuffer)
{
	if (hdr->icount == 0 ||
	    (hdr->flags & ~XFS_BULK_IREQ_FLAGS_ALL) ||
	    memchr_inv(hdr->reserved, 0, sizeof(hdr->reserved)))
		return -EINVAL;

	breq->startino = hdr->ino;
	breq->ubuffer = ubuffer;
	breq->icount = hdr->icount;
	breq->ocount = 0;
	breq->flags = 0;

	/*
	 * The @ino parameter is a special value, so we must look it up here.
	 * We're not allowed to have IREQ_AGNO, and we only return one inode
	 * worth of data.
	 */
	if (hdr->flags & XFS_BULK_IREQ_SPECIAL) {
		if (hdr->flags & XFS_BULK_IREQ_AGNO)
			return -EINVAL;

		switch (hdr->ino) {
		case XFS_BULK_IREQ_SPECIAL_ROOT:
			hdr->ino = mp->m_sb.sb_rootino;
			break;
		default:
			return -EINVAL;
		}
		breq->icount = 1;
	}

	/*
	 * The IREQ_AGNO flag means that we only want results from a given AG.
	 * If @hdr->ino is zero, we start iterating in that AG.  If @hdr->ino is
	 * beyond the specified AG then we return no results.
	 */
	if (hdr->flags & XFS_BULK_IREQ_AGNO) {
		if (hdr->agno >= mp->m_sb.sb_agcount)
			return -EINVAL;

		if (breq->startino == 0)
			breq->startino = XFS_AGINO_TO_INO(mp, hdr->agno, 0);
		else if (XFS_INO_TO_AGNO(mp, breq->startino) < hdr->agno)
			return -EINVAL;

		breq->flags |= XFS_IBULK_SAME_AG;

		/* Asking for an inode past the end of the AG?  We're done! */
		if (XFS_INO_TO_AGNO(mp, breq->startino) > hdr->agno)
			return -ECANCELED;
	} else if (hdr->agno)
		return -EINVAL;

	/* Asking for an inode past the end of the FS?  We're done! */
	if (XFS_INO_TO_AGNO(mp, breq->startino) >= mp->m_sb.sb_agcount)
		return -ECANCELED;

	return 0;
}