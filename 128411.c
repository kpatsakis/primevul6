struct sctp_transport *sctp_assoc_lookup_tsn(struct sctp_association *asoc,
					     __u32 tsn)
{
	struct sctp_transport *active;
	struct sctp_transport *match;
	struct sctp_transport *transport;
	struct sctp_chunk *chunk;
	__be32 key = htonl(tsn);

	match = NULL;

	/*
	 * FIXME: In general, find a more efficient data structure for
	 * searching.
	 */

	/*
	 * The general strategy is to search each transport's transmitted
	 * list.   Return which transport this TSN lives on.
	 *
	 * Let's be hopeful and check the active_path first.
	 * Another optimization would be to know if there is only one
	 * outbound path and not have to look for the TSN at all.
	 *
	 */

	active = asoc->peer.active_path;

	list_for_each_entry(chunk, &active->transmitted,
			transmitted_list) {

		if (key == chunk->subh.data_hdr->tsn) {
			match = active;
			goto out;
		}
	}

	/* If not found, go search all the other transports. */
	list_for_each_entry(transport, &asoc->peer.transport_addr_list,
			transports) {

		if (transport == active)
			continue;
		list_for_each_entry(chunk, &transport->transmitted,
				transmitted_list) {
			if (key == chunk->subh.data_hdr->tsn) {
				match = transport;
				goto out;
			}
		}
	}
out:
	return match;
}