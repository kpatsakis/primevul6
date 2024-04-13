xfs_bulk_ireq_teardown(
	struct xfs_bulk_ireq	*hdr,
	struct xfs_ibulk	*breq)
{
	hdr->ino = breq->startino;
	hdr->ocount = breq->ocount;
}