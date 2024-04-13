static int StreamTcpPacketStateNone(ThreadVars *tv, Packet *p,
                        StreamTcpThread *stt, TcpSession *ssn, PacketQueue *pq)
{
    if (p->tcph->th_flags & TH_RST) {
        StreamTcpSetEvent(p, STREAM_RST_BUT_NO_SESSION);
        SCLogDebug("RST packet received, no session setup");
        return -1;

    } else if (p->tcph->th_flags & TH_FIN) {
        StreamTcpSetEvent(p, STREAM_FIN_BUT_NO_SESSION);
        SCLogDebug("FIN packet received, no session setup");
        return -1;

    /* SYN/ACK */
    } else if ((p->tcph->th_flags & (TH_SYN|TH_ACK)) == (TH_SYN|TH_ACK)) {
        if (stream_config.midstream == FALSE &&
                stream_config.async_oneside == FALSE)
            return 0;

        if (ssn == NULL) {
            ssn = StreamTcpNewSession(p, stt->ssn_pool_id);
            if (ssn == NULL) {
                StatsIncr(tv, stt->counter_tcp_ssn_memcap);
                return -1;
            }
            StatsIncr(tv, stt->counter_tcp_sessions);
            StatsIncr(tv, stt->counter_tcp_midstream_pickups);
        }

        /* reverse packet and flow */
        SCLogDebug("reversing flow and packet");
        PacketSwap(p);
        FlowSwap(p->flow);

        /* set the state */
        StreamTcpPacketSetState(p, ssn, TCP_SYN_RECV);
        SCLogDebug("ssn %p: =~ midstream picked ssn state is now "
                "TCP_SYN_RECV", ssn);
        ssn->flags |= STREAMTCP_FLAG_MIDSTREAM;
        /* Flag used to change the direct in the later stage in the session */
        ssn->flags |= STREAMTCP_FLAG_MIDSTREAM_SYNACK;
        if (stream_config.async_oneside) {
            SCLogDebug("ssn %p: =~ ASYNC", ssn);
            ssn->flags |= STREAMTCP_FLAG_ASYNC;
        }

        /* sequence number & window */
        ssn->server.isn = TCP_GET_SEQ(p);
        STREAMTCP_SET_RA_BASE_SEQ(&ssn->server, ssn->server.isn);
        ssn->server.next_seq = ssn->server.isn + 1;
        ssn->server.window = TCP_GET_WINDOW(p);
        SCLogDebug("ssn %p: server window %u", ssn, ssn->server.window);

        ssn->client.isn = TCP_GET_ACK(p) - 1;
        STREAMTCP_SET_RA_BASE_SEQ(&ssn->client, ssn->client.isn);
        ssn->client.next_seq = ssn->client.isn + 1;

        ssn->client.last_ack = TCP_GET_ACK(p);
        ssn->server.last_ack = TCP_GET_SEQ(p);

        ssn->server.next_win = ssn->server.last_ack + ssn->server.window;

        /** If the client has a wscale option the server had it too,
         *  so set the wscale for the server to max. Otherwise none
         *  will have the wscale opt just like it should. */
        if (TCP_HAS_WSCALE(p)) {
            ssn->client.wscale = TCP_GET_WSCALE(p);
            ssn->server.wscale = TCP_WSCALE_MAX;
            SCLogDebug("ssn %p: wscale enabled. client %u server %u",
                    ssn, ssn->client.wscale, ssn->server.wscale);
        }

        SCLogDebug("ssn %p: ssn->client.isn %"PRIu32", ssn->client.next_seq"
                " %"PRIu32", ssn->client.last_ack %"PRIu32"", ssn,
                ssn->client.isn, ssn->client.next_seq,
                ssn->client.last_ack);
        SCLogDebug("ssn %p: ssn->server.isn %"PRIu32", ssn->server.next_seq"
                " %"PRIu32", ssn->server.last_ack %"PRIu32"", ssn,
                ssn->server.isn, ssn->server.next_seq,
                ssn->server.last_ack);

        /* Set the timestamp value for both streams, if packet has timestamp
         * option enabled.*/
        if (TCP_HAS_TS(p)) {
            ssn->server.last_ts = TCP_GET_TSVAL(p);
            ssn->client.last_ts = TCP_GET_TSECR(p);
            SCLogDebug("ssn %p: ssn->server.last_ts %" PRIu32" "
                    "ssn->client.last_ts %" PRIu32"", ssn,
                    ssn->server.last_ts, ssn->client.last_ts);

            ssn->flags |= STREAMTCP_FLAG_TIMESTAMP;

            ssn->server.last_pkt_ts = p->ts.tv_sec;
            if (ssn->server.last_ts == 0)
                ssn->server.flags |= STREAMTCP_STREAM_FLAG_ZERO_TIMESTAMP;
            if (ssn->client.last_ts == 0)
                ssn->client.flags |= STREAMTCP_STREAM_FLAG_ZERO_TIMESTAMP;

        } else {
            ssn->server.last_ts = 0;
            ssn->client.last_ts = 0;
        }

        if (TCP_GET_SACKOK(p) == 1) {
            ssn->flags |= STREAMTCP_FLAG_SACKOK;
            SCLogDebug("ssn %p: SYN/ACK with SACK permitted, assuming "
                    "SACK permitted for both sides", ssn);
        }
        return 0;

    } else if (p->tcph->th_flags & TH_SYN) {
        if (ssn == NULL) {
            ssn = StreamTcpNewSession(p, stt->ssn_pool_id);
            if (ssn == NULL) {
                StatsIncr(tv, stt->counter_tcp_ssn_memcap);
                return -1;
            }

            StatsIncr(tv, stt->counter_tcp_sessions);
        }

        /* set the state */
        StreamTcpPacketSetState(p, ssn, TCP_SYN_SENT);
        SCLogDebug("ssn %p: =~ ssn state is now TCP_SYN_SENT", ssn);

        if (stream_config.async_oneside) {
            SCLogDebug("ssn %p: =~ ASYNC", ssn);
            ssn->flags |= STREAMTCP_FLAG_ASYNC;
        }

        /* set the sequence numbers and window */
        ssn->client.isn = TCP_GET_SEQ(p);
        STREAMTCP_SET_RA_BASE_SEQ(&ssn->client, ssn->client.isn);
        ssn->client.next_seq = ssn->client.isn + 1;

        /* Set the stream timestamp value, if packet has timestamp option
         * enabled. */
        if (TCP_HAS_TS(p)) {
            ssn->client.last_ts = TCP_GET_TSVAL(p);
            SCLogDebug("ssn %p: %02x", ssn, ssn->client.last_ts);

            if (ssn->client.last_ts == 0)
                ssn->client.flags |= STREAMTCP_STREAM_FLAG_ZERO_TIMESTAMP;

            ssn->client.last_pkt_ts = p->ts.tv_sec;
            ssn->client.flags |= STREAMTCP_STREAM_FLAG_TIMESTAMP;
        }

        ssn->server.window = TCP_GET_WINDOW(p);
        if (TCP_HAS_WSCALE(p)) {
            ssn->flags |= STREAMTCP_FLAG_SERVER_WSCALE;
            ssn->server.wscale = TCP_GET_WSCALE(p);
        }

        if (TCP_GET_SACKOK(p) == 1) {
            ssn->flags |= STREAMTCP_FLAG_CLIENT_SACKOK;
            SCLogDebug("ssn %p: SACK permitted on SYN packet", ssn);
        }

        if (TCP_HAS_TFO(p)) {
            ssn->flags |= STREAMTCP_FLAG_TCP_FAST_OPEN;
            if (p->payload_len) {
                StreamTcpUpdateNextSeq(ssn, &ssn->client, (ssn->client.next_seq + p->payload_len));
                SCLogDebug("ssn: %p (TFO) [len: %d] isn %u base_seq %u next_seq %u payload len %u",
                        ssn, p->tcpvars.tfo.len, ssn->client.isn, ssn->client.base_seq, ssn->client.next_seq, p->payload_len);
                StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn, &ssn->client, p, pq);
            }
        }

        SCLogDebug("ssn %p: ssn->client.isn %" PRIu32 ", "
                "ssn->client.next_seq %" PRIu32 ", ssn->client.last_ack "
                "%"PRIu32"", ssn, ssn->client.isn, ssn->client.next_seq,
                ssn->client.last_ack);

    } else if (p->tcph->th_flags & TH_ACK) {
        if (stream_config.midstream == FALSE)
            return 0;

        if (ssn == NULL) {
            ssn = StreamTcpNewSession(p, stt->ssn_pool_id);
            if (ssn == NULL) {
                StatsIncr(tv, stt->counter_tcp_ssn_memcap);
                return -1;
            }
            StatsIncr(tv, stt->counter_tcp_sessions);
            StatsIncr(tv, stt->counter_tcp_midstream_pickups);
        }
        /* set the state */
        StreamTcpPacketSetState(p, ssn, TCP_ESTABLISHED);
        SCLogDebug("ssn %p: =~ midstream picked ssn state is now "
                "TCP_ESTABLISHED", ssn);

        ssn->flags = STREAMTCP_FLAG_MIDSTREAM;
        ssn->flags |= STREAMTCP_FLAG_MIDSTREAM_ESTABLISHED;
        if (stream_config.async_oneside) {
            SCLogDebug("ssn %p: =~ ASYNC", ssn);
            ssn->flags |= STREAMTCP_FLAG_ASYNC;
        }

        /** window scaling for midstream pickups, we can't do much other
         *  than assume that it's set to the max value: 14 */
        ssn->client.wscale = TCP_WSCALE_MAX;
        ssn->server.wscale = TCP_WSCALE_MAX;

        /* set the sequence numbers and window */
        ssn->client.isn = TCP_GET_SEQ(p) - 1;
        STREAMTCP_SET_RA_BASE_SEQ(&ssn->client, ssn->client.isn);
        ssn->client.next_seq = TCP_GET_SEQ(p) + p->payload_len;
        ssn->client.window = TCP_GET_WINDOW(p) << ssn->client.wscale;
        ssn->client.last_ack = TCP_GET_SEQ(p);
        ssn->client.next_win = ssn->client.last_ack + ssn->client.window;
        SCLogDebug("ssn %p: ssn->client.isn %u, ssn->client.next_seq %u",
                ssn, ssn->client.isn, ssn->client.next_seq);

        ssn->server.isn = TCP_GET_ACK(p) - 1;
        STREAMTCP_SET_RA_BASE_SEQ(&ssn->server, ssn->server.isn);
        ssn->server.next_seq = ssn->server.isn + 1;
        ssn->server.last_ack = TCP_GET_ACK(p);
        ssn->server.next_win = ssn->server.last_ack;

        SCLogDebug("ssn %p: ssn->client.next_win %"PRIu32", "
                "ssn->server.next_win %"PRIu32"", ssn,
                ssn->client.next_win, ssn->server.next_win);
        SCLogDebug("ssn %p: ssn->client.last_ack %"PRIu32", "
                "ssn->server.last_ack %"PRIu32"", ssn,
                ssn->client.last_ack, ssn->server.last_ack);

        /* Set the timestamp value for both streams, if packet has timestamp
         * option enabled.*/
        if (TCP_HAS_TS(p)) {
            ssn->client.last_ts = TCP_GET_TSVAL(p);
            ssn->server.last_ts = TCP_GET_TSECR(p);
            SCLogDebug("ssn %p: ssn->server.last_ts %" PRIu32" "
                    "ssn->client.last_ts %" PRIu32"", ssn,
                    ssn->server.last_ts, ssn->client.last_ts);

            ssn->flags |= STREAMTCP_FLAG_TIMESTAMP;

            ssn->client.last_pkt_ts = p->ts.tv_sec;
            if (ssn->server.last_ts == 0)
                ssn->server.flags |= STREAMTCP_STREAM_FLAG_ZERO_TIMESTAMP;
            if (ssn->client.last_ts == 0)
                ssn->client.flags |= STREAMTCP_STREAM_FLAG_ZERO_TIMESTAMP;

        } else {
            ssn->server.last_ts = 0;
            ssn->client.last_ts = 0;
        }

        StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn, &ssn->client, p, pq);

        ssn->flags |= STREAMTCP_FLAG_SACKOK;
        SCLogDebug("ssn %p: assuming SACK permitted for both sides", ssn);

    } else {
        SCLogDebug("default case");
    }

    return 0;
}