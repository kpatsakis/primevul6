void sctp_assoc_del_peer(struct sctp_association *asoc,
			 const union sctp_addr *addr)
{
	struct list_head	*pos;
	struct list_head	*temp;
	struct sctp_transport	*transport;

	list_for_each_safe(pos, temp, &asoc->peer.transport_addr_list) {
		transport = list_entry(pos, struct sctp_transport, transports);
		if (sctp_cmp_addr_exact(addr, &transport->ipaddr)) {
			/* Do book keeping for removing the peer and free it. */
			sctp_assoc_rm_peer(asoc, transport);
			break;
		}
	}
}