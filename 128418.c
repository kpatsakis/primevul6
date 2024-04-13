sctp_assoc_choose_alter_transport(struct sctp_association *asoc,
				  struct sctp_transport *last_sent_to)
{
	/* If this is the first time packet is sent, use the active path,
	 * else use the retran path. If the last packet was sent over the
	 * retran path, update the retran path and use it.
	 */
	if (last_sent_to == NULL) {
		return asoc->peer.active_path;
	} else {
		if (last_sent_to == asoc->peer.retran_path)
			sctp_assoc_update_retran_path(asoc);

		return asoc->peer.retran_path;
	}
}