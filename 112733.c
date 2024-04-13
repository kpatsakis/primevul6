tcp_update_sack_list(struct tcpcb *tp, tcp_seq rcv_laststart,
    tcp_seq rcv_lastend)
{
	/*
	 * First reported block MUST be the most recent one.  Subsequent
	 * blocks SHOULD be in the order in which they arrived at the
	 * receiver.  These two conditions make the implementation fully
	 * compliant with RFC 2018.
	 */
	int i, j = 0, count = 0, lastpos = -1;
	struct sackblk sack, firstsack, temp[MAX_SACK_BLKS];

	/* First clean up current list of sacks */
	for (i = 0; i < tp->rcv_numsacks; i++) {
		sack = tp->sackblks[i];
		if (sack.start == 0 && sack.end == 0) {
			count++; /* count = number of blocks to be discarded */
			continue;
		}
		if (SEQ_LEQ(sack.end, tp->rcv_nxt)) {
			tp->sackblks[i].start = tp->sackblks[i].end = 0;
			count++;
		} else {
			temp[j].start = tp->sackblks[i].start;
			temp[j++].end = tp->sackblks[i].end;
		}
	}
	tp->rcv_numsacks -= count;
	if (tp->rcv_numsacks == 0) { /* no sack blocks currently (fast path) */
		tcp_clean_sackreport(tp);
		if (SEQ_LT(tp->rcv_nxt, rcv_laststart)) {
			/* ==> need first sack block */
			tp->sackblks[0].start = rcv_laststart;
			tp->sackblks[0].end = rcv_lastend;
			tp->rcv_numsacks = 1;
		}
		return;
	}
	/* Otherwise, sack blocks are already present. */
	for (i = 0; i < tp->rcv_numsacks; i++)
		tp->sackblks[i] = temp[i]; /* first copy back sack list */
	if (SEQ_GEQ(tp->rcv_nxt, rcv_lastend))
		return;     /* sack list remains unchanged */
	/*
	 * From here, segment just received should be (part of) the 1st sack.
	 * Go through list, possibly coalescing sack block entries.
	 */
	firstsack.start = rcv_laststart;
	firstsack.end = rcv_lastend;
	for (i = 0; i < tp->rcv_numsacks; i++) {
		sack = tp->sackblks[i];
		if (SEQ_LT(sack.end, firstsack.start) ||
		    SEQ_GT(sack.start, firstsack.end))
			continue; /* no overlap */
		if (sack.start == firstsack.start && sack.end == firstsack.end){
			/*
			 * identical block; delete it here since we will
			 * move it to the front of the list.
			 */
			tp->sackblks[i].start = tp->sackblks[i].end = 0;
			lastpos = i;    /* last posn with a zero entry */
			continue;
		}
		if (SEQ_LEQ(sack.start, firstsack.start))
			firstsack.start = sack.start; /* merge blocks */
		if (SEQ_GEQ(sack.end, firstsack.end))
			firstsack.end = sack.end;     /* merge blocks */
		tp->sackblks[i].start = tp->sackblks[i].end = 0;
		lastpos = i;    /* last posn with a zero entry */
	}
	if (lastpos != -1) {    /* at least one merge */
		for (i = 0, j = 1; i < tp->rcv_numsacks; i++) {
			sack = tp->sackblks[i];
			if (sack.start == 0 && sack.end == 0)
				continue;
			temp[j++] = sack;
		}
		tp->rcv_numsacks = j; /* including first blk (added later) */
		for (i = 1; i < tp->rcv_numsacks; i++) /* now copy back */
			tp->sackblks[i] = temp[i];
	} else {        /* no merges -- shift sacks by 1 */
		if (tp->rcv_numsacks < MAX_SACK_BLKS)
			tp->rcv_numsacks++;
		for (i = tp->rcv_numsacks-1; i > 0; i--)
			tp->sackblks[i] = tp->sackblks[i-1];
	}
	tp->sackblks[0] = firstsack;
	return;
}