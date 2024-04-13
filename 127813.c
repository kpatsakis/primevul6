slhc_compress(struct slcompress *comp, unsigned char *icp, int isize,
	unsigned char *ocp, unsigned char **cpp, int compress_cid)
{
	register struct cstate *ocs = &(comp->tstate[comp->xmit_oldest]);
	register struct cstate *lcs = ocs;
	register struct cstate *cs = lcs->next;
	register unsigned long deltaS, deltaA;
	register short changes = 0;
	int hlen;
	unsigned char new_seq[16];
	register unsigned char *cp = new_seq;
	struct iphdr *ip;
	struct tcphdr *th, *oth;
	__sum16 csum;


	/*
	 *	Don't play with runt packets.
	 */

	if(isize<sizeof(struct iphdr))
		return isize;

	ip = (struct iphdr *) icp;

	/* Bail if this packet isn't TCP, or is an IP fragment */
	if (ip->protocol != IPPROTO_TCP || (ntohs(ip->frag_off) & 0x3fff)) {
		/* Send as regular IP */
		if(ip->protocol != IPPROTO_TCP)
			comp->sls_o_nontcp++;
		else
			comp->sls_o_tcp++;
		return isize;
	}
	/* Extract TCP header */

	th = (struct tcphdr *)(((unsigned char *)ip) + ip->ihl*4);
	hlen = ip->ihl*4 + th->doff*4;

	/*  Bail if the TCP packet isn't `compressible' (i.e., ACK isn't set or
	 *  some other control bit is set). Also uncompressible if
	 *  it's a runt.
	 */
	if(hlen > isize || th->syn || th->fin || th->rst ||
	    ! (th->ack)){
		/* TCP connection stuff; send as regular IP */
		comp->sls_o_tcp++;
		return isize;
	}
	/*
	 * Packet is compressible -- we're going to send either a
	 * COMPRESSED_TCP or UNCOMPRESSED_TCP packet.  Either way,
	 * we need to locate (or create) the connection state.
	 *
	 * States are kept in a circularly linked list with
	 * xmit_oldest pointing to the end of the list.  The
	 * list is kept in lru order by moving a state to the
	 * head of the list whenever it is referenced.  Since
	 * the list is short and, empirically, the connection
	 * we want is almost always near the front, we locate
	 * states via linear search.  If we don't find a state
	 * for the datagram, the oldest state is (re-)used.
	 */
	for ( ; ; ) {
		if( ip->saddr == cs->cs_ip.saddr
		 && ip->daddr == cs->cs_ip.daddr
		 && th->source == cs->cs_tcp.source
		 && th->dest == cs->cs_tcp.dest)
			goto found;

		/* if current equal oldest, at end of list */
		if ( cs == ocs )
			break;
		lcs = cs;
		cs = cs->next;
		comp->sls_o_searches++;
	}
	/*
	 * Didn't find it -- re-use oldest cstate.  Send an
	 * uncompressed packet that tells the other side what
	 * connection number we're using for this conversation.
	 *
	 * Note that since the state list is circular, the oldest
	 * state points to the newest and we only need to set
	 * xmit_oldest to update the lru linkage.
	 */
	comp->sls_o_misses++;
	comp->xmit_oldest = lcs->cs_this;
	goto uncompressed;

found:
	/*
	 * Found it -- move to the front on the connection list.
	 */
	if(lcs == ocs) {
 		/* found at most recently used */
	} else if (cs == ocs) {
		/* found at least recently used */
		comp->xmit_oldest = lcs->cs_this;
	} else {
		/* more than 2 elements */
		lcs->next = cs->next;
		cs->next = ocs->next;
		ocs->next = cs;
	}

	/*
	 * Make sure that only what we expect to change changed.
	 * Check the following:
	 * IP protocol version, header length & type of service.
	 * The "Don't fragment" bit.
	 * The time-to-live field.
	 * The TCP header length.
	 * IP options, if any.
	 * TCP options, if any.
	 * If any of these things are different between the previous &
	 * current datagram, we send the current datagram `uncompressed'.
	 */
	oth = &cs->cs_tcp;

	if(ip->version != cs->cs_ip.version || ip->ihl != cs->cs_ip.ihl
	 || ip->tos != cs->cs_ip.tos
	 || (ip->frag_off & htons(0x4000)) != (cs->cs_ip.frag_off & htons(0x4000))
	 || ip->ttl != cs->cs_ip.ttl
	 || th->doff != cs->cs_tcp.doff
	 || (ip->ihl > 5 && memcmp(ip+1,cs->cs_ipopt,((ip->ihl)-5)*4) != 0)
	 || (th->doff > 5 && memcmp(th+1,cs->cs_tcpopt,((th->doff)-5)*4) != 0)){
		goto uncompressed;
	}

	/*
	 * Figure out which of the changing fields changed.  The
	 * receiver expects changes in the order: urgent, window,
	 * ack, seq (the order minimizes the number of temporaries
	 * needed in this section of code).
	 */
	if(th->urg){
		deltaS = ntohs(th->urg_ptr);
		cp = encode(cp,deltaS);
		changes |= NEW_U;
	} else if(th->urg_ptr != oth->urg_ptr){
		/* argh! URG not set but urp changed -- a sensible
		 * implementation should never do this but RFC793
		 * doesn't prohibit the change so we have to deal
		 * with it. */
		goto uncompressed;
	}
	if((deltaS = ntohs(th->window) - ntohs(oth->window)) != 0){
		cp = encode(cp,deltaS);
		changes |= NEW_W;
	}
	if((deltaA = ntohl(th->ack_seq) - ntohl(oth->ack_seq)) != 0L){
		if(deltaA > 0x0000ffff)
			goto uncompressed;
		cp = encode(cp,deltaA);
		changes |= NEW_A;
	}
	if((deltaS = ntohl(th->seq) - ntohl(oth->seq)) != 0L){
		if(deltaS > 0x0000ffff)
			goto uncompressed;
		cp = encode(cp,deltaS);
		changes |= NEW_S;
	}

	switch(changes){
	case 0:	/* Nothing changed. If this packet contains data and the
		 * last one didn't, this is probably a data packet following
		 * an ack (normal on an interactive connection) and we send
		 * it compressed.  Otherwise it's probably a retransmit,
		 * retransmitted ack or window probe.  Send it uncompressed
		 * in case the other side missed the compressed version.
		 */
		if(ip->tot_len != cs->cs_ip.tot_len &&
		   ntohs(cs->cs_ip.tot_len) == hlen)
			break;
		goto uncompressed;
	case SPECIAL_I:
	case SPECIAL_D:
		/* actual changes match one of our special case encodings --
		 * send packet uncompressed.
		 */
		goto uncompressed;
	case NEW_S|NEW_A:
		if(deltaS == deltaA &&
		    deltaS == ntohs(cs->cs_ip.tot_len) - hlen){
			/* special case for echoed terminal traffic */
			changes = SPECIAL_I;
			cp = new_seq;
		}
		break;
	case NEW_S:
		if(deltaS == ntohs(cs->cs_ip.tot_len) - hlen){
			/* special case for data xfer */
			changes = SPECIAL_D;
			cp = new_seq;
		}
		break;
	}
	deltaS = ntohs(ip->id) - ntohs(cs->cs_ip.id);
	if(deltaS != 1){
		cp = encode(cp,deltaS);
		changes |= NEW_I;
	}
	if(th->psh)
		changes |= TCP_PUSH_BIT;
	/* Grab the cksum before we overwrite it below.  Then update our
	 * state with this packet's header.
	 */
	csum = th->check;
	memcpy(&cs->cs_ip,ip,20);
	memcpy(&cs->cs_tcp,th,20);
	/* We want to use the original packet as our compressed packet.
	 * (cp - new_seq) is the number of bytes we need for compressed
	 * sequence numbers.  In addition we need one byte for the change
	 * mask, one for the connection id and two for the tcp checksum.
	 * So, (cp - new_seq) + 4 bytes of header are needed.
	 */
	deltaS = cp - new_seq;
	if(compress_cid == 0 || comp->xmit_current != cs->cs_this){
		cp = ocp;
		*cpp = ocp;
		*cp++ = changes | NEW_C;
		*cp++ = cs->cs_this;
		comp->xmit_current = cs->cs_this;
	} else {
		cp = ocp;
		*cpp = ocp;
		*cp++ = changes;
	}
	*(__sum16 *)cp = csum;
	cp += 2;
/* deltaS is now the size of the change section of the compressed header */
	memcpy(cp,new_seq,deltaS);	/* Write list of deltas */
	memcpy(cp+deltaS,icp+hlen,isize-hlen);
	comp->sls_o_compressed++;
	ocp[0] |= SL_TYPE_COMPRESSED_TCP;
	return isize - hlen + deltaS + (cp - ocp);

	/* Update connection state cs & send uncompressed packet (i.e.,
	 * a regular ip/tcp packet but with the 'conversation id' we hope
	 * to use on future compressed packets in the protocol field).
	 */
uncompressed:
	memcpy(&cs->cs_ip,ip,20);
	memcpy(&cs->cs_tcp,th,20);
	if (ip->ihl > 5)
	  memcpy(cs->cs_ipopt, ip+1, ((ip->ihl) - 5) * 4);
	if (th->doff > 5)
	  memcpy(cs->cs_tcpopt, th+1, ((th->doff) - 5) * 4);
	comp->xmit_current = cs->cs_this;
	comp->sls_o_uncompressed++;
	memcpy(ocp, icp, isize);
	*cpp = ocp;
	ocp[9] = cs->cs_this;
	ocp[0] |= SL_TYPE_UNCOMPRESSED_TCP;
	return isize;
}