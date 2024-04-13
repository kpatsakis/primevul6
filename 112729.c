tcp_dooptions(struct tcpcb *tp, u_char *cp, int cnt, struct tcphdr *th,
    struct mbuf *m, int iphlen, struct tcp_opt_info *oi,
    u_int rtableid)
{
	u_int16_t mss = 0;
	int opt, optlen;
#ifdef TCP_SIGNATURE
	caddr_t sigp = NULL;
	struct tdb *tdb = NULL;
#endif /* TCP_SIGNATURE */

	for (; cp && cnt > 0; cnt -= optlen, cp += optlen) {
		opt = cp[0];
		if (opt == TCPOPT_EOL)
			break;
		if (opt == TCPOPT_NOP)
			optlen = 1;
		else {
			if (cnt < 2)
				break;
			optlen = cp[1];
			if (optlen < 2 || optlen > cnt)
				break;
		}
		switch (opt) {

		default:
			continue;

		case TCPOPT_MAXSEG:
			if (optlen != TCPOLEN_MAXSEG)
				continue;
			if (!(th->th_flags & TH_SYN))
				continue;
			if (TCPS_HAVERCVDSYN(tp->t_state))
				continue;
			memcpy(&mss, cp + 2, sizeof(mss));
			mss = ntohs(mss);
			oi->maxseg = mss;
			break;

		case TCPOPT_WINDOW:
			if (optlen != TCPOLEN_WINDOW)
				continue;
			if (!(th->th_flags & TH_SYN))
				continue;
			if (TCPS_HAVERCVDSYN(tp->t_state))
				continue;
			tp->t_flags |= TF_RCVD_SCALE;
			tp->requested_s_scale = min(cp[2], TCP_MAX_WINSHIFT);
			break;

		case TCPOPT_TIMESTAMP:
			if (optlen != TCPOLEN_TIMESTAMP)
				continue;
			oi->ts_present = 1;
			memcpy(&oi->ts_val, cp + 2, sizeof(oi->ts_val));
			oi->ts_val = ntohl(oi->ts_val);
			memcpy(&oi->ts_ecr, cp + 6, sizeof(oi->ts_ecr));
			oi->ts_ecr = ntohl(oi->ts_ecr);

			if (!(th->th_flags & TH_SYN))
				continue;
			if (TCPS_HAVERCVDSYN(tp->t_state))
				continue;
			/*
			 * A timestamp received in a SYN makes
			 * it ok to send timestamp requests and replies.
			 */
			tp->t_flags |= TF_RCVD_TSTMP;
			tp->ts_recent = oi->ts_val;
			tp->ts_recent_age = tcp_now;
			break;

		case TCPOPT_SACK_PERMITTED:
			if (!tp->sack_enable || optlen!=TCPOLEN_SACK_PERMITTED)
				continue;
			if (!(th->th_flags & TH_SYN))
				continue;
			if (TCPS_HAVERCVDSYN(tp->t_state))
				continue;
			/* MUST only be set on SYN */
			tp->t_flags |= TF_SACK_PERMIT;
			break;
		case TCPOPT_SACK:
			tcp_sack_option(tp, th, cp, optlen);
			break;
#ifdef TCP_SIGNATURE
		case TCPOPT_SIGNATURE:
			if (optlen != TCPOLEN_SIGNATURE)
				continue;

			if (sigp && timingsafe_bcmp(sigp, cp + 2, 16))
				return (-1);

			sigp = cp + 2;
			break;
#endif /* TCP_SIGNATURE */
		}
	}

#ifdef TCP_SIGNATURE
	if (tp->t_flags & TF_SIGNATURE) {
		union sockaddr_union src, dst;

		memset(&src, 0, sizeof(union sockaddr_union));
		memset(&dst, 0, sizeof(union sockaddr_union));

		switch (tp->pf) {
		case 0:
		case AF_INET:
			src.sa.sa_len = sizeof(struct sockaddr_in);
			src.sa.sa_family = AF_INET;
			src.sin.sin_addr = mtod(m, struct ip *)->ip_src;
			dst.sa.sa_len = sizeof(struct sockaddr_in);
			dst.sa.sa_family = AF_INET;
			dst.sin.sin_addr = mtod(m, struct ip *)->ip_dst;
			break;
#ifdef INET6
		case AF_INET6:
			src.sa.sa_len = sizeof(struct sockaddr_in6);
			src.sa.sa_family = AF_INET6;
			src.sin6.sin6_addr = mtod(m, struct ip6_hdr *)->ip6_src;
			dst.sa.sa_len = sizeof(struct sockaddr_in6);
			dst.sa.sa_family = AF_INET6;
			dst.sin6.sin6_addr = mtod(m, struct ip6_hdr *)->ip6_dst;
			break;
#endif /* INET6 */
		}

		tdb = gettdbbysrcdst(rtable_l2(rtableid),
		    0, &src, &dst, IPPROTO_TCP);

		/*
		 * We don't have an SA for this peer, so we turn off
		 * TF_SIGNATURE on the listen socket
		 */
		if (tdb == NULL && tp->t_state == TCPS_LISTEN)
			tp->t_flags &= ~TF_SIGNATURE;

	}

	if ((sigp ? TF_SIGNATURE : 0) ^ (tp->t_flags & TF_SIGNATURE)) {
		tcpstat_inc(tcps_rcvbadsig);
		return (-1);
	}

	if (sigp) {
		char sig[16];

		if (tdb == NULL) {
			tcpstat_inc(tcps_rcvbadsig);
			return (-1);
		}

		if (tcp_signature(tdb, tp->pf, m, th, iphlen, 1, sig) < 0)
			return (-1);

		if (timingsafe_bcmp(sig, sigp, 16)) {
			tcpstat_inc(tcps_rcvbadsig);
			return (-1);
		}

		tcpstat_inc(tcps_rcvgoodsig);
	}
#endif /* TCP_SIGNATURE */

	return (0);
}