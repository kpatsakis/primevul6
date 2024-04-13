void tipc_sk_mcast_rcv(struct net *net, struct sk_buff_head *arrvq,
		       struct sk_buff_head *inputq)
{
	u32 self = tipc_own_addr(net);
	struct sk_buff *skb, *_skb;
	u32 portid, onode;
	struct sk_buff_head tmpq;
	struct list_head dports;
	struct tipc_msg *hdr;
	struct tipc_uaddr ua;
	int user, mtyp, hlen;

	__skb_queue_head_init(&tmpq);
	INIT_LIST_HEAD(&dports);
	ua.addrtype = TIPC_SERVICE_RANGE;

	/* tipc_skb_peek() increments the head skb's reference counter */
	skb = tipc_skb_peek(arrvq, &inputq->lock);
	for (; skb; skb = tipc_skb_peek(arrvq, &inputq->lock)) {
		hdr = buf_msg(skb);
		user = msg_user(hdr);
		mtyp = msg_type(hdr);
		hlen = skb_headroom(skb) + msg_hdr_sz(hdr);
		onode = msg_orignode(hdr);
		ua.sr.type = msg_nametype(hdr);
		ua.sr.lower = msg_namelower(hdr);
		ua.sr.upper = msg_nameupper(hdr);
		if (onode == self)
			ua.scope = TIPC_ANY_SCOPE;
		else
			ua.scope = TIPC_CLUSTER_SCOPE;

		if (mtyp == TIPC_GRP_UCAST_MSG || user == GROUP_PROTOCOL) {
			spin_lock_bh(&inputq->lock);
			if (skb_peek(arrvq) == skb) {
				__skb_dequeue(arrvq);
				__skb_queue_tail(inputq, skb);
			}
			kfree_skb(skb);
			spin_unlock_bh(&inputq->lock);
			continue;
		}

		/* Group messages require exact scope match */
		if (msg_in_group(hdr)) {
			ua.sr.lower = 0;
			ua.sr.upper = ~0;
			ua.scope = msg_lookup_scope(hdr);
		}

		/* Create destination port list: */
		tipc_nametbl_lookup_mcast_sockets(net, &ua, &dports);

		/* Clone message per destination */
		while (tipc_dest_pop(&dports, NULL, &portid)) {
			_skb = __pskb_copy(skb, hlen, GFP_ATOMIC);
			if (_skb) {
				msg_set_destport(buf_msg(_skb), portid);
				__skb_queue_tail(&tmpq, _skb);
				continue;
			}
			pr_warn("Failed to clone mcast rcv buffer\n");
		}
		/* Append clones to inputq only if skb is still head of arrvq */
		spin_lock_bh(&inputq->lock);
		if (skb_peek(arrvq) == skb) {
			skb_queue_splice_tail_init(&tmpq, inputq);
			/* Decrement the skb's refcnt */
			kfree_skb(__skb_dequeue(arrvq));
		}
		spin_unlock_bh(&inputq->lock);
		__skb_queue_purge(&tmpq);
		kfree_skb(skb);
	}
	tipc_sk_rcv(net, inputq);
}