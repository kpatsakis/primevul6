tcp_del_sackholes(struct tcpcb *tp, struct tcphdr *th)
{
	if (tp->sack_enable && tp->t_state != TCPS_LISTEN) {
		/* max because this could be an older ack just arrived */
		tcp_seq lastack = SEQ_GT(th->th_ack, tp->snd_una) ?
			th->th_ack : tp->snd_una;
		struct sackhole *cur = tp->snd_holes;
		struct sackhole *prev;
		while (cur)
			if (SEQ_LEQ(cur->end, lastack)) {
				prev = cur;
				cur = cur->next;
				pool_put(&sackhl_pool, prev);
				tp->snd_numholes--;
			} else if (SEQ_LT(cur->start, lastack)) {
				cur->start = lastack;
				if (SEQ_LT(cur->rxmit, cur->start))
					cur->rxmit = cur->start;
				break;
			} else
				break;
		tp->snd_holes = cur;
	}
}