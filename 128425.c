void sctp_asconf_queue_teardown(struct sctp_association *asoc)
{
	/* Free any cached ASCONF_ACK chunk. */
	sctp_assoc_free_asconf_acks(asoc);

	/* Free the ASCONF queue. */
	sctp_assoc_free_asconf_queue(asoc);

	/* Free any cached ASCONF chunk. */
	if (asoc->addip_last_asconf)
		sctp_chunk_free(asoc->addip_last_asconf);
}