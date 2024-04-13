struct sctp_transport *sctp_assoc_lookup_paddr(
					const struct sctp_association *asoc,
					const union sctp_addr *address)
{
	struct sctp_transport *t;

	/* Cycle through all transports searching for a peer address. */

	list_for_each_entry(t, &asoc->peer.transport_addr_list,
			transports) {
		if (sctp_cmp_addr_exact(address, &t->ipaddr))
			return t;
	}

	return NULL;
}