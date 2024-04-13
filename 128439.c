int sctp_assoc_set_bind_addr_from_ep(struct sctp_association *asoc,
				     sctp_scope_t scope, gfp_t gfp)
{
	int flags;

	/* Use scoping rules to determine the subset of addresses from
	 * the endpoint.
	 */
	flags = (PF_INET6 == asoc->base.sk->sk_family) ? SCTP_ADDR6_ALLOWED : 0;
	if (asoc->peer.ipv4_address)
		flags |= SCTP_ADDR4_PEERSUPP;
	if (asoc->peer.ipv6_address)
		flags |= SCTP_ADDR6_PEERSUPP;

	return sctp_bind_addr_copy(sock_net(asoc->base.sk),
				   &asoc->base.bind_addr,
				   &asoc->ep->base.bind_addr,
				   scope, gfp, flags);
}