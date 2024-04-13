int sctp_cmp_addr_exact(const union sctp_addr *ss1,
			const union sctp_addr *ss2)
{
	struct sctp_af *af;

	af = sctp_get_af_specific(ss1->sa.sa_family);
	if (unlikely(!af))
		return 0;

	return af->cmp_addr(ss1, ss2);
}