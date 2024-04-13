tcp_flush_queue(struct tcpcb *tp)
{
	struct socket *so = tp->t_inpcb->inp_socket;
	struct tcpqent *q, *nq;
	int flags;

	/*
	 * Present data to user, advancing rcv_nxt through
	 * completed sequence space.
	 */
	if (TCPS_HAVEESTABLISHED(tp->t_state) == 0)
		return (0);
	q = TAILQ_FIRST(&tp->t_segq);
	if (q == NULL || q->tcpqe_tcp->th_seq != tp->rcv_nxt)
		return (0);
	if (tp->t_state == TCPS_SYN_RECEIVED && q->tcpqe_tcp->th_reseqlen)
		return (0);
	do {
		tp->rcv_nxt += q->tcpqe_tcp->th_reseqlen;
		flags = q->tcpqe_tcp->th_flags & TH_FIN;

		nq = TAILQ_NEXT(q, tcpqe_q);
		TAILQ_REMOVE(&tp->t_segq, q, tcpqe_q);
		ND6_HINT(tp);
		if (so->so_state & SS_CANTRCVMORE)
			m_freem(q->tcpqe_m);
		else
			sbappendstream(so, &so->so_rcv, q->tcpqe_m);
		pool_put(&tcpqe_pool, q);
		q = nq;
	} while (q != NULL && q->tcpqe_tcp->th_seq == tp->rcv_nxt);
	tp->t_flags |= TF_BLOCKOUTPUT;
	sorwakeup(so);
	tp->t_flags &= ~TF_BLOCKOUTPUT;
	return (flags);
}