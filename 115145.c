static void StreamTcpPseudoPacketCreateDetectLogFlush(ThreadVars *tv,
        StreamTcpThread *stt, Packet *parent,
        TcpSession *ssn, PacketQueue *pq, int dir)
{
    SCEnter();
    Flow *f = parent->flow;

    if (parent->flags & PKT_PSEUDO_DETECTLOG_FLUSH) {
        SCReturn;
    }

    Packet *np = PacketPoolGetPacket();
    if (np == NULL) {
        SCReturn;
    }
    PKT_SET_SRC(np, PKT_SRC_STREAM_TCP_DETECTLOG_FLUSH);

    np->tenant_id = f->tenant_id;
    np->datalink = DLT_RAW;
    np->proto = IPPROTO_TCP;
    FlowReference(&np->flow, f);
    np->flags |= PKT_STREAM_EST;
    np->flags |= PKT_HAS_FLOW;
    np->flags |= PKT_IGNORE_CHECKSUM;
    np->flags |= PKT_PSEUDO_DETECTLOG_FLUSH;
    np->vlan_id[0] = f->vlan_id[0];
    np->vlan_id[1] = f->vlan_id[1];
    np->vlan_idx = f->vlan_idx;
    np->livedev = (struct LiveDevice_ *)f->livedev;

    if (f->flags & FLOW_NOPACKET_INSPECTION) {
        DecodeSetNoPacketInspectionFlag(np);
    }
    if (f->flags & FLOW_NOPAYLOAD_INSPECTION) {
        DecodeSetNoPayloadInspectionFlag(np);
    }

    if (dir == 0) {
        SCLogDebug("pseudo is to_server");
        np->flowflags |= FLOW_PKT_TOSERVER;
    } else {
        SCLogDebug("pseudo is to_client");
        np->flowflags |= FLOW_PKT_TOCLIENT;
    }
    np->flowflags |= FLOW_PKT_ESTABLISHED;
    np->payload = NULL;
    np->payload_len = 0;

    if (FLOW_IS_IPV4(f)) {
        if (dir == 0) {
            FLOW_COPY_IPV4_ADDR_TO_PACKET(&f->src, &np->src);
            FLOW_COPY_IPV4_ADDR_TO_PACKET(&f->dst, &np->dst);
            np->sp = f->sp;
            np->dp = f->dp;
        } else {
            FLOW_COPY_IPV4_ADDR_TO_PACKET(&f->src, &np->dst);
            FLOW_COPY_IPV4_ADDR_TO_PACKET(&f->dst, &np->src);
            np->sp = f->dp;
            np->dp = f->sp;
        }

        /* Check if we have enough room in direct data. We need ipv4 hdr + tcp hdr.
         * Force an allocation if it is not the case.
         */
        if (GET_PKT_DIRECT_MAX_SIZE(np) <  40) {
            if (PacketCallocExtPkt(np, 40) == -1) {
                goto error;
            }
        }
        /* set the ip header */
        np->ip4h = (IPV4Hdr *)GET_PKT_DATA(np);
        /* version 4 and length 20 bytes for the tcp header */
        np->ip4h->ip_verhl = 0x45;
        np->ip4h->ip_tos = 0;
        np->ip4h->ip_len = htons(40);
        np->ip4h->ip_id = 0;
        np->ip4h->ip_off = 0;
        np->ip4h->ip_ttl = 64;
        np->ip4h->ip_proto = IPPROTO_TCP;
        if (dir == 0) {
            np->ip4h->s_ip_src.s_addr = f->src.addr_data32[0];
            np->ip4h->s_ip_dst.s_addr = f->dst.addr_data32[0];
        } else {
            np->ip4h->s_ip_src.s_addr = f->dst.addr_data32[0];
            np->ip4h->s_ip_dst.s_addr = f->src.addr_data32[0];
        }

        /* set the tcp header */
        np->tcph = (TCPHdr *)((uint8_t *)GET_PKT_DATA(np) + 20);

        SET_PKT_LEN(np, 40); /* ipv4 hdr + tcp hdr */

    } else if (FLOW_IS_IPV6(f)) {
        if (dir == 0) {
            FLOW_COPY_IPV6_ADDR_TO_PACKET(&f->src, &np->src);
            FLOW_COPY_IPV6_ADDR_TO_PACKET(&f->dst, &np->dst);
            np->sp = f->sp;
            np->dp = f->dp;
        } else {
            FLOW_COPY_IPV6_ADDR_TO_PACKET(&f->src, &np->dst);
            FLOW_COPY_IPV6_ADDR_TO_PACKET(&f->dst, &np->src);
            np->sp = f->dp;
            np->dp = f->sp;
        }

        /* Check if we have enough room in direct data. We need ipv6 hdr + tcp hdr.
         * Force an allocation if it is not the case.
         */
        if (GET_PKT_DIRECT_MAX_SIZE(np) <  60) {
            if (PacketCallocExtPkt(np, 60) == -1) {
                goto error;
            }
        }
        /* set the ip header */
        np->ip6h = (IPV6Hdr *)GET_PKT_DATA(np);
        /* version 6 */
        np->ip6h->s_ip6_vfc = 0x60;
        np->ip6h->s_ip6_flow = 0;
        np->ip6h->s_ip6_nxt = IPPROTO_TCP;
        np->ip6h->s_ip6_plen = htons(20);
        np->ip6h->s_ip6_hlim = 64;
        if (dir == 0) {
            np->ip6h->s_ip6_src[0] = f->src.addr_data32[0];
            np->ip6h->s_ip6_src[1] = f->src.addr_data32[1];
            np->ip6h->s_ip6_src[2] = f->src.addr_data32[2];
            np->ip6h->s_ip6_src[3] = f->src.addr_data32[3];
            np->ip6h->s_ip6_dst[0] = f->dst.addr_data32[0];
            np->ip6h->s_ip6_dst[1] = f->dst.addr_data32[1];
            np->ip6h->s_ip6_dst[2] = f->dst.addr_data32[2];
            np->ip6h->s_ip6_dst[3] = f->dst.addr_data32[3];
        } else {
            np->ip6h->s_ip6_src[0] = f->dst.addr_data32[0];
            np->ip6h->s_ip6_src[1] = f->dst.addr_data32[1];
            np->ip6h->s_ip6_src[2] = f->dst.addr_data32[2];
            np->ip6h->s_ip6_src[3] = f->dst.addr_data32[3];
            np->ip6h->s_ip6_dst[0] = f->src.addr_data32[0];
            np->ip6h->s_ip6_dst[1] = f->src.addr_data32[1];
            np->ip6h->s_ip6_dst[2] = f->src.addr_data32[2];
            np->ip6h->s_ip6_dst[3] = f->src.addr_data32[3];
        }

        /* set the tcp header */
        np->tcph = (TCPHdr *)((uint8_t *)GET_PKT_DATA(np) + 40);

        SET_PKT_LEN(np, 60); /* ipv6 hdr + tcp hdr */
    }

    np->tcph->th_offx2 = 0x50;
    np->tcph->th_flags |= TH_ACK;
    np->tcph->th_win = 10;
    np->tcph->th_urp = 0;

    /* to server */
    if (dir == 0) {
        np->tcph->th_sport = htons(f->sp);
        np->tcph->th_dport = htons(f->dp);

        np->tcph->th_seq = htonl(ssn->client.next_seq);
        np->tcph->th_ack = htonl(ssn->server.last_ack);

    /* to client */
    } else {
        np->tcph->th_sport = htons(f->dp);
        np->tcph->th_dport = htons(f->sp);

        np->tcph->th_seq = htonl(ssn->server.next_seq);
        np->tcph->th_ack = htonl(ssn->client.last_ack);
    }

    /* use parent time stamp */
    memcpy(&np->ts, &parent->ts, sizeof(struct timeval));

    SCLogDebug("np %p", np);
    PacketEnqueue(pq, np);

    StatsIncr(tv, stt->counter_tcp_pseudo);
    SCReturn;
error:
    FlowDeReference(&np->flow);
    SCReturn;
}