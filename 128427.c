int sctp_assoc_set_id(struct sctp_association *asoc, gfp_t gfp)
{
	bool preload = !!(gfp & __GFP_WAIT);
	int ret;

	/* If the id is already assigned, keep it. */
	if (asoc->assoc_id)
		return 0;

	if (preload)
		idr_preload(gfp);
	spin_lock_bh(&sctp_assocs_id_lock);
	/* 0 is not a valid assoc_id, must be >= 1 */
	ret = idr_alloc_cyclic(&sctp_assocs_id, asoc, 1, 0, GFP_NOWAIT);
	spin_unlock_bh(&sctp_assocs_id_lock);
	if (preload)
		idr_preload_end();
	if (ret < 0)
		return ret;

	asoc->assoc_id = (sctp_assoc_t)ret;
	return 0;
}