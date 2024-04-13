static void isdn_ppp_mp_receive(isdn_net_dev *net_dev, isdn_net_local *lp,
				struct sk_buff *skb)
{
	struct ippp_struct *is;
	isdn_net_local *lpq;
	ippp_bundle *mp;
	isdn_mppp_stats *stats;
	struct sk_buff *newfrag, *frag, *start, *nextf;
	u32 newseq, minseq, thisseq;
	unsigned long flags;
	int slot;

	spin_lock_irqsave(&net_dev->pb->lock, flags);
	mp = net_dev->pb;
	stats = &mp->stats;
	slot = lp->ppp_slot;
	if (slot < 0 || slot >= ISDN_MAX_CHANNELS) {
		printk(KERN_ERR "%s: lp->ppp_slot(%d)\n",
		       __func__, lp->ppp_slot);
		stats->frame_drops++;
		dev_kfree_skb(skb);
		spin_unlock_irqrestore(&mp->lock, flags);
		return;
	}
	is = ippp_table[slot];
	if (++mp->frames > stats->max_queue_len)
		stats->max_queue_len = mp->frames;

	if (is->debug & 0x8)
		isdn_ppp_mp_print_recv_pkt(lp->ppp_slot, skb);

	newseq = isdn_ppp_mp_get_seq(is->mpppcfg & SC_IN_SHORT_SEQ,
				     skb, is->last_link_seqno);


	/* if this packet seq # is less than last already processed one,
	 * toss it right away, but check for sequence start case first
	 */
	if (mp->seq > MP_LONGSEQ_MAX && (newseq & MP_LONGSEQ_MAXBIT)) {
		mp->seq = newseq;	/* the first packet: required for
					 * rfc1990 non-compliant clients --
					 * prevents constant packet toss */
	} else if (MP_LT(newseq, mp->seq)) {
		stats->frame_drops++;
		isdn_ppp_mp_free_skb(mp, skb);
		spin_unlock_irqrestore(&mp->lock, flags);
		return;
	}

	/* find the minimum received sequence number over all links */
	is->last_link_seqno = minseq = newseq;
	for (lpq = net_dev->queue;;) {
		slot = lpq->ppp_slot;
		if (slot < 0 || slot >= ISDN_MAX_CHANNELS) {
			printk(KERN_ERR "%s: lpq->ppp_slot(%d)\n",
			       __func__, lpq->ppp_slot);
		} else {
			u32 lls = ippp_table[slot]->last_link_seqno;
			if (MP_LT(lls, minseq))
				minseq = lls;
		}
		if ((lpq = lpq->next) == net_dev->queue)
			break;
	}
	if (MP_LT(minseq, mp->seq))
		minseq = mp->seq;	/* can't go beyond already processed
					 * packets */
	newfrag = skb;

	/* if this new fragment is before the first one, then enqueue it now. */
	if ((frag = mp->frags) == NULL || MP_LT(newseq, MP_SEQ(frag))) {
		newfrag->next = frag;
		mp->frags = frag = newfrag;
		newfrag = NULL;
	}

	start = MP_FLAGS(frag) & MP_BEGIN_FRAG &&
		MP_SEQ(frag) == mp->seq ? frag : NULL;

