slhc_uncompress(struct slcompress *comp, unsigned char *icp, int isize)
{
	register int changes;
	long x;
	register struct tcphdr *thp;
	register struct iphdr *ip;
	register struct cstate *cs;
	int len, hdrlen;
	unsigned char *cp = icp;

	/* We've got a compressed packet; read the change byte */
	comp->sls_i_compressed++;
	if(isize < 3){
		comp->sls_i_error++;
		return 0;
	}
	changes = *cp++;
	if(changes & NEW_C){
		/* Make sure the state index is in range, then grab the state.
		 * If we have a good state index, clear the 'discard' flag.
		 */
		x = *cp++;	/* Read conn index */
		if(x < 0 || x > comp->rslot_limit)
			goto bad;

		comp->flags &=~ SLF_TOSS;
		comp->recv_current = x;
	} else {
		/* this packet has an implicit state index.  If we've
		 * had a line error since the last time we got an
		 * explicit state index, we have to toss the packet. */
		if(comp->flags & SLF_TOSS){
			comp->sls_i_tossed++;
			return 0;
		}
	}
	cs = &comp->rstate[comp->recv_current];
	thp = &cs->cs_tcp;
	ip = &cs->cs_ip;

	thp->check = *(__sum16 *)cp;
	cp += 2;

	thp->psh = (changes & TCP_PUSH_BIT) ? 1 : 0;
/*
 * we can use the same number for the length of the saved header and
 * the current one, because the packet wouldn't have been sent
 * as compressed unless the options were the same as the previous one
 */

	hdrlen = ip->ihl * 4 + thp->doff * 4;

	switch(changes & SPECIALS_MASK){
	case SPECIAL_I:		/* Echoed terminal traffic */
		{
		register short i;
		i = ntohs(ip->tot_len) - hdrlen;
		thp->ack_seq = htonl( ntohl(thp->ack_seq) + i);
		thp->seq = htonl( ntohl(thp->seq) + i);
		}
		break;

	case SPECIAL_D:			/* Unidirectional data */
		thp->seq = htonl( ntohl(thp->seq) +
				  ntohs(ip->tot_len) - hdrlen);
		break;

	default:
		if(changes & NEW_U){
			thp->urg = 1;
			if((x = decode(&cp)) == -1) {
				goto bad;
			}
			thp->urg_ptr = htons(x);
		} else
			thp->urg = 0;
		if(changes & NEW_W){
			if((x = decode(&cp)) == -1) {
				goto bad;
			}
			thp->window = htons( ntohs(thp->window) + x);
		}
		if(changes & NEW_A){
			if((x = decode(&cp)) == -1) {
				goto bad;
			}
			thp->ack_seq = htonl( ntohl(thp->ack_seq) + x);
		}
		if(changes & NEW_S){
			if((x = decode(&cp)) == -1) {
				goto bad;
			}
			thp->seq = htonl( ntohl(thp->seq) + x);
		}
		break;
	}
	if(changes & NEW_I){
		if((x = decode(&cp)) == -1) {
			goto bad;
		}
		ip->id = htons (ntohs (ip->id) + x);
	} else
		ip->id = htons (ntohs (ip->id) + 1);

	/*
	 * At this point, cp points to the first byte of data in the
	 * packet.  Put the reconstructed TCP and IP headers back on the
	 * packet.  Recalculate IP checksum (but not TCP checksum).
	 */

	len = isize - (cp - icp);
	if (len < 0)
		goto bad;
	len += hdrlen;
	ip->tot_len = htons(len);
	ip->check = 0;

	memmove(icp + hdrlen, cp, len - hdrlen);

	cp = icp;
	memcpy(cp, ip, 20);
	cp += 20;

	if (ip->ihl > 5) {
	  memcpy(cp, cs->cs_ipopt, (ip->ihl - 5) * 4);
	  cp += (ip->ihl - 5) * 4;
	}

	put_unaligned(ip_fast_csum(icp, ip->ihl),
		      &((struct iphdr *)icp)->check);

	memcpy(cp, thp, 20);
	cp += 20;

	if (thp->doff > 5) {
	  memcpy(cp, cs->cs_tcpopt, ((thp->doff) - 5) * 4);
	  cp += ((thp->doff) - 5) * 4;
	}

	return len;
bad:
	comp->sls_i_error++;
	return slhc_toss( comp );
}