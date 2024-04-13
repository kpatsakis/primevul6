tcp_hdrsz(struct tcpcb *tp)
{
	u_int hlen;

	switch (tp->pf) {
#ifdef INET6
	case AF_INET6:
		hlen = sizeof(struct ip6_hdr);
		break;
#endif
	case AF_INET:
		hlen = sizeof(struct ip);
		break;
	default:
		hlen = 0;
		break;
	}
	hlen += sizeof(struct tcphdr);

	if ((tp->t_flags & (TF_REQ_TSTMP|TF_NOOPT)) == TF_REQ_TSTMP &&
	    (tp->t_flags & TF_RCVD_TSTMP) == TF_RCVD_TSTMP)
		hlen += TCPOLEN_TSTAMP_APPA;
#ifdef TCP_SIGNATURE
	if (tp->t_flags & TF_SIGNATURE)
		hlen += TCPOLEN_SIGLEN;
#endif
	return (hlen);
}