	/*
	 * main fragment traversing loop
	 *
	 * try to accomplish several tasks:
	 * - insert new fragment into the proper sequence slot (once that's done
	 *   newfrag will be set to NULL)
	 * - reassemble any complete fragment sequence (non-null 'start'
	 *   indicates there is a contiguous sequence present)
	 * - discard any incomplete sequences that are below minseq -- due
	 *   to the fact that sender always increment sequence number, if there
	 *   is an incomplete sequence below minseq, no new fragments would
	 *   come to complete such sequence and it should be discarded
	 *
	 * loop completes when we accomplished the following tasks:
	 * - new fragment is inserted in the proper sequence ('newfrag' is
	 *   set to NULL)
	 * - we hit a gap in the sequence, so no reassembly/processing is
	 *   possible ('start' would be set to NULL)
	 *
	 * algorithm for this code is derived from code in the book
	 * 'PPP Design And Debugging' by James Carlson (Addison-Wesley)
	 */
	while (start != NULL || newfrag != NULL) {

		thisseq = MP_SEQ(frag);
		nextf = frag->next;

		/* drop any duplicate fragments */
		if (newfrag != NULL && thisseq == newseq) {
			isdn_ppp_mp_free_skb(mp, newfrag);
			newfrag = NULL;
		}

		/* insert new fragment before next element if possible. */
		if (newfrag != NULL && (nextf == NULL ||
					MP_LT(newseq, MP_SEQ(nextf)))) {
			newfrag->next = nextf;
			frag->next = nextf = newfrag;
			newfrag = NULL;
		}

		if (start != NULL) {
			/* check for misplaced start */
			if (start != frag && (MP_FLAGS(frag) & MP_BEGIN_FRAG)) {
				printk(KERN_WARNING"isdn_mppp(seq %d): new "
				       "BEGIN flag with no prior END", thisseq);
				stats->seqerrs++;
				stats->frame_drops++;
				start = isdn_ppp_mp_discard(mp, start, frag);
				nextf = frag->next;
			}
		} else if (MP_LE(thisseq, minseq)) {
			if (MP_FLAGS(frag) & MP_BEGIN_FRAG)
				start = frag;
			else {
				if (MP_FLAGS(frag) & MP_END_FRAG)
					stats->frame_drops++;
				if (mp->frags == frag)
					mp->frags = nextf;
				isdn_ppp_mp_free_skb(mp, frag);
				frag = nextf;
				continue;
			}
		}

		/* if start is non-null and we have end fragment, then
		 * we have full reassembly sequence -- reassemble
		 * and process packet now
		 */
		if (start != NULL && (MP_FLAGS(frag) & MP_END_FRAG)) {
			minseq = mp->seq = (thisseq + 1) & MP_LONGSEQ_MASK;
			/* Reassemble the packet then dispatch it */
			isdn_ppp_mp_reassembly(net_dev, lp, start, nextf);

			start = NULL;
			frag = NULL;

			mp->frags = nextf;
		}

		/* check if need to update start pointer: if we just
		 * reassembled the packet and sequence is contiguous
		 * then next fragment should be the start of new reassembly
		 * if sequence is contiguous, but we haven't reassembled yet,
		 * keep going.
		 * if sequence is not contiguous, either clear everything
		 * below low watermark and set start to the next frag or
		 * clear start ptr.
		 */
		if (nextf != NULL &&
		    ((thisseq + 1) & MP_LONGSEQ_MASK) == MP_SEQ(nextf)) {
			/* if we just reassembled and the next one is here,
			 * then start another reassembly. */

			if (frag == NULL) {
				if (MP_FLAGS(nextf) & MP_BEGIN_FRAG)
					start = nextf;
				else
				{
					printk(KERN_WARNING"isdn_mppp(seq %d):"
					       " END flag with no following "
					       "BEGIN", thisseq);
					stats->seqerrs++;
				}
			}

		} else {
			if (nextf != NULL && frag != NULL &&
			    MP_LT(thisseq, minseq)) {
				/* we've got a break in the sequence
				 * and we not at the end yet
				 * and we did not just reassembled
				 *(if we did, there wouldn't be anything before)
				 * and we below the low watermark
				 * discard all the frames below low watermark
				 * and start over */
				stats->frame_drops++;
				mp->frags = isdn_ppp_mp_discard(mp, start, nextf);
			}
			/* break in the sequence, no reassembly */
			start = NULL;
		}

		frag = nextf;
	}	/* while -- main loop */

	if (mp->frags == NULL)
		mp->frags = frag;

	/* rather straighforward way to deal with (not very) possible
	 * queue overflow */
	if (mp->frames > MP_MAX_QUEUE_LEN) {
		stats->overflows++;
		while (mp->frames > MP_MAX_QUEUE_LEN) {
			frag = mp->frags->next;
			isdn_ppp_mp_free_skb(mp, mp->frags);
			mp->frags = frag;
		}
	}
	spin_unlock_irqrestore(&mp->lock, flags);
}