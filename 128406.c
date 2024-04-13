int sctp_assoc_set_bind_addr_from_cookie(struct sctp_association *asoc,
					 struct sctp_cookie *cookie,
					 gfp_t gfp)
{
	int var_size2 = ntohs(cookie->peer_init->chunk_hdr.length);
	int var_size3 = cookie->raw_addr_list_len;
	__u8 *raw = (__u8 *)cookie->peer_init + var_size2;

	return sctp_raw_to_bind_addrs(&asoc->base.bind_addr, raw, var_size3,
				      asoc->ep->base.bind_addr.port, gfp);
}