tcp_reass(struct tcpcb *tp, struct tcphdr *th, struct mbuf *m, int *tlen)
{
	struct tcpqent *p, *q, *nq, *tiqe;

	/*
	 * Allocate a new queue entry, before we throw away any data.
	 * If we can't, just drop the packet.  XXX
	 */
	tiqe = pool_get(&tcpqe_pool, PR_NOWAIT);
	if (tiqe == NULL) {
		tiqe = TAILQ_LAST(&tp->t_segq, tcpqehead);
		if (tiqe != NULL && th->th_seq == tp->rcv_nxt) {
			/* Reuse last entry since new segment fills a hole */
			m_freem(tiqe->tcpqe_m);
			TAILQ_REMOVE(&tp->t_segq, tiqe, tcpqe_q);
		}
		if (tiqe == NULL || th->th_seq != tp->rcv_nxt) {
			/* Flush segment queue for this connection */
			tcp_freeq(tp);
			tcpstat_inc(tcps_rcvmemdrop);
			m_freem(m);
			return (0);
		}
	}

	/*
	 * Find a segment which begins after this one does.
	 */
	for (p = NULL, q = TAILQ_FIRST(&tp->t_segq); q != NULL;
	    p = q, q = TAILQ_NEXT(q, tcpqe_q))
		if (SEQ_GT(q->tcpqe_tcp->th_seq, th->th_seq))
			break;

	/*
	 * If there is a preceding segment, it may provide some of
	 * our data already.  If so, drop the data from the incoming
	 * segment.  If it provides all of our data, drop us.
	 */
	if (p != NULL) {
		struct tcphdr *phdr = p->tcpqe_tcp;
		int i;

		/* conversion to int (in i) handles seq wraparound */
		i = phdr->th_seq + phdr->th_reseqlen - th->th_seq;
		if (i > 0) {
		        if (i >= *tlen) {
				tcpstat_pkt(tcps_rcvduppack, tcps_rcvdupbyte,
				    *tlen);
				m_freem(m);
				pool_put(&tcpqe_pool, tiqe);
				return (0);
			}
			m_adj(m, i);
			*tlen -= i;
			th->th_seq += i;
		}
	}
	tcpstat_pkt(tcps_rcvoopack, tcps_rcvoobyte, *tlen);

	/*
	 * While we overlap succeeding segments trim them or,
	 * if they are completely covered, dequeue them.
	 */
	for (; q != NULL; q = nq) {
		struct tcphdr *qhdr = q->tcpqe_tcp;
		int i = (th->th_seq + *tlen) - qhdr->th_seq;

		if (i <= 0)
			break;
		if (i < qhdr->th_reseqlen) {
			qhdr->th_seq += i;
			qhdr->th_reseqlen -= i;
			m_adj(q->tcpqe_m, i);
			break;
		}
		nq = TAILQ_NEXT(q, tcpqe_q);
		m_freem(q->tcpqe_m);
		TAILQ_REMOVE(&tp->t_segq, q, tcpqe_q);
		pool_put(&tcpqe_pool, q);
	}

	/* Insert the new segment queue entry into place. */
	tiqe->tcpqe_m = m;
	th->th_reseqlen = *tlen;
	tiqe->tcpqe_tcp = th;
	if (p == NULL) {
		TAILQ_INSERT_HEAD(&tp->t_segq, tiqe, tcpqe_q);
	} else {
		TAILQ_INSERT_AFTER(&tp->t_segq, p, tiqe, tcpqe_q);
	}

	if (th->th_seq != tp->rcv_nxt)
		return (0);

	return (tcp_flush_queue(tp));
}