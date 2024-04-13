struct sctp_transport *sctp_assoc_is_match(struct sctp_association *asoc,
					   struct net *net,
					   const union sctp_addr *laddr,
					   const union sctp_addr *paddr)
{
	struct sctp_transport *transport;

	if ((htons(asoc->base.bind_addr.port) == laddr->v4.sin_port) &&
	    (htons(asoc->peer.port) == paddr->v4.sin_port) &&
	    net_eq(sock_net(asoc->base.sk), net)) {
		transport = sctp_assoc_lookup_paddr(asoc, paddr);
		if (!transport)
			goto out;

		if (sctp_bind_addr_match(&asoc->base.bind_addr, laddr,
					 sctp_sk(asoc->base.sk)))
			goto out;
	}
	transport = NULL;

out:
	return transport;
}