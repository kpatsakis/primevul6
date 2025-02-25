static int ip_frag_reasm(struct ipq *qp, struct sk_buff *skb,
			 struct sk_buff *prev_tail, struct net_device *dev)
{
	struct net *net = container_of(qp->q.net, struct net, ipv4.frags);
	struct iphdr *iph;
	struct sk_buff *fp, *head = skb_rb_first(&qp->q.rb_fragments);
	struct sk_buff **nextp; /* To build frag_list. */
	struct rb_node *rbn;
	int len;
	int ihlen;
	int err;
	u8 ecn;

	ipq_kill(qp);

	ecn = ip_frag_ecn_table[qp->ecn];
	if (unlikely(ecn == 0xff)) {
		err = -EINVAL;
		goto out_fail;
	}
	/* Make the one we just received the head. */
	if (head != skb) {
		fp = skb_clone(skb, GFP_ATOMIC);
		if (!fp)
			goto out_nomem;
		FRAG_CB(fp)->next_frag = FRAG_CB(skb)->next_frag;
		if (RB_EMPTY_NODE(&skb->rbnode))
			FRAG_CB(prev_tail)->next_frag = fp;
		else
			rb_replace_node(&skb->rbnode, &fp->rbnode,
					&qp->q.rb_fragments);
		if (qp->q.fragments_tail == skb)
			qp->q.fragments_tail = fp;
		skb_morph(skb, head);
		FRAG_CB(skb)->next_frag = FRAG_CB(head)->next_frag;
		rb_replace_node(&head->rbnode, &skb->rbnode,
				&qp->q.rb_fragments);
		consume_skb(head);
		head = skb;
	}

	WARN_ON(head->ip_defrag_offset != 0);

	/* Allocate a new buffer for the datagram. */
	ihlen = ip_hdrlen(head);
	len = ihlen + qp->q.len;

	err = -E2BIG;
	if (len > 65535)
		goto out_oversize;

	/* Head of list must not be cloned. */
	if (skb_unclone(head, GFP_ATOMIC))
		goto out_nomem;

	/* If the first fragment is fragmented itself, we split
	 * it to two chunks: the first with data and paged part
	 * and the second, holding only fragments. */
	if (skb_has_frag_list(head)) {
		struct sk_buff *clone;
		int i, plen = 0;

		clone = alloc_skb(0, GFP_ATOMIC);
		if (!clone)
			goto out_nomem;
		skb_shinfo(clone)->frag_list = skb_shinfo(head)->frag_list;
		skb_frag_list_init(head);
		for (i = 0; i < skb_shinfo(head)->nr_frags; i++)
			plen += skb_frag_size(&skb_shinfo(head)->frags[i]);
		clone->len = clone->data_len = head->data_len - plen;
		head->truesize += clone->truesize;
		clone->csum = 0;
		clone->ip_summed = head->ip_summed;
		add_frag_mem_limit(qp->q.net, clone->truesize);
		skb_shinfo(head)->frag_list = clone;
		nextp = &clone->next;
	} else {
		nextp = &skb_shinfo(head)->frag_list;
	}

	skb_push(head, head->data - skb_network_header(head));

	/* Traverse the tree in order, to build frag_list. */
	fp = FRAG_CB(head)->next_frag;
	rbn = rb_next(&head->rbnode);
	rb_erase(&head->rbnode, &qp->q.rb_fragments);
	while (rbn || fp) {
		/* fp points to the next sk_buff in the current run;
		 * rbn points to the next run.
		 */
		/* Go through the current run. */
		while (fp) {
			*nextp = fp;
			nextp = &fp->next;
			fp->prev = NULL;
			memset(&fp->rbnode, 0, sizeof(fp->rbnode));
			head->data_len += fp->len;
			head->len += fp->len;
			if (head->ip_summed != fp->ip_summed)
				head->ip_summed = CHECKSUM_NONE;
			else if (head->ip_summed == CHECKSUM_COMPLETE)
				head->csum = csum_add(head->csum, fp->csum);
			head->truesize += fp->truesize;
			fp = FRAG_CB(fp)->next_frag;
		}
		/* Move to the next run. */
		if (rbn) {
			struct rb_node *rbnext = rb_next(rbn);

			fp = rb_to_skb(rbn);
			rb_erase(rbn, &qp->q.rb_fragments);
			rbn = rbnext;
		}
	}
	sub_frag_mem_limit(qp->q.net, head->truesize);

	*nextp = NULL;
	head->next = NULL;
	head->prev = NULL;
	head->dev = dev;
	head->tstamp = qp->q.stamp;
	IPCB(head)->frag_max_size = max(qp->max_df_size, qp->q.max_size);

	iph = ip_hdr(head);
	iph->tot_len = htons(len);
	iph->tos |= ecn;

	/* When we set IP_DF on a refragmented skb we must also force a
	 * call to ip_fragment to avoid forwarding a DF-skb of size s while
	 * original sender only sent fragments of size f (where f < s).
	 *
	 * We only set DF/IPSKB_FRAG_PMTU if such DF fragment was the largest
	 * frag seen to avoid sending tiny DF-fragments in case skb was built
	 * from one very small df-fragment and one large non-df frag.
	 */
	if (qp->max_df_size == qp->q.max_size) {
		IPCB(head)->flags |= IPSKB_FRAG_PMTU;
		iph->frag_off = htons(IP_DF);
	} else {
		iph->frag_off = 0;
	}

	ip_send_check(iph);

	__IP_INC_STATS(net, IPSTATS_MIB_REASMOKS);
	qp->q.fragments = NULL;
	qp->q.rb_fragments = RB_ROOT;
	qp->q.fragments_tail = NULL;
	qp->q.last_run_head = NULL;
	return 0;

out_nomem:
	net_dbg_ratelimited("queue_glue: no memory for gluing queue %p\n", qp);
	err = -ENOMEM;
	goto out_fail;
out_oversize:
	net_info_ratelimited("Oversized IP packet from %pI4\n", &qp->q.key.v4.saddr);
out_fail:
	__IP_INC_STATS(net, IPSTATS_MIB_REASMFAILS);
	return err;
}