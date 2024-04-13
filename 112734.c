tcp_input(struct mbuf **mp, int *offp, int proto, int af)
{
	struct mbuf *m = *mp;
	int iphlen = *offp;
	struct ip *ip = NULL;
	struct inpcb *inp = NULL;
	u_int8_t *optp = NULL;
	int optlen = 0;
	int tlen, off;
	struct tcpcb *otp = NULL, *tp = NULL;
	int tiflags;
	struct socket *so = NULL;
	int todrop, acked, ourfinisacked;
	int hdroptlen = 0;
	short ostate;
	caddr_t saveti;
	tcp_seq iss, *reuse = NULL;
	u_long tiwin;
	struct tcp_opt_info opti;
	struct tcphdr *th;
#ifdef INET6
	struct ip6_hdr *ip6 = NULL;
#endif /* INET6 */
#ifdef IPSEC
	struct m_tag *mtag;
	struct tdb_ident *tdbi;
	struct tdb *tdb;
	int error;
#endif /* IPSEC */
#ifdef TCP_ECN
	u_char iptos;
#endif

	tcpstat_inc(tcps_rcvtotal);

	opti.ts_present = 0;
	opti.maxseg = 0;

	/*
	 * RFC1122 4.2.3.10, p. 104: discard bcast/mcast SYN
	 */
	if (m->m_flags & (M_BCAST|M_MCAST))
		goto drop;

	/*
	 * Get IP and TCP header together in first mbuf.
	 * Note: IP leaves IP header in first mbuf.
	 */
	IP6_EXTHDR_GET(th, struct tcphdr *, m, iphlen, sizeof(*th));
	if (!th) {
		tcpstat_inc(tcps_rcvshort);
		return IPPROTO_DONE;
	}

	tlen = m->m_pkthdr.len - iphlen;
	switch (af) {
	case AF_INET:
		ip = mtod(m, struct ip *);
#ifdef TCP_ECN
		/* save ip_tos before clearing it for checksum */
		iptos = ip->ip_tos;
#endif
		break;
#ifdef INET6
	case AF_INET6:
		ip6 = mtod(m, struct ip6_hdr *);
#ifdef TCP_ECN
		iptos = (ntohl(ip6->ip6_flow) >> 20) & 0xff;
#endif

		/*
		 * Be proactive about unspecified IPv6 address in source.
		 * As we use all-zero to indicate unbounded/unconnected pcb,
		 * unspecified IPv6 address can be used to confuse us.
		 *
		 * Note that packets with unspecified IPv6 destination is
		 * already dropped in ip6_input.
		 */
		if (IN6_IS_ADDR_UNSPECIFIED(&ip6->ip6_src)) {
			/* XXX stat */
			goto drop;
		}

		/* Discard packets to multicast */
		if (IN6_IS_ADDR_MULTICAST(&ip6->ip6_dst)) {
			/* XXX stat */
			goto drop;
		}
		break;
#endif
	default:
		unhandled_af(af);
	}

	/*
	 * Checksum extended TCP header and data.
	 */
	if ((m->m_pkthdr.csum_flags & M_TCP_CSUM_IN_OK) == 0) {
		int sum;

		if (m->m_pkthdr.csum_flags & M_TCP_CSUM_IN_BAD) {
			tcpstat_inc(tcps_rcvbadsum);
			goto drop;
		}
		tcpstat_inc(tcps_inswcsum);
		switch (af) {
		case AF_INET:
			sum = in4_cksum(m, IPPROTO_TCP, iphlen, tlen);
			break;
#ifdef INET6
		case AF_INET6:
			sum = in6_cksum(m, IPPROTO_TCP, sizeof(struct ip6_hdr),
			    tlen);
			break;
#endif
		}
		if (sum != 0) {
			tcpstat_inc(tcps_rcvbadsum);
			goto drop;
		}
	}

	/*
	 * Check that TCP offset makes sense,
	 * pull out TCP options and adjust length.		XXX
	 */
	off = th->th_off << 2;
	if (off < sizeof(struct tcphdr) || off > tlen) {
		tcpstat_inc(tcps_rcvbadoff);
		goto drop;
	}
	tlen -= off;
	if (off > sizeof(struct tcphdr)) {
		IP6_EXTHDR_GET(th, struct tcphdr *, m, iphlen, off);
		if (!th) {
			tcpstat_inc(tcps_rcvshort);
			return IPPROTO_DONE;
		}
		optlen = off - sizeof(struct tcphdr);
		optp = (u_int8_t *)(th + 1);
		/*
		 * Do quick retrieval of timestamp options ("options
		 * prediction?").  If timestamp is the only option and it's
		 * formatted as recommended in RFC 1323 appendix A, we
		 * quickly get the values now and not bother calling
		 * tcp_dooptions(), etc.
		 */
		if ((optlen == TCPOLEN_TSTAMP_APPA ||
		     (optlen > TCPOLEN_TSTAMP_APPA &&
			optp[TCPOLEN_TSTAMP_APPA] == TCPOPT_EOL)) &&
		     *(u_int32_t *)optp == htonl(TCPOPT_TSTAMP_HDR) &&
		     (th->th_flags & TH_SYN) == 0) {
			opti.ts_present = 1;
			opti.ts_val = ntohl(*(u_int32_t *)(optp + 4));
			opti.ts_ecr = ntohl(*(u_int32_t *)(optp + 8));
			optp = NULL;	/* we've parsed the options */
		}
	}
	tiflags = th->th_flags;

	/*
	 * Convert TCP protocol specific fields to host format.
	 */
	th->th_seq = ntohl(th->th_seq);
	th->th_ack = ntohl(th->th_ack);
	th->th_win = ntohs(th->th_win);
	th->th_urp = ntohs(th->th_urp);

	/*
	 * Locate pcb for segment.
	 */
#if NPF > 0
	inp = pf_inp_lookup(m);
#endif
findpcb:
	if (inp == NULL) {
		switch (af) {
#ifdef INET6
		case AF_INET6:
			inp = in6_pcbhashlookup(&tcbtable, &ip6->ip6_src,
			    th->th_sport, &ip6->ip6_dst, th->th_dport,
			    m->m_pkthdr.ph_rtableid);
			break;
#endif
		case AF_INET:
			inp = in_pcbhashlookup(&tcbtable, ip->ip_src,
			    th->th_sport, ip->ip_dst, th->th_dport,
			    m->m_pkthdr.ph_rtableid);
			break;
		}
	}
	if (inp == NULL) {
		tcpstat_inc(tcps_pcbhashmiss);
		switch (af) {
#ifdef INET6
		case AF_INET6:
			inp = in6_pcblookup_listen(&tcbtable, &ip6->ip6_dst,
			    th->th_dport, m, m->m_pkthdr.ph_rtableid);
			break;
#endif /* INET6 */
		case AF_INET:
			inp = in_pcblookup_listen(&tcbtable, ip->ip_dst,
			    th->th_dport, m, m->m_pkthdr.ph_rtableid);
			break;
		}
		/*
		 * If the state is CLOSED (i.e., TCB does not exist) then
		 * all data in the incoming segment is discarded.
		 * If the TCB exists but is in CLOSED state, it is embryonic,
		 * but should either do a listen or a connect soon.
		 */
		if (inp == NULL) {
			tcpstat_inc(tcps_noport);
			goto dropwithreset_ratelim;
		}
	}
	KASSERT(sotoinpcb(inp->inp_socket) == inp);
	KASSERT(intotcpcb(inp) == NULL || intotcpcb(inp)->t_inpcb == inp);
	soassertlocked(inp->inp_socket);

	/* Check the minimum TTL for socket. */
	switch (af) {
	case AF_INET:
		if (inp->inp_ip_minttl && inp->inp_ip_minttl > ip->ip_ttl)
			goto drop;
		break;
#ifdef INET6
	case AF_INET6:
		if (inp->inp_ip6_minhlim &&
		    inp->inp_ip6_minhlim > ip6->ip6_hlim)
			goto drop;
		break;
#endif
	}

	tp = intotcpcb(inp);
	if (tp == NULL)
		goto dropwithreset_ratelim;
	if (tp->t_state == TCPS_CLOSED)
		goto drop;

	/* Unscale the window into a 32-bit value. */
	if ((tiflags & TH_SYN) == 0)
		tiwin = th->th_win << tp->snd_scale;
	else
		tiwin = th->th_win;

	so = inp->inp_socket;
	if (so->so_options & (SO_DEBUG|SO_ACCEPTCONN)) {
		union syn_cache_sa src;
		union syn_cache_sa dst;

		bzero(&src, sizeof(src));
		bzero(&dst, sizeof(dst));
		switch (af) {
		case AF_INET:
			src.sin.sin_len = sizeof(struct sockaddr_in);
			src.sin.sin_family = AF_INET;
			src.sin.sin_addr = ip->ip_src;
			src.sin.sin_port = th->th_sport;

			dst.sin.sin_len = sizeof(struct sockaddr_in);
			dst.sin.sin_family = AF_INET;
			dst.sin.sin_addr = ip->ip_dst;
			dst.sin.sin_port = th->th_dport;
			break;
#ifdef INET6
		case AF_INET6:
			src.sin6.sin6_len = sizeof(struct sockaddr_in6);
			src.sin6.sin6_family = AF_INET6;
			src.sin6.sin6_addr = ip6->ip6_src;
			src.sin6.sin6_port = th->th_sport;

			dst.sin6.sin6_len = sizeof(struct sockaddr_in6);
			dst.sin6.sin6_family = AF_INET6;
			dst.sin6.sin6_addr = ip6->ip6_dst;
			dst.sin6.sin6_port = th->th_dport;
			break;
#endif /* INET6 */
		}

		if (so->so_options & SO_DEBUG) {
			otp = tp;
			ostate = tp->t_state;
			switch (af) {
#ifdef INET6
			case AF_INET6:
				saveti = (caddr_t) &tcp_saveti6;
				memcpy(&tcp_saveti6.ti6_i, ip6, sizeof(*ip6));
				memcpy(&tcp_saveti6.ti6_t, th, sizeof(*th));
				break;
#endif
			case AF_INET:
				saveti = (caddr_t) &tcp_saveti;
				memcpy(&tcp_saveti.ti_i, ip, sizeof(*ip));
				memcpy(&tcp_saveti.ti_t, th, sizeof(*th));
				break;
			}
		}
		if (so->so_options & SO_ACCEPTCONN) {
			switch (tiflags & (TH_RST|TH_SYN|TH_ACK)) {

			case TH_SYN|TH_ACK|TH_RST:
			case TH_SYN|TH_RST:
			case TH_ACK|TH_RST:
			case TH_RST:
				syn_cache_reset(&src.sa, &dst.sa, th,
				    inp->inp_rtableid);
				goto drop;

			case TH_SYN|TH_ACK:
				/*
				 * Received a SYN,ACK.  This should
				 * never happen while we are in
				 * LISTEN.  Send an RST.
				 */
				goto badsyn;

			case TH_ACK:
				so = syn_cache_get(&src.sa, &dst.sa,
					th, iphlen, tlen, so, m);
				if (so == NULL) {
					/*
					 * We don't have a SYN for
					 * this ACK; send an RST.
					 */
					goto badsyn;
				} else if (so == (struct socket *)(-1)) {
					/*
					 * We were unable to create
					 * the connection.  If the
					 * 3-way handshake was
					 * completed, and RST has
					 * been sent to the peer.
					 * Since the mbuf might be
					 * in use for the reply,
					 * do not free it.
					 */
					m = *mp = NULL;
					goto drop;
				} else {
					/*
					 * We have created a
					 * full-blown connection.
					 */
					tp = NULL;
					inp = sotoinpcb(so);
					tp = intotcpcb(inp);
					if (tp == NULL)
						goto badsyn;	/*XXX*/

				}
				break;

			default:
				/*
				 * None of RST, SYN or ACK was set.
				 * This is an invalid packet for a
				 * TCB in LISTEN state.  Send a RST.
				 */
				goto badsyn;

			case TH_SYN:
				/*
				 * Received a SYN.
				 */
#ifdef INET6
				/*
				 * If deprecated address is forbidden, we do
				 * not accept SYN to deprecated interface
				 * address to prevent any new inbound
				 * connection from getting established.
				 * When we do not accept SYN, we send a TCP
				 * RST, with deprecated source address (instead
				 * of dropping it).  We compromise it as it is
				 * much better for peer to send a RST, and
				 * RST will be the final packet for the
				 * exchange.
				 *
				 * If we do not forbid deprecated addresses, we
				 * accept the SYN packet.  RFC2462 does not
				 * suggest dropping SYN in this case.
				 * If we decipher RFC2462 5.5.4, it says like
				 * this:
				 * 1. use of deprecated addr with existing
				 *    communication is okay - "SHOULD continue
				 *    to be used"
				 * 2. use of it with new communication:
				 *   (2a) "SHOULD NOT be used if alternate
				 *        address with sufficient scope is
				 *        available"
				 *   (2b) nothing mentioned otherwise.
				 * Here we fall into (2b) case as we have no
				 * choice in our source address selection - we
				 * must obey the peer.
				 *
				 * The wording in RFC2462 is confusing, and
				 * there are multiple description text for
				 * deprecated address handling - worse, they
				 * are not exactly the same.  I believe 5.5.4
				 * is the best one, so we follow 5.5.4.
				 */
				if (ip6 && !ip6_use_deprecated) {
					struct in6_ifaddr *ia6;
					struct ifnet *ifp =
					    if_get(m->m_pkthdr.ph_ifidx);

					if (ifp &&
					    (ia6 = in6ifa_ifpwithaddr(ifp,
					    &ip6->ip6_dst)) &&
					    (ia6->ia6_flags &
					    IN6_IFF_DEPRECATED)) {
						tp = NULL;
						if_put(ifp);
						goto dropwithreset;
					}
					if_put(ifp);
				}
#endif

				/*
				 * LISTEN socket received a SYN
				 * from itself?  This can't possibly
				 * be valid; drop the packet.
				 */
				if (th->th_dport == th->th_sport) {
					switch (af) {
#ifdef INET6
					case AF_INET6:
						if (IN6_ARE_ADDR_EQUAL(&ip6->ip6_src,
						    &ip6->ip6_dst)) {
							tcpstat_inc(tcps_badsyn);
							goto drop;
						}
						break;
#endif /* INET6 */
					case AF_INET:
						if (ip->ip_dst.s_addr == ip->ip_src.s_addr) {
							tcpstat_inc(tcps_badsyn);
							goto drop;
						}
						break;
					}
				}

				/*
				 * SYN looks ok; create compressed TCP
				 * state for it.
				 */
				if (so->so_qlen > so->so_qlimit ||
				    syn_cache_add(&src.sa, &dst.sa, th, iphlen,
				    so, m, optp, optlen, &opti, reuse) == -1) {
					tcpstat_inc(tcps_dropsyn);
					goto drop;
				}
				return IPPROTO_DONE;
			}
		}
	}

#ifdef DIAGNOSTIC
	/*
	 * Should not happen now that all embryonic connections
	 * are handled with compressed state.
	 */
	if (tp->t_state == TCPS_LISTEN)
		panic("tcp_input: TCPS_LISTEN");
#endif

#if NPF > 0
	pf_inp_link(m, inp);
#endif

#ifdef IPSEC
	/* Find most recent IPsec tag */
	mtag = m_tag_find(m, PACKET_TAG_IPSEC_IN_DONE, NULL);
	if (mtag != NULL) {
		tdbi = (struct tdb_ident *)(mtag + 1);
	        tdb = gettdb(tdbi->rdomain, tdbi->spi,
		    &tdbi->dst, tdbi->proto);
	} else
		tdb = NULL;
	ipsp_spd_lookup(m, af, iphlen, &error, IPSP_DIRECTION_IN,
	    tdb, inp, 0);
	if (error) {
		tcpstat_inc(tcps_rcvnosec);
		goto drop;
	}
#endif /* IPSEC */

	/*
	 * Segment received on connection.
	 * Reset idle time and keep-alive timer.
	 */
	tp->t_rcvtime = tcp_now;
	if (TCPS_HAVEESTABLISHED(tp->t_state))
		TCP_TIMER_ARM(tp, TCPT_KEEP, tcp_keepidle);

	if (tp->sack_enable)
		tcp_del_sackholes(tp, th); /* Delete stale SACK holes */

	/*
	 * Process options.
	 */
#ifdef TCP_SIGNATURE
	if (optp || (tp->t_flags & TF_SIGNATURE))
#else
	if (optp)
#endif
		if (tcp_dooptions(tp, optp, optlen, th, m, iphlen, &opti,
		    m->m_pkthdr.ph_rtableid))
			goto drop;

	if (opti.ts_present && opti.ts_ecr) {
		int rtt_test;

		/* subtract out the tcp timestamp modulator */
		opti.ts_ecr -= tp->ts_modulate;

		/* make sure ts_ecr is sensible */
		rtt_test = tcp_now - opti.ts_ecr;
		if (rtt_test < 0 || rtt_test > TCP_RTT_MAX)
			opti.ts_ecr = 0;
	}

#ifdef TCP_ECN
	/* if congestion experienced, set ECE bit in subsequent packets. */
	if ((iptos & IPTOS_ECN_MASK) == IPTOS_ECN_CE) {
		tp->t_flags |= TF_RCVD_CE;
		tcpstat_inc(tcps_ecn_rcvce);
	}
#endif
	/*
	 * Header prediction: check for the two common cases
	 * of a uni-directional data xfer.  If the packet has
	 * no control flags, is in-sequence, the window didn't
	 * change and we're not retransmitting, it's a
	 * candidate.  If the length is zero and the ack moved
	 * forward, we're the sender side of the xfer.  Just
	 * free the data acked & wake any higher level process
	 * that was blocked waiting for space.  If the length
	 * is non-zero and the ack didn't move, we're the
	 * receiver side.  If we're getting packets in-order
	 * (the reassembly queue is empty), add the data to
	 * the socket buffer and note that we need a delayed ack.
	 */
	if (tp->t_state == TCPS_ESTABLISHED &&
#ifdef TCP_ECN
	    (tiflags & (TH_SYN|TH_FIN|TH_RST|TH_URG|TH_ECE|TH_CWR|TH_ACK)) == TH_ACK &&
#else
	    (tiflags & (TH_SYN|TH_FIN|TH_RST|TH_URG|TH_ACK)) == TH_ACK &&
#endif
	    (!opti.ts_present || TSTMP_GEQ(opti.ts_val, tp->ts_recent)) &&
	    th->th_seq == tp->rcv_nxt &&
	    tiwin && tiwin == tp->snd_wnd &&
	    tp->snd_nxt == tp->snd_max) {

		/*
		 * If last ACK falls within this segment's sequence numbers,
		 *  record the timestamp.
		 * Fix from Braden, see Stevens p. 870
		 */
		if (opti.ts_present && SEQ_LEQ(th->th_seq, tp->last_ack_sent)) {
			tp->ts_recent_age = tcp_now;
			tp->ts_recent = opti.ts_val;
		}

		if (tlen == 0) {
			if (SEQ_GT(th->th_ack, tp->snd_una) &&
			    SEQ_LEQ(th->th_ack, tp->snd_max) &&
			    tp->snd_cwnd >= tp->snd_wnd &&
			    tp->t_dupacks == 0) {
				/*
				 * this is a pure ack for outstanding data.
				 */
				tcpstat_inc(tcps_predack);
				if (opti.ts_present && opti.ts_ecr)
					tcp_xmit_timer(tp, tcp_now - opti.ts_ecr);
				else if (tp->t_rtttime &&
				    SEQ_GT(th->th_ack, tp->t_rtseq))
					tcp_xmit_timer(tp,
					    tcp_now - tp->t_rtttime);
				acked = th->th_ack - tp->snd_una;
				tcpstat_pkt(tcps_rcvackpack, tcps_rcvackbyte,
				    acked);
				ND6_HINT(tp);
				sbdrop(so, &so->so_snd, acked);

				/*
				 * If we had a pending ICMP message that
				 * refers to data that have just been
				 * acknowledged, disregard the recorded ICMP
				 * message.
				 */
				if ((tp->t_flags & TF_PMTUD_PEND) &&
				    SEQ_GT(th->th_ack, tp->t_pmtud_th_seq))
					tp->t_flags &= ~TF_PMTUD_PEND;

				/*
				 * Keep track of the largest chunk of data
				 * acknowledged since last PMTU update
				 */
				if (tp->t_pmtud_mss_acked < acked)
					tp->t_pmtud_mss_acked = acked;

				tp->snd_una = th->th_ack;
				/*
				 * We want snd_last to track snd_una so
				 * as to avoid sequence wraparound problems
				 * for very large transfers.
				 */
#ifdef TCP_ECN
				if (SEQ_GT(tp->snd_una, tp->snd_last))
#endif
				tp->snd_last = tp->snd_una;
				m_freem(m);

				/*
				 * If all outstanding data are acked, stop
				 * retransmit timer, otherwise restart timer
				 * using current (possibly backed-off) value.
				 * If process is waiting for space,
				 * wakeup/selwakeup/signal.  If data
				 * are ready to send, let tcp_output
				 * decide between more output or persist.
				 */
				if (tp->snd_una == tp->snd_max)
					TCP_TIMER_DISARM(tp, TCPT_REXMT);
				else if (TCP_TIMER_ISARMED(tp, TCPT_PERSIST) == 0)
					TCP_TIMER_ARM(tp, TCPT_REXMT, tp->t_rxtcur);

				tcp_update_sndspace(tp);
				if (sb_notify(so, &so->so_snd)) {
					tp->t_flags |= TF_BLOCKOUTPUT;
					sowwakeup(so);
					tp->t_flags &= ~TF_BLOCKOUTPUT;
				}
				if (so->so_snd.sb_cc ||
				    tp->t_flags & TF_NEEDOUTPUT)
					(void) tcp_output(tp);
				return IPPROTO_DONE;
			}
		} else if (th->th_ack == tp->snd_una &&
		    TAILQ_EMPTY(&tp->t_segq) &&
		    tlen <= sbspace(so, &so->so_rcv)) {
			/*
			 * This is a pure, in-sequence data packet
			 * with nothing on the reassembly queue and
			 * we have enough buffer space to take it.
			 */
			/* Clean receiver SACK report if present */
			if (tp->sack_enable && tp->rcv_numsacks)
				tcp_clean_sackreport(tp);
			tcpstat_inc(tcps_preddat);
			tp->rcv_nxt += tlen;
			tcpstat_pkt(tcps_rcvpack, tcps_rcvbyte, tlen);
			ND6_HINT(tp);

			TCP_SETUP_ACK(tp, tiflags, m);
			/*
			 * Drop TCP, IP headers and TCP options then add data
			 * to socket buffer.
			 */
			if (so->so_state & SS_CANTRCVMORE)
				m_freem(m);
			else {
				if (opti.ts_present && opti.ts_ecr) {
					if (tp->rfbuf_ts < opti.ts_ecr &&
					    opti.ts_ecr - tp->rfbuf_ts < hz) {
						tcp_update_rcvspace(tp);
						/* Start over with next RTT. */
						tp->rfbuf_cnt = 0;
						tp->rfbuf_ts = 0;
					} else
						tp->rfbuf_cnt += tlen;
				}
				m_adj(m, iphlen + off);
				sbappendstream(so, &so->so_rcv, m);
			}
			tp->t_flags |= TF_BLOCKOUTPUT;
			sorwakeup(so);
			tp->t_flags &= ~TF_BLOCKOUTPUT;
			if (tp->t_flags & (TF_ACKNOW|TF_NEEDOUTPUT))
				(void) tcp_output(tp);
			return IPPROTO_DONE;
		}
	}

	/*
	 * Compute mbuf offset to TCP data segment.
	 */
	hdroptlen = iphlen + off;

	/*
	 * Calculate amount of space in receive window,
	 * and then do TCP input processing.
	 * Receive window is amount of space in rcv queue,
	 * but not less than advertised window.
	 */
	{ int win;

	win = sbspace(so, &so->so_rcv);
	if (win < 0)
		win = 0;
	tp->rcv_wnd = imax(win, (int)(tp->rcv_adv - tp->rcv_nxt));
	}

	/* Reset receive buffer auto scaling when not in bulk receive mode. */
	tp->rfbuf_cnt = 0;
	tp->rfbuf_ts = 0;

	switch (tp->t_state) {

	/*
	 * If the state is SYN_RECEIVED:
	 * 	if seg contains SYN/ACK, send an RST.
	 *	if seg contains an ACK, but not for our SYN/ACK, send an RST
	 */

	case TCPS_SYN_RECEIVED:
		if (tiflags & TH_ACK) {
			if (tiflags & TH_SYN) {
				tcpstat_inc(tcps_badsyn);
				goto dropwithreset;
			}
			if (SEQ_LEQ(th->th_ack, tp->snd_una) ||
			    SEQ_GT(th->th_ack, tp->snd_max))
				goto dropwithreset;
		}
		break;

	/*
	 * If the state is SYN_SENT:
	 *	if seg contains an ACK, but not for our SYN, drop the input.
	 *	if seg contains a RST, then drop the connection.
	 *	if seg does not contain SYN, then drop it.
	 * Otherwise this is an acceptable SYN segment
	 *	initialize tp->rcv_nxt and tp->irs
	 *	if seg contains ack then advance tp->snd_una
	 *	if SYN has been acked change to ESTABLISHED else SYN_RCVD state
	 *	arrange for segment to be acked (eventually)
	 *	continue processing rest of data/controls, beginning with URG
	 */
	case TCPS_SYN_SENT:
		if ((tiflags & TH_ACK) &&
		    (SEQ_LEQ(th->th_ack, tp->iss) ||
		     SEQ_GT(th->th_ack, tp->snd_max)))
			goto dropwithreset;
		if (tiflags & TH_RST) {
#ifdef TCP_ECN
			/* if ECN is enabled, fall back to non-ecn at rexmit */
			if (tcp_do_ecn && !(tp->t_flags & TF_DISABLE_ECN))
				goto drop;
#endif
			if (tiflags & TH_ACK)
				tp = tcp_drop(tp, ECONNREFUSED);
			goto drop;
		}
		if ((tiflags & TH_SYN) == 0)
			goto drop;
		if (tiflags & TH_ACK) {
			tp->snd_una = th->th_ack;
			if (SEQ_LT(tp->snd_nxt, tp->snd_una))
				tp->snd_nxt = tp->snd_una;
		}
		TCP_TIMER_DISARM(tp, TCPT_REXMT);
		tp->irs = th->th_seq;
		tcp_mss(tp, opti.maxseg);
		/* Reset initial window to 1 segment for retransmit */
		if (tp->t_rxtshift > 0)
			tp->snd_cwnd = tp->t_maxseg;
		tcp_rcvseqinit(tp);
		tp->t_flags |= TF_ACKNOW;
                /*
                 * If we've sent a SACK_PERMITTED option, and the peer
                 * also replied with one, then TF_SACK_PERMIT should have
                 * been set in tcp_dooptions().  If it was not, disable SACKs.
                 */
		if (tp->sack_enable)
			tp->sack_enable = tp->t_flags & TF_SACK_PERMIT;
#ifdef TCP_ECN
		/*
		 * if ECE is set but CWR is not set for SYN-ACK, or
		 * both ECE and CWR are set for simultaneous open,
		 * peer is ECN capable.
		 */
		if (tcp_do_ecn) {
			switch (tiflags & (TH_ACK|TH_ECE|TH_CWR)) {
			case TH_ACK|TH_ECE:
			case TH_ECE|TH_CWR:
				tp->t_flags |= TF_ECN_PERMIT;
				tiflags &= ~(TH_ECE|TH_CWR);
				tcpstat_inc(tcps_ecn_accepts);
			}
		}
#endif

		if (tiflags & TH_ACK && SEQ_GT(tp->snd_una, tp->iss)) {
			tcpstat_inc(tcps_connects);
			tp->t_flags |= TF_BLOCKOUTPUT;
			soisconnected(so);
			tp->t_flags &= ~TF_BLOCKOUTPUT;
			tp->t_state = TCPS_ESTABLISHED;
			TCP_TIMER_ARM(tp, TCPT_KEEP, tcp_keepidle);
			/* Do window scaling on this connection? */
			if ((tp->t_flags & (TF_RCVD_SCALE|TF_REQ_SCALE)) ==
				(TF_RCVD_SCALE|TF_REQ_SCALE)) {
				tp->snd_scale = tp->requested_s_scale;
				tp->rcv_scale = tp->request_r_scale;
			}
			tcp_flush_queue(tp);

			/*
			 * if we didn't have to retransmit the SYN,
			 * use its rtt as our initial srtt & rtt var.
			 */
			if (tp->t_rtttime)
				tcp_xmit_timer(tp, tcp_now - tp->t_rtttime);
			/*
			 * Since new data was acked (the SYN), open the
			 * congestion window by one MSS.  We do this
			 * here, because we won't go through the normal
			 * ACK processing below.  And since this is the
			 * start of the connection, we know we are in
			 * the exponential phase of slow-start.
			 */
			tp->snd_cwnd += tp->t_maxseg;
		} else
			tp->t_state = TCPS_SYN_RECEIVED;

#if 0
trimthenstep6:
#endif
		/*
		 * Advance th->th_seq to correspond to first data byte.
		 * If data, trim to stay within window,
		 * dropping FIN if necessary.
		 */
		th->th_seq++;
		if (tlen > tp->rcv_wnd) {
			todrop = tlen - tp->rcv_wnd;
			m_adj(m, -todrop);
			tlen = tp->rcv_wnd;
			tiflags &= ~TH_FIN;
			tcpstat_pkt(tcps_rcvpackafterwin, tcps_rcvbyteafterwin,
			    todrop);
		}
		tp->snd_wl1 = th->th_seq - 1;
		tp->rcv_up = th->th_seq;
		goto step6;
	/*
	 * If a new connection request is received while in TIME_WAIT,
	 * drop the old connection and start over if the if the
	 * timestamp or the sequence numbers are above the previous
	 * ones.
	 */
	case TCPS_TIME_WAIT:
		if (((tiflags & (TH_SYN|TH_ACK)) == TH_SYN) &&
		    ((opti.ts_present &&
		    TSTMP_LT(tp->ts_recent, opti.ts_val)) ||
		    SEQ_GT(th->th_seq, tp->rcv_nxt))) {
#if NPF > 0
			/*
			 * The socket will be recreated but the new state
			 * has already been linked to the socket.  Remove the
			 * link between old socket and new state.
			 */
			pf_inp_unlink(inp);
#endif
			/*
			* Advance the iss by at least 32768, but
			* clear the msb in order to make sure
			* that SEG_LT(snd_nxt, iss).
			*/
			iss = tp->snd_nxt +
			    ((arc4random() & 0x7fffffff) | 0x8000);
			reuse = &iss;
			tp = tcp_close(tp);
			inp = NULL;
			goto findpcb;
		}
	}

	/*
	 * States other than LISTEN or SYN_SENT.
	 * First check timestamp, if present.
	 * Then check that at least some bytes of segment are within
	 * receive window.  If segment begins before rcv_nxt,
	 * drop leading data (and SYN); if nothing left, just ack.
	 *
	 * RFC 1323 PAWS: If we have a timestamp reply on this segment
	 * and it's less than opti.ts_recent, drop it.
	 */
	if (opti.ts_present && (tiflags & TH_RST) == 0 && tp->ts_recent &&
	    TSTMP_LT(opti.ts_val, tp->ts_recent)) {

		/* Check to see if ts_recent is over 24 days old.  */
		if ((int)(tcp_now - tp->ts_recent_age) > TCP_PAWS_IDLE) {
			/*
			 * Invalidate ts_recent.  If this segment updates
			 * ts_recent, the age will be reset later and ts_recent
			 * will get a valid value.  If it does not, setting
			 * ts_recent to zero will at least satisfy the
			 * requirement that zero be placed in the timestamp
			 * echo reply when ts_recent isn't valid.  The
			 * age isn't reset until we get a valid ts_recent
			 * because we don't want out-of-order segments to be
			 * dropped when ts_recent is old.
			 */
			tp->ts_recent = 0;
		} else {
			tcpstat_pkt(tcps_rcvduppack, tcps_rcvdupbyte, tlen);
			tcpstat_inc(tcps_pawsdrop);
			if (tlen)
				goto dropafterack;
			goto drop;
		}
	}

	todrop = tp->rcv_nxt - th->th_seq;
	if (todrop > 0) {
		if (tiflags & TH_SYN) {
			tiflags &= ~TH_SYN;
			th->th_seq++;
			if (th->th_urp > 1)
				th->th_urp--;
			else
				tiflags &= ~TH_URG;
			todrop--;
		}
		if (todrop > tlen ||
		    (todrop == tlen && (tiflags & TH_FIN) == 0)) {
			/*
			 * Any valid FIN must be to the left of the
			 * window.  At this point, FIN must be a
			 * duplicate or out-of-sequence, so drop it.
			 */
			tiflags &= ~TH_FIN;
			/*
			 * Send ACK to resynchronize, and drop any data,
			 * but keep on processing for RST or ACK.
			 */
			tp->t_flags |= TF_ACKNOW;
			todrop = tlen;
			tcpstat_pkt(tcps_rcvduppack, tcps_rcvdupbyte, todrop);
		} else {
			tcpstat_pkt(tcps_rcvpartduppack, tcps_rcvpartdupbyte,
			    todrop);
		}
		hdroptlen += todrop;	/* drop from head afterwards */
		th->th_seq += todrop;
		tlen -= todrop;
		if (th->th_urp > todrop)
			th->th_urp -= todrop;
		else {
			tiflags &= ~TH_URG;
			th->th_urp = 0;
		}
	}

	/*
	 * If new data are received on a connection after the
	 * user processes are gone, then RST the other end.
	 */
	if ((so->so_state & SS_NOFDREF) &&
	    tp->t_state > TCPS_CLOSE_WAIT && tlen) {
		tp = tcp_close(tp);
		tcpstat_inc(tcps_rcvafterclose);
		goto dropwithreset;
	}

	/*
	 * If segment ends after window, drop trailing data
	 * (and PUSH and FIN); if nothing left, just ACK.
	 */
	todrop = (th->th_seq + tlen) - (tp->rcv_nxt+tp->rcv_wnd);
	if (todrop > 0) {
		tcpstat_inc(tcps_rcvpackafterwin);
		if (todrop >= tlen) {
			tcpstat_add(tcps_rcvbyteafterwin, tlen);
			/*
			 * If window is closed can only take segments at
			 * window edge, and have to drop data and PUSH from
			 * incoming segments.  Continue processing, but
			 * remember to ack.  Otherwise, drop segment
			 * and ack.
			 */
			if (tp->rcv_wnd == 0 && th->th_seq == tp->rcv_nxt) {
				tp->t_flags |= TF_ACKNOW;
				tcpstat_inc(tcps_rcvwinprobe);
			} else
				goto dropafterack;
		} else
			tcpstat_add(tcps_rcvbyteafterwin, todrop);
		m_adj(m, -todrop);
		tlen -= todrop;
		tiflags &= ~(TH_PUSH|TH_FIN);
	}

	/*
	 * If last ACK falls within this segment's sequence numbers,
	 * record its timestamp if it's more recent.
	 * NOTE that the test is modified according to the latest
	 * proposal of the tcplw@cray.com list (Braden 1993/04/26).
	 */
	if (opti.ts_present && TSTMP_GEQ(opti.ts_val, tp->ts_recent) &&
	    SEQ_LEQ(th->th_seq, tp->last_ack_sent)) {
		tp->ts_recent_age = tcp_now;
		tp->ts_recent = opti.ts_val;
	}

	/*
	 * If the RST bit is set examine the state:
	 *    SYN_RECEIVED STATE:
	 *	If passive open, return to LISTEN state.
	 *	If active open, inform user that connection was refused.
	 *    ESTABLISHED, FIN_WAIT_1, FIN_WAIT2, CLOSE_WAIT STATES:
	 *	Inform user that connection was reset, and close tcb.
	 *    CLOSING, LAST_ACK, TIME_WAIT STATES
	 *	Close the tcb.
	 */
	if (tiflags & TH_RST) {
		if (th->th_seq != tp->last_ack_sent &&
		    th->th_seq != tp->rcv_nxt &&
		    th->th_seq != (tp->rcv_nxt + 1))
			goto drop;

		switch (tp->t_state) {
		case TCPS_SYN_RECEIVED:
#ifdef TCP_ECN
			/* if ECN is enabled, fall back to non-ecn at rexmit */
			if (tcp_do_ecn && !(tp->t_flags & TF_DISABLE_ECN))
				goto drop;
#endif
			so->so_error = ECONNREFUSED;
			goto close;

		case TCPS_ESTABLISHED:
		case TCPS_FIN_WAIT_1:
		case TCPS_FIN_WAIT_2:
		case TCPS_CLOSE_WAIT:
			so->so_error = ECONNRESET;
		close:
			tp->t_state = TCPS_CLOSED;
			tcpstat_inc(tcps_drops);
			tp = tcp_close(tp);
			goto drop;
		case TCPS_CLOSING:
		case TCPS_LAST_ACK:
		case TCPS_TIME_WAIT:
			tp = tcp_close(tp);
			goto drop;
		}
	}

	/*
	 * If a SYN is in the window, then this is an
	 * error and we ACK and drop the packet.
	 */
	if (tiflags & TH_SYN)
		goto dropafterack_ratelim;

	/*
	 * If the ACK bit is off we drop the segment and return.
	 */
	if ((tiflags & TH_ACK) == 0) {
		if (tp->t_flags & TF_ACKNOW)
			goto dropafterack;
		else
			goto drop;
	}

	/*
	 * Ack processing.
	 */
	switch (tp->t_state) {

	/*
	 * In SYN_RECEIVED state, the ack ACKs our SYN, so enter
	 * ESTABLISHED state and continue processing.
	 * The ACK was checked above.
	 */
	case TCPS_SYN_RECEIVED:
		tcpstat_inc(tcps_connects);
		tp->t_flags |= TF_BLOCKOUTPUT;
		soisconnected(so);
		tp->t_flags &= ~TF_BLOCKOUTPUT;
		tp->t_state = TCPS_ESTABLISHED;
		TCP_TIMER_ARM(tp, TCPT_KEEP, tcp_keepidle);
		/* Do window scaling? */
		if ((tp->t_flags & (TF_RCVD_SCALE|TF_REQ_SCALE)) ==
			(TF_RCVD_SCALE|TF_REQ_SCALE)) {
			tp->snd_scale = tp->requested_s_scale;
			tp->rcv_scale = tp->request_r_scale;
			tiwin = th->th_win << tp->snd_scale;
		}
		tcp_flush_queue(tp);
		tp->snd_wl1 = th->th_seq - 1;
		/* fall into ... */

	/*
	 * In ESTABLISHED state: drop duplicate ACKs; ACK out of range
	 * ACKs.  If the ack is in the range
	 *	tp->snd_una < th->th_ack <= tp->snd_max
	 * then advance tp->snd_una to th->th_ack and drop
	 * data from the retransmission queue.  If this ACK reflects
	 * more up to date window information we update our window information.
	 */
	case TCPS_ESTABLISHED:
	case TCPS_FIN_WAIT_1:
	case TCPS_FIN_WAIT_2:
	case TCPS_CLOSE_WAIT:
	case TCPS_CLOSING:
	case TCPS_LAST_ACK:
	case TCPS_TIME_WAIT:
#ifdef TCP_ECN
		/*
		 * if we receive ECE and are not already in recovery phase,
		 * reduce cwnd by half but don't slow-start.
		 * advance snd_last to snd_max not to reduce cwnd again
		 * until all outstanding packets are acked.
		 */
		if (tcp_do_ecn && (tiflags & TH_ECE)) {
			if ((tp->t_flags & TF_ECN_PERMIT) &&
			    SEQ_GEQ(tp->snd_una, tp->snd_last)) {
				u_int win;

				win = min(tp->snd_wnd, tp->snd_cwnd) / tp->t_maxseg;
				if (win > 1) {
					tp->snd_ssthresh = win / 2 * tp->t_maxseg;
					tp->snd_cwnd = tp->snd_ssthresh;
					tp->snd_last = tp->snd_max;
					tp->t_flags |= TF_SEND_CWR;
					tcpstat_inc(tcps_cwr_ecn);
				}
			}
			tcpstat_inc(tcps_ecn_rcvece);
		}
		/*
		 * if we receive CWR, we know that the peer has reduced
		 * its congestion window.  stop sending ecn-echo.
		 */
		if ((tiflags & TH_CWR)) {
			tp->t_flags &= ~TF_RCVD_CE;
			tcpstat_inc(tcps_ecn_rcvcwr);
		}
#endif /* TCP_ECN */

		if (SEQ_LEQ(th->th_ack, tp->snd_una)) {
			/*
			 * Duplicate/old ACK processing.
			 * Increments t_dupacks:
			 *	Pure duplicate (same seq/ack/window, no data)
			 * Doesn't affect t_dupacks:
			 *	Data packets.
			 *	Normal window updates (window opens)
			 * Resets t_dupacks:
			 *	New data ACKed.
			 *	Window shrinks
			 *	Old ACK
			 */
			if (tlen) {
				/* Drop very old ACKs unless th_seq matches */
				if (th->th_seq != tp->rcv_nxt &&
				   SEQ_LT(th->th_ack,
				   tp->snd_una - tp->max_sndwnd)) {
					tcpstat_inc(tcps_rcvacktooold);
					goto drop;
				}
				break;
			}
			/*
			 * If we get an old ACK, there is probably packet
			 * reordering going on.  Be conservative and reset
			 * t_dupacks so that we are less aggressive in
			 * doing a fast retransmit.
			 */
			if (th->th_ack != tp->snd_una) {
				tp->t_dupacks = 0;
				break;
			}
			if (tiwin == tp->snd_wnd) {
				tcpstat_inc(tcps_rcvdupack);
				/*
				 * If we have outstanding data (other than
				 * a window probe), this is a completely
				 * duplicate ack (ie, window info didn't
				 * change), the ack is the biggest we've
				 * seen and we've seen exactly our rexmt
				 * threshold of them, assume a packet
				 * has been dropped and retransmit it.
				 * Kludge snd_nxt & the congestion
				 * window so we send only this one
				 * packet.
				 *
				 * We know we're losing at the current
				 * window size so do congestion avoidance
				 * (set ssthresh to half the current window
				 * and pull our congestion window back to
				 * the new ssthresh).
				 *
				 * Dup acks mean that packets have left the
				 * network (they're now cached at the receiver)
				 * so bump cwnd by the amount in the receiver
				 * to keep a constant cwnd packets in the
				 * network.
				 */
				if (TCP_TIMER_ISARMED(tp, TCPT_REXMT) == 0)
					tp->t_dupacks = 0;
				else if (++tp->t_dupacks == tcprexmtthresh) {
					tcp_seq onxt = tp->snd_nxt;
					u_long win =
					    ulmin(tp->snd_wnd, tp->snd_cwnd) /
						2 / tp->t_maxseg;

					if (SEQ_LT(th->th_ack, tp->snd_last)){
						/*
						 * False fast retx after
						 * timeout.  Do not cut window.
						 */
						tp->t_dupacks = 0;
						goto drop;
					}
					if (win < 2)
						win = 2;
					tp->snd_ssthresh = win * tp->t_maxseg;
					tp->snd_last = tp->snd_max;
					if (tp->sack_enable) {
						TCP_TIMER_DISARM(tp, TCPT_REXMT);
						tp->t_rtttime = 0;
#ifdef TCP_ECN
						tp->t_flags |= TF_SEND_CWR;
#endif
						tcpstat_inc(tcps_cwr_frecovery);
						tcpstat_inc(tcps_sack_recovery_episode);
						/*
						 * tcp_output() will send
						 * oldest SACK-eligible rtx.
						 */
						(void) tcp_output(tp);
						tp->snd_cwnd = tp->snd_ssthresh+
					           tp->t_maxseg * tp->t_dupacks;
						goto drop;
					}
					TCP_TIMER_DISARM(tp, TCPT_REXMT);
					tp->t_rtttime = 0;
					tp->snd_nxt = th->th_ack;
					tp->snd_cwnd = tp->t_maxseg;
#ifdef TCP_ECN
					tp->t_flags |= TF_SEND_CWR;
#endif
					tcpstat_inc(tcps_cwr_frecovery);
					tcpstat_inc(tcps_sndrexmitfast);
					(void) tcp_output(tp);

					tp->snd_cwnd = tp->snd_ssthresh +
					    tp->t_maxseg * tp->t_dupacks;
					if (SEQ_GT(onxt, tp->snd_nxt))
						tp->snd_nxt = onxt;
					goto drop;
				} else if (tp->t_dupacks > tcprexmtthresh) {
					tp->snd_cwnd += tp->t_maxseg;
					(void) tcp_output(tp);
					goto drop;
				}
			} else if (tiwin < tp->snd_wnd) {
				/*
				 * The window was retracted!  Previous dup
				 * ACKs may have been due to packets arriving
				 * after the shrunken window, not a missing
				 * packet, so play it safe and reset t_dupacks
				 */
				tp->t_dupacks = 0;
			}
			break;
		}
		/*
		 * If the congestion window was inflated to account
		 * for the other side's cached packets, retract it.
		 */
		if (tp->t_dupacks >= tcprexmtthresh) {
			/* Check for a partial ACK */
			if (SEQ_LT(th->th_ack, tp->snd_last)) {
				if (tp->sack_enable)
					tcp_sack_partialack(tp, th);
				else
					tcp_newreno_partialack(tp, th);
			} else {
				/* Out of fast recovery */
				tp->snd_cwnd = tp->snd_ssthresh;
				if (tcp_seq_subtract(tp->snd_max, th->th_ack) <
				    tp->snd_ssthresh)
					tp->snd_cwnd =
					    tcp_seq_subtract(tp->snd_max,
					    th->th_ack);
				tp->t_dupacks = 0;
			}
		} else {
			/*
			 * Reset the duplicate ACK counter if we
			 * were not in fast recovery.
			 */
			tp->t_dupacks = 0;
		}
		if (SEQ_GT(th->th_ack, tp->snd_max)) {
			tcpstat_inc(tcps_rcvacktoomuch);
			goto dropafterack_ratelim;
		}
		acked = th->th_ack - tp->snd_una;
		tcpstat_pkt(tcps_rcvackpack, tcps_rcvackbyte, acked);

		/*
		 * If we have a timestamp reply, update smoothed
		 * round trip time.  If no timestamp is present but
		 * transmit timer is running and timed sequence
		 * number was acked, update smoothed round trip time.
		 * Since we now have an rtt measurement, cancel the
		 * timer backoff (cf., Phil Karn's retransmit alg.).
		 * Recompute the initial retransmit timer.
		 */
		if (opti.ts_present && opti.ts_ecr)
			tcp_xmit_timer(tp, tcp_now - opti.ts_ecr);
		else if (tp->t_rtttime && SEQ_GT(th->th_ack, tp->t_rtseq))
			tcp_xmit_timer(tp, tcp_now - tp->t_rtttime);

		/*
		 * If all outstanding data is acked, stop retransmit
		 * timer and remember to restart (more output or persist).
		 * If there is more data to be acked, restart retransmit
		 * timer, using current (possibly backed-off) value.
		 */
		if (th->th_ack == tp->snd_max) {
			TCP_TIMER_DISARM(tp, TCPT_REXMT);
			tp->t_flags |= TF_NEEDOUTPUT;
		} else if (TCP_TIMER_ISARMED(tp, TCPT_PERSIST) == 0)
			TCP_TIMER_ARM(tp, TCPT_REXMT, tp->t_rxtcur);
		/*
		 * When new data is acked, open the congestion window.
		 * If the window gives us less than ssthresh packets
		 * in flight, open exponentially (maxseg per packet).
		 * Otherwise open linearly: maxseg per window
		 * (maxseg^2 / cwnd per packet).
		 */
		{
		u_int cw = tp->snd_cwnd;
		u_int incr = tp->t_maxseg;

		if (cw > tp->snd_ssthresh)
			incr = incr * incr / cw;
		if (tp->t_dupacks < tcprexmtthresh)
			tp->snd_cwnd = ulmin(cw + incr,
			    TCP_MAXWIN << tp->snd_scale);
		}
		ND6_HINT(tp);
		if (acked > so->so_snd.sb_cc) {
			tp->snd_wnd -= so->so_snd.sb_cc;
			sbdrop(so, &so->so_snd, (int)so->so_snd.sb_cc);
			ourfinisacked = 1;
		} else {
			sbdrop(so, &so->so_snd, acked);
			tp->snd_wnd -= acked;
			ourfinisacked = 0;
		}

		tcp_update_sndspace(tp);
		if (sb_notify(so, &so->so_snd)) {
			tp->t_flags |= TF_BLOCKOUTPUT;
			sowwakeup(so);
			tp->t_flags &= ~TF_BLOCKOUTPUT;
		}

		/*
		 * If we had a pending ICMP message that referred to data
		 * that have just been acknowledged, disregard the recorded
		 * ICMP message.
		 */
		if ((tp->t_flags & TF_PMTUD_PEND) &&
		    SEQ_GT(th->th_ack, tp->t_pmtud_th_seq))
			tp->t_flags &= ~TF_PMTUD_PEND;

		/*
		 * Keep track of the largest chunk of data acknowledged
		 * since last PMTU update
		 */
		if (tp->t_pmtud_mss_acked < acked)
			tp->t_pmtud_mss_acked = acked;

		tp->snd_una = th->th_ack;
#ifdef TCP_ECN
		/* sync snd_last with snd_una */
		if (SEQ_GT(tp->snd_una, tp->snd_last))
			tp->snd_last = tp->snd_una;
#endif
		if (SEQ_LT(tp->snd_nxt, tp->snd_una))
			tp->snd_nxt = tp->snd_una;

		switch (tp->t_state) {

		/*
		 * In FIN_WAIT_1 STATE in addition to the processing
		 * for the ESTABLISHED state if our FIN is now acknowledged
		 * then enter FIN_WAIT_2.
		 */
		case TCPS_FIN_WAIT_1:
			if (ourfinisacked) {
				/*
				 * If we can't receive any more
				 * data, then closing user can proceed.
				 * Starting the timer is contrary to the
				 * specification, but if we don't get a FIN
				 * we'll hang forever.
				 */
				if (so->so_state & SS_CANTRCVMORE) {
					tp->t_flags |= TF_BLOCKOUTPUT;
					soisdisconnected(so);
					tp->t_flags &= ~TF_BLOCKOUTPUT;
					TCP_TIMER_ARM(tp, TCPT_2MSL, tcp_maxidle);
				}
				tp->t_state = TCPS_FIN_WAIT_2;
			}
			break;

		/*
		 * In CLOSING STATE in addition to the processing for
		 * the ESTABLISHED state if the ACK acknowledges our FIN
		 * then enter the TIME-WAIT state, otherwise ignore
		 * the segment.
		 */
		case TCPS_CLOSING:
			if (ourfinisacked) {
				tp->t_state = TCPS_TIME_WAIT;
				tcp_canceltimers(tp);
				TCP_TIMER_ARM(tp, TCPT_2MSL, 2 * TCPTV_MSL);
				tp->t_flags |= TF_BLOCKOUTPUT;
				soisdisconnected(so);
				tp->t_flags &= ~TF_BLOCKOUTPUT;
			}
			break;

		/*
		 * In LAST_ACK, we may still be waiting for data to drain
		 * and/or to be acked, as well as for the ack of our FIN.
		 * If our FIN is now acknowledged, delete the TCB,
		 * enter the closed state and return.
		 */
		case TCPS_LAST_ACK:
			if (ourfinisacked) {
				tp = tcp_close(tp);
				goto drop;
			}
			break;

		/*
		 * In TIME_WAIT state the only thing that should arrive
		 * is a retransmission of the remote FIN.  Acknowledge
		 * it and restart the finack timer.
		 */
		case TCPS_TIME_WAIT:
			TCP_TIMER_ARM(tp, TCPT_2MSL, 2 * TCPTV_MSL);
			goto dropafterack;
		}
	}

step6:
	/*
	 * Update window information.
	 * Don't look at window if no ACK: TAC's send garbage on first SYN.
	 */
	if ((tiflags & TH_ACK) &&
	    (SEQ_LT(tp->snd_wl1, th->th_seq) || (tp->snd_wl1 == th->th_seq &&
	    (SEQ_LT(tp->snd_wl2, th->th_ack) ||
	    (tp->snd_wl2 == th->th_ack && tiwin > tp->snd_wnd))))) {
		/* keep track of pure window updates */
		if (tlen == 0 &&
		    tp->snd_wl2 == th->th_ack && tiwin > tp->snd_wnd)
			tcpstat_inc(tcps_rcvwinupd);
		tp->snd_wnd = tiwin;
		tp->snd_wl1 = th->th_seq;
		tp->snd_wl2 = th->th_ack;
		if (tp->snd_wnd > tp->max_sndwnd)
			tp->max_sndwnd = tp->snd_wnd;
		tp->t_flags |= TF_NEEDOUTPUT;
	}

	/*
	 * Process segments with URG.
	 */
	if ((tiflags & TH_URG) && th->th_urp &&
	    TCPS_HAVERCVDFIN(tp->t_state) == 0) {
		/*
		 * This is a kludge, but if we receive and accept
		 * random urgent pointers, we'll crash in
		 * soreceive.  It's hard to imagine someone
		 * actually wanting to send this much urgent data.
		 */
		if (th->th_urp + so->so_rcv.sb_cc > sb_max) {
			th->th_urp = 0;			/* XXX */
			tiflags &= ~TH_URG;		/* XXX */
			goto dodata;			/* XXX */
		}
		/*
		 * If this segment advances the known urgent pointer,
		 * then mark the data stream.  This should not happen
		 * in CLOSE_WAIT, CLOSING, LAST_ACK or TIME_WAIT STATES since
		 * a FIN has been received from the remote side.
		 * In these states we ignore the URG.
		 *
		 * According to RFC961 (Assigned Protocols),
		 * the urgent pointer points to the last octet
		 * of urgent data.  We continue, however,
		 * to consider it to indicate the first octet
		 * of data past the urgent section as the original
		 * spec states (in one of two places).
		 */
		if (SEQ_GT(th->th_seq+th->th_urp, tp->rcv_up)) {
			tp->rcv_up = th->th_seq + th->th_urp;
			so->so_oobmark = so->so_rcv.sb_cc +
			    (tp->rcv_up - tp->rcv_nxt) - 1;
			if (so->so_oobmark == 0)
				so->so_state |= SS_RCVATMARK;
			sohasoutofband(so);
			tp->t_oobflags &= ~(TCPOOB_HAVEDATA | TCPOOB_HADDATA);
		}
		/*
		 * Remove out of band data so doesn't get presented to user.
		 * This can happen independent of advancing the URG pointer,
		 * but if two URG's are pending at once, some out-of-band
		 * data may creep in... ick.
		 */
		if (th->th_urp <= (u_int16_t) tlen &&
		    (so->so_options & SO_OOBINLINE) == 0)
		        tcp_pulloutofband(so, th->th_urp, m, hdroptlen);
	} else
		/*
		 * If no out of band data is expected,
		 * pull receive urgent pointer along
		 * with the receive window.
		 */
		if (SEQ_GT(tp->rcv_nxt, tp->rcv_up))
			tp->rcv_up = tp->rcv_nxt;
dodata:							/* XXX */

	/*
	 * Process the segment text, merging it into the TCP sequencing queue,
	 * and arranging for acknowledgment of receipt if necessary.
	 * This process logically involves adjusting tp->rcv_wnd as data
	 * is presented to the user (this happens in tcp_usrreq.c,
	 * case PRU_RCVD).  If a FIN has already been received on this
	 * connection then we just ignore the text.
	 */
	if ((tlen || (tiflags & TH_FIN)) &&
	    TCPS_HAVERCVDFIN(tp->t_state) == 0) {
		tcp_seq laststart = th->th_seq;
		tcp_seq lastend = th->th_seq + tlen;

		if (th->th_seq == tp->rcv_nxt && TAILQ_EMPTY(&tp->t_segq) &&
		    tp->t_state == TCPS_ESTABLISHED) {
			TCP_SETUP_ACK(tp, tiflags, m);
			tp->rcv_nxt += tlen;
			tiflags = th->th_flags & TH_FIN;
			tcpstat_pkt(tcps_rcvpack, tcps_rcvbyte, tlen);
			ND6_HINT(tp);
			if (so->so_state & SS_CANTRCVMORE)
				m_freem(m);
			else {
				m_adj(m, hdroptlen);
				sbappendstream(so, &so->so_rcv, m);
			}
			tp->t_flags |= TF_BLOCKOUTPUT;
			sorwakeup(so);
			tp->t_flags &= ~TF_BLOCKOUTPUT;
		} else {
			m_adj(m, hdroptlen);
			tiflags = tcp_reass(tp, th, m, &tlen);
			tp->t_flags |= TF_ACKNOW;
		}
		if (tp->sack_enable)
			tcp_update_sack_list(tp, laststart, lastend);

		/*
		 * variable len never referenced again in modern BSD,
		 * so why bother computing it ??
		 */
#if 0
		/*
		 * Note the amount of data that peer has sent into
		 * our window, in order to estimate the sender's
		 * buffer size.
		 */
		len = so->so_rcv.sb_hiwat - (tp->rcv_adv - tp->rcv_nxt);
#endif /* 0 */
	} else {
		m_freem(m);
		tiflags &= ~TH_FIN;
	}

	/*
	 * If FIN is received ACK the FIN and let the user know
	 * that the connection is closing.  Ignore a FIN received before
	 * the connection is fully established.
	 */
	if ((tiflags & TH_FIN) && TCPS_HAVEESTABLISHED(tp->t_state)) {
		if (TCPS_HAVERCVDFIN(tp->t_state) == 0) {
			tp->t_flags |= TF_BLOCKOUTPUT;
			socantrcvmore(so);
			tp->t_flags &= ~TF_BLOCKOUTPUT;
			tp->t_flags |= TF_ACKNOW;
			tp->rcv_nxt++;
		}
		switch (tp->t_state) {

		/*
		 * In ESTABLISHED STATE enter the CLOSE_WAIT state.
		 */
		case TCPS_ESTABLISHED:
			tp->t_state = TCPS_CLOSE_WAIT;
			break;

		/*
		 * If still in FIN_WAIT_1 STATE FIN has not been acked so
		 * enter the CLOSING state.
		 */
		case TCPS_FIN_WAIT_1:
			tp->t_state = TCPS_CLOSING;
			break;

		/*
		 * In FIN_WAIT_2 state enter the TIME_WAIT state,
		 * starting the time-wait timer, turning off the other
		 * standard timers.
		 */
		case TCPS_FIN_WAIT_2:
			tp->t_state = TCPS_TIME_WAIT;
			tcp_canceltimers(tp);
			TCP_TIMER_ARM(tp, TCPT_2MSL, 2 * TCPTV_MSL);
			tp->t_flags |= TF_BLOCKOUTPUT;
			soisdisconnected(so);
			tp->t_flags &= ~TF_BLOCKOUTPUT;
			break;

		/*
		 * In TIME_WAIT state restart the 2 MSL time_wait timer.
		 */
		case TCPS_TIME_WAIT:
			TCP_TIMER_ARM(tp, TCPT_2MSL, 2 * TCPTV_MSL);
			break;
		}
	}
	if (otp)
		tcp_trace(TA_INPUT, ostate, tp, otp, saveti, 0, tlen);

	/*
	 * Return any desired output.
	 */
	if (tp->t_flags & (TF_ACKNOW|TF_NEEDOUTPUT))
		(void) tcp_output(tp);
	return IPPROTO_DONE;

badsyn:
	/*
	 * Received a bad SYN.  Increment counters and dropwithreset.
	 */
	tcpstat_inc(tcps_badsyn);
	tp = NULL;
	goto dropwithreset;

dropafterack_ratelim:
	if (ppsratecheck(&tcp_ackdrop_ppslim_last, &tcp_ackdrop_ppslim_count,
	    tcp_ackdrop_ppslim) == 0) {
		/* XXX stat */
		goto drop;
	}
	/* ...fall into dropafterack... */

dropafterack:
	/*
	 * Generate an ACK dropping incoming segment if it occupies
	 * sequence space, where the ACK reflects our state.
	 */
	if (tiflags & TH_RST)
		goto drop;
	m_freem(m);
	tp->t_flags |= TF_ACKNOW;
	(void) tcp_output(tp);
	return IPPROTO_DONE;

dropwithreset_ratelim:
	/*
	 * We may want to rate-limit RSTs in certain situations,
	 * particularly if we are sending an RST in response to
	 * an attempt to connect to or otherwise communicate with
	 * a port for which we have no socket.
	 */
	if (ppsratecheck(&tcp_rst_ppslim_last, &tcp_rst_ppslim_count,
	    tcp_rst_ppslim) == 0) {
		/* XXX stat */
		goto drop;
	}
	/* ...fall into dropwithreset... */

dropwithreset:
	/*
	 * Generate a RST, dropping incoming segment.
	 * Make ACK acceptable to originator of segment.
	 * Don't bother to respond to RST.
	 */
	if (tiflags & TH_RST)
		goto drop;
	if (tiflags & TH_ACK) {
		tcp_respond(tp, mtod(m, caddr_t), th, (tcp_seq)0, th->th_ack,
		    TH_RST, m->m_pkthdr.ph_rtableid);
	} else {
		if (tiflags & TH_SYN)
			tlen++;
		tcp_respond(tp, mtod(m, caddr_t), th, th->th_seq + tlen,
		    (tcp_seq)0, TH_RST|TH_ACK, m->m_pkthdr.ph_rtableid);
	}
	m_freem(m);
	return IPPROTO_DONE;

drop:
	/*
	 * Drop space held by incoming segment and return.
	 */
	if (otp)
		tcp_trace(TA_DROP, ostate, tp, otp, saveti, 0, tlen);

	m_freem(m);
	return IPPROTO_DONE;
}