slhc_remember(struct slcompress *comp, unsigned char *icp, int isize)
{
	register struct cstate *cs;
	unsigned ihl;

	unsigned char index;

	if(isize < 20) {
		/* The packet is shorter than a legal IP header */
		comp->sls_i_runt++;
		return slhc_toss( comp );
	}
	/* Peek at the IP header's IHL field to find its length */
	ihl = icp[0] & 0xf;
	if(ihl < 20 / 4){
		/* The IP header length field is too small */
		comp->sls_i_runt++;
		return slhc_toss( comp );
	}
	index = icp[9];
	icp[9] = IPPROTO_TCP;

	if (ip_fast_csum(icp, ihl)) {
		/* Bad IP header checksum; discard */
		comp->sls_i_badcheck++;
		return slhc_toss( comp );
	}
	if(index > comp->rslot_limit) {
		comp->sls_i_error++;
		return slhc_toss(comp);
	}

	/* Update local state */
	cs = &comp->rstate[comp->recv_current = index];
	comp->flags &=~ SLF_TOSS;
	memcpy(&cs->cs_ip,icp,20);
	memcpy(&cs->cs_tcp,icp + ihl*4,20);
	if (ihl > 5)
	  memcpy(cs->cs_ipopt, icp + sizeof(struct iphdr), (ihl - 5) * 4);
	if (cs->cs_tcp.doff > 5)
	  memcpy(cs->cs_tcpopt, icp + ihl*4 + sizeof(struct tcphdr), (cs->cs_tcp.doff - 5) * 4);
	cs->cs_hsize = ihl*2 + cs->cs_tcp.doff*2;
	/* Put headers back on packet
	 * Neither header checksum is recalculated
	 */
	comp->sls_i_uncompressed++;
	return isize;
}