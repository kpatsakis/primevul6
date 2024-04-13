static void sctp_association_destroy(struct sctp_association *asoc)
{
	if (unlikely(!asoc->base.dead)) {
		WARN(1, "Attempt to destroy undead association %p!\n", asoc);
		return;
	}

	sctp_endpoint_put(asoc->ep);
	sock_put(asoc->base.sk);

	if (asoc->assoc_id != 0) {
		spin_lock_bh(&sctp_assocs_id_lock);
		idr_remove(&sctp_assocs_id, asoc->assoc_id);
		spin_unlock_bh(&sctp_assocs_id_lock);
	}

	WARN_ON(atomic_read(&asoc->rmem_alloc));

	kfree(asoc);
	SCTP_DBG_OBJCNT_DEC(assoc);
}