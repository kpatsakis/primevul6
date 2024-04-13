tcp_newreno_partialack(struct tcpcb *tp, struct tcphdr *th)
{
	/*
	 * snd_una has not been updated and the socket send buffer
	 * not yet drained of the acked data, so we have to leave
	 * snd_una as it was to get the correct data offset in
	 * tcp_output().
	 */
	tcp_seq onxt = tp->snd_nxt;
	u_long  ocwnd = tp->snd_cwnd;

	TCP_TIMER_DISARM(tp, TCPT_REXMT);
	tp->t_rtttime = 0;
	tp->snd_nxt = th->th_ack;
	/*
	 * Set snd_cwnd to one segment beyond acknowledged offset
	 * (tp->snd_una not yet updated when this function is called)
	 */
	tp->snd_cwnd = tp->t_maxseg + (th->th_ack - tp->snd_una);
	(void)tcp_output(tp);
	tp->snd_cwnd = ocwnd;
	if (SEQ_GT(onxt, tp->snd_nxt))
		tp->snd_nxt = onxt;
	/*
	 * Partial window deflation.  Relies on fact that tp->snd_una
	 * not updated yet.
	 */
	if (tp->snd_cwnd > th->th_ack - tp->snd_una)
		tp->snd_cwnd -= th->th_ack - tp->snd_una;
	else
		tp->snd_cwnd = 0;
	tp->snd_cwnd += tp->t_maxseg;
}