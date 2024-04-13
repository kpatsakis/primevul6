void sctp_assoc_del_nonprimary_peers(struct sctp_association *asoc,
				     struct sctp_transport *primary)
{
	struct sctp_transport	*temp;
	struct sctp_transport	*t;

	list_for_each_entry_safe(t, temp, &asoc->peer.transport_addr_list,
				 transports) {
		/* if the current transport is not the primary one, delete it */
		if (t != primary)
			sctp_assoc_rm_peer(asoc, t);
	}
}