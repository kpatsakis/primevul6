static int StreamTcpPacketStateSynRecv(ThreadVars *tv, Packet *p,
                        StreamTcpThread *stt, TcpSession *ssn, PacketQueue *pq)
{
    if (ssn == NULL)
        return -1;

    if (p->tcph->th_flags & TH_RST) {
        if (!StreamTcpValidateRst(ssn, p))
            return -1;

        uint8_t reset = TRUE;
        /* After receiveing the RST in SYN_RECV state and if detection
           evasion flags has been set, then the following operating
           systems will not closed the connection. As they consider the
           packet as stray packet and not belonging to the current
           session, for more information check
           http://www.packetstan.com/2010/06/recently-ive-been-on-campaign-to-make.html */
        if (ssn->flags & STREAMTCP_FLAG_DETECTION_EVASION_ATTEMPT) {
            if (PKT_IS_TOSERVER(p)) {
                if ((ssn->server.os_policy == OS_POLICY_LINUX) ||
                        (ssn->server.os_policy == OS_POLICY_OLD_LINUX) ||
                        (ssn->server.os_policy == OS_POLICY_SOLARIS))
                {
                    reset = FALSE;
                    SCLogDebug("Detection evasion has been attempted, so"
                            " not resetting the connection !!");
                }
            } else {
                if ((ssn->client.os_policy == OS_POLICY_LINUX) ||
                        (ssn->client.os_policy == OS_POLICY_OLD_LINUX) ||
                        (ssn->client.os_policy == OS_POLICY_SOLARIS))
                {
                    reset = FALSE;
                    SCLogDebug("Detection evasion has been attempted, so"
                            " not resetting the connection !!");
                }
            }
        }

        if (reset == TRUE) {
            StreamTcpPacketSetState(p, ssn, TCP_CLOSED);
            SCLogDebug("ssn %p: Reset received and state changed to "
                    "TCP_CLOSED", ssn);

            if (ssn->flags & STREAMTCP_FLAG_TIMESTAMP) {
                StreamTcpHandleTimestamp(ssn, p);
            }
        }

    } else if (p->tcph->th_flags & TH_FIN) {
        /* FIN is handled in the same way as in TCP_ESTABLISHED case */;
        if (ssn->flags & STREAMTCP_FLAG_TIMESTAMP) {
            if (!StreamTcpValidateTimestamp(ssn, p))
                return -1;
        }

        if ((StreamTcpHandleFin(tv, stt, ssn, p, pq)) == -1)
            return -1;

    /* SYN/ACK */
    } else if ((p->tcph->th_flags & (TH_SYN|TH_ACK)) == (TH_SYN|TH_ACK)) {
        SCLogDebug("ssn %p: SYN/ACK packet on state SYN_RECV. resent", ssn);

        if (PKT_IS_TOSERVER(p)) {
            SCLogDebug("ssn %p: SYN/ACK-pkt to server in SYN_RECV state", ssn);

            StreamTcpSetEvent(p, STREAM_3WHS_SYNACK_TOSERVER_ON_SYN_RECV);
            return -1;
        }

        /* Check if the SYN/ACK packets ACK matches the earlier
         * received SYN/ACK packet. */
        if (!(SEQ_EQ(TCP_GET_ACK(p), ssn->client.last_ack))) {
            SCLogDebug("ssn %p: ACK mismatch, packet ACK %" PRIu32 " != "
                    "%" PRIu32 " from stream", ssn, TCP_GET_ACK(p),
                    ssn->client.isn + 1);

            StreamTcpSetEvent(p, STREAM_3WHS_SYNACK_RESEND_WITH_DIFFERENT_ACK);
            return -1;
        }

        /* Check if the SYN/ACK packet SEQ the earlier
         * received SYN/ACK packet, server resend with different ISN. */
        if (!(SEQ_EQ(TCP_GET_SEQ(p), ssn->server.isn))) {
            SCLogDebug("ssn %p: SEQ mismatch, packet SEQ %" PRIu32 " != "
                    "%" PRIu32 " from stream", ssn, TCP_GET_SEQ(p),
                    ssn->client.isn);

            if (StreamTcp3whsQueueSynAck(ssn, p) == -1)
                return -1;
            SCLogDebug("ssn %p: queued different SYN/ACK", ssn);
        }

    } else if (p->tcph->th_flags & TH_SYN) {
        SCLogDebug("ssn %p: SYN packet on state SYN_RECV... resent", ssn);

        if (PKT_IS_TOCLIENT(p)) {
            SCLogDebug("ssn %p: SYN-pkt to client in SYN_RECV state", ssn);

            StreamTcpSetEvent(p, STREAM_3WHS_SYN_TOCLIENT_ON_SYN_RECV);
            return -1;
        }

        if (!(SEQ_EQ(TCP_GET_SEQ(p), ssn->client.isn))) {
            SCLogDebug("ssn %p: SYN with different SEQ on SYN_RECV state", ssn);

            StreamTcpSetEvent(p, STREAM_3WHS_SYN_RESEND_DIFF_SEQ_ON_SYN_RECV);
            return -1;
        }

    } else if (p->tcph->th_flags & TH_ACK) {
        if (ssn->queue_len) {
            SCLogDebug("ssn %p: checking ACK against queued SYN/ACKs", ssn);
            TcpStateQueue *q = StreamTcp3whsFindSynAckByAck(ssn, p);
            if (q != NULL) {
                SCLogDebug("ssn %p: here we update state against queued SYN/ACK", ssn);
                StreamTcp3whsSynAckUpdate(ssn, p, /* using queue to update state */q);
            } else {
                SCLogDebug("ssn %p: none found, now checking ACK against original SYN/ACK (state)", ssn);
            }
        }


        /* If the timestamp option is enabled for both the streams, then
         * validate the received packet timestamp value against the
         * stream->last_ts. If the timestamp is valid then process the
         * packet normally otherwise the drop the packet (RFC 1323)*/
        if (ssn->flags & STREAMTCP_FLAG_TIMESTAMP) {
            if (!(StreamTcpValidateTimestamp(ssn, p))) {
                return -1;
            }
        }

        if ((ssn->flags & STREAMTCP_FLAG_4WHS) && PKT_IS_TOCLIENT(p)) {
            SCLogDebug("ssn %p: ACK received on 4WHS session",ssn);

            if (!(SEQ_EQ(TCP_GET_SEQ(p), ssn->server.next_seq))) {
                SCLogDebug("ssn %p: 4WHS wrong seq nr on packet", ssn);
                StreamTcpSetEvent(p, STREAM_4WHS_WRONG_SEQ);
                return -1;
            }

            if (StreamTcpValidateAck(ssn, &ssn->client, p) == -1) {
                SCLogDebug("ssn %p: 4WHS invalid ack nr on packet", ssn);
                StreamTcpSetEvent(p, STREAM_4WHS_INVALID_ACK);
                return -1;
            }

            SCLogDebug("4WHS normal pkt");
            SCLogDebug("ssn %p: pkt (%" PRIu32 ") is to client: SEQ "
                    "%" PRIu32 ", ACK %" PRIu32 "", ssn, p->payload_len,
                    TCP_GET_SEQ(p), TCP_GET_ACK(p));

            if (ssn->flags & STREAMTCP_FLAG_TIMESTAMP) {
                StreamTcpHandleTimestamp(ssn, p);
            }

            StreamTcpUpdateLastAck(ssn, &ssn->client, TCP_GET_ACK(p));
            StreamTcpUpdateNextSeq(ssn, &ssn->server, (ssn->server.next_seq + p->payload_len));
            ssn->client.window = TCP_GET_WINDOW(p) << ssn->client.wscale;
            ssn->client.next_win = ssn->client.last_ack + ssn->client.window;

            StreamTcpPacketSetState(p, ssn, TCP_ESTABLISHED);
            SCLogDebug("ssn %p: =~ ssn state is now TCP_ESTABLISHED", ssn);

            StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn,
                    &ssn->server, p, pq);

            SCLogDebug("ssn %p: ssn->client.next_win %" PRIu32 ", "
                    "ssn->client.last_ack %"PRIu32"", ssn,
                    ssn->client.next_win, ssn->client.last_ack);
            return 0;
        }

        bool ack_indicates_missed_3whs_ack_packet = false;
        /* Check if the ACK received is in right direction. But when we have
         * picked up a mid stream session after missing the initial SYN pkt,
         * in this case the ACK packet can arrive from either client (normal
         * case) or from server itself (asynchronous streams). Therefore
         *  the check has been avoided in this case */
        if (PKT_IS_TOCLIENT(p)) {
            /* special case, handle 4WHS, so SYN/ACK in the opposite
             * direction */
            if (ssn->flags & STREAMTCP_FLAG_MIDSTREAM_SYNACK) {
                SCLogDebug("ssn %p: ACK received on midstream SYN/ACK "
                        "pickup session",ssn);
                /* fall through */
            } else if (ssn->flags & STREAMTCP_FLAG_TCP_FAST_OPEN) {
                SCLogDebug("ssn %p: ACK received on TFO session",ssn);
                /* fall through */

            } else {
                /* if we missed traffic between the S/SA and the current
                 * 'wrong direction' ACK, we could end up here. In IPS
                 * reject it. But in IDS mode we continue.
                 *
                 * IPS rejects as it should see all packets, so pktloss
                 * should lead to retransmissions. As this can also be
                 * pattern for MOTS/MITM injection attacks, we need to be
                 * careful.
                 */
                if (StreamTcpInlineMode()) {
                    if (p->payload_len > 0 &&
                            SEQ_EQ(TCP_GET_ACK(p), ssn->client.last_ack) &&
                            SEQ_EQ(TCP_GET_SEQ(p), ssn->server.next_seq)) {
                        /* packet loss is possible but unlikely here */
                        SCLogDebug("ssn %p: possible data injection", ssn);
                        StreamTcpSetEvent(p, STREAM_3WHS_ACK_DATA_INJECT);
                        return -1;
                    }

                    SCLogDebug("ssn %p: ACK received in the wrong direction",
                            ssn);
                    StreamTcpSetEvent(p, STREAM_3WHS_ACK_IN_WRONG_DIR);
                    return -1;
                }
                ack_indicates_missed_3whs_ack_packet = true;
            }
        }

        SCLogDebug("ssn %p: pkt (%" PRIu32 ") is to server: SEQ %" PRIu32 ""
                ", ACK %" PRIu32 "", ssn, p->payload_len, TCP_GET_SEQ(p),
                TCP_GET_ACK(p));

        /* Check both seq and ack number before accepting the packet and
           changing to ESTABLISHED state */
        if ((SEQ_EQ(TCP_GET_SEQ(p), ssn->client.next_seq)) &&
                SEQ_EQ(TCP_GET_ACK(p), ssn->server.next_seq)) {
            SCLogDebug("normal pkt");

            /* process the packet normal, No Async streams :) */

            if (ssn->flags & STREAMTCP_FLAG_TIMESTAMP) {
                StreamTcpHandleTimestamp(ssn, p);
            }

            StreamTcpUpdateLastAck(ssn, &ssn->server, TCP_GET_ACK(p));
            StreamTcpUpdateNextSeq(ssn, &ssn->client, (ssn->client.next_seq + p->payload_len));
            ssn->server.window = TCP_GET_WINDOW(p) << ssn->server.wscale;

            ssn->server.next_win = ssn->server.last_ack + ssn->server.window;

            if (ssn->flags & STREAMTCP_FLAG_MIDSTREAM) {
                ssn->client.window = TCP_GET_WINDOW(p) << ssn->client.wscale;
                ssn->client.next_win = ssn->client.last_ack + ssn->client.window;
                ssn->server.next_win = ssn->server.last_ack +
                    ssn->server.window;
                if (!(ssn->flags & STREAMTCP_FLAG_MIDSTREAM_SYNACK)) {
                    /* window scaling for midstream pickups, we can't do much
                     * other than assume that it's set to the max value: 14 */
                    ssn->server.wscale = TCP_WSCALE_MAX;
                    ssn->client.wscale = TCP_WSCALE_MAX;
                    ssn->flags |= STREAMTCP_FLAG_SACKOK;
                }
            }

            StreamTcpPacketSetState(p, ssn, TCP_ESTABLISHED);
            SCLogDebug("ssn %p: =~ ssn state is now TCP_ESTABLISHED", ssn);

            StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn,
                    &ssn->client, p, pq);

            /* If asynchronous stream handling is allowed then set the session,
               if packet's seq number is equal the expected seq no.*/
        } else if (stream_config.async_oneside == TRUE &&
                (SEQ_EQ(TCP_GET_SEQ(p), ssn->server.next_seq)))
        {
            /*set the ASYNC flag used to indicate the session as async stream
              and helps in relaxing the windows checks.*/
            ssn->flags |= STREAMTCP_FLAG_ASYNC;
            ssn->server.next_seq += p->payload_len;
            ssn->server.last_ack = TCP_GET_SEQ(p);

            ssn->client.window = TCP_GET_WINDOW(p) << ssn->client.wscale;
            ssn->client.last_ack = TCP_GET_ACK(p);

            if (ssn->flags & STREAMTCP_FLAG_TIMESTAMP) {
                StreamTcpHandleTimestamp(ssn, p);
            }

            if (ssn->flags & STREAMTCP_FLAG_MIDSTREAM) {
                ssn->server.window = TCP_GET_WINDOW(p);
                ssn->client.next_win = ssn->server.last_ack +
                    ssn->server.window;
                /* window scaling for midstream pickups, we can't do much
                 * other than assume that it's set to the max value: 14 */
                ssn->server.wscale = TCP_WSCALE_MAX;
                ssn->client.wscale = TCP_WSCALE_MAX;
                ssn->flags |= STREAMTCP_FLAG_SACKOK;
            }

            SCLogDebug("ssn %p: synrecv => Asynchronous stream, packet SEQ"
                    " %" PRIu32 ", payload size %" PRIu32 " (%" PRIu32 "), "
                    "ssn->server.next_seq %" PRIu32 "\n"
                    , ssn, TCP_GET_SEQ(p), p->payload_len, TCP_GET_SEQ(p)
                    + p->payload_len, ssn->server.next_seq);

            StreamTcpPacketSetState(p, ssn, TCP_ESTABLISHED);
            SCLogDebug("ssn %p: =~ ssn state is now TCP_ESTABLISHED", ssn);

            StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn,
                    &ssn->server, p, pq);
            /* Upon receiving the packet with correct seq number and wrong
               ACK number, it causes the other end to send RST. But some target
               system (Linux & solaris) does not RST the connection, so it is
               likely to avoid the detection */
        } else if (SEQ_EQ(TCP_GET_SEQ(p), ssn->client.next_seq)){
            ssn->flags |= STREAMTCP_FLAG_DETECTION_EVASION_ATTEMPT;
            SCLogDebug("ssn %p: wrong ack nr on packet, possible evasion!!",
                    ssn);

            StreamTcpSetEvent(p, STREAM_3WHS_RIGHT_SEQ_WRONG_ACK_EVASION);
            return -1;

        /* if we get a packet with a proper ack, but a seq that is beyond
         * next_seq but in-window, we probably missed some packets */
        } else if (SEQ_GT(TCP_GET_SEQ(p), ssn->client.next_seq) &&
                   SEQ_LEQ(TCP_GET_SEQ(p),ssn->client.next_win) &&
                   SEQ_EQ(TCP_GET_ACK(p), ssn->server.next_seq))
        {
            SCLogDebug("ssn %p: ACK for missing data", ssn);

            if (ssn->flags & STREAMTCP_FLAG_TIMESTAMP) {
                StreamTcpHandleTimestamp(ssn, p);
            }

            StreamTcpUpdateLastAck(ssn, &ssn->server, TCP_GET_ACK(p));

            ssn->client.next_seq = TCP_GET_SEQ(p) + p->payload_len;
            SCLogDebug("ssn %p: ACK for missing data: ssn->client.next_seq %u", ssn, ssn->client.next_seq);
            ssn->server.window = TCP_GET_WINDOW(p) << ssn->server.wscale;

            ssn->server.next_win = ssn->server.last_ack + ssn->server.window;

            if (ssn->flags & STREAMTCP_FLAG_MIDSTREAM) {
                ssn->client.window = TCP_GET_WINDOW(p);
                ssn->server.next_win = ssn->server.last_ack +
                    ssn->server.window;
                /* window scaling for midstream pickups, we can't do much
                 * other than assume that it's set to the max value: 14 */
                ssn->server.wscale = TCP_WSCALE_MAX;
                ssn->client.wscale = TCP_WSCALE_MAX;
                ssn->flags |= STREAMTCP_FLAG_SACKOK;
            }

            StreamTcpPacketSetState(p, ssn, TCP_ESTABLISHED);
            SCLogDebug("ssn %p: =~ ssn state is now TCP_ESTABLISHED", ssn);

            StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn,
                    &ssn->client, p, pq);

        /* toclient packet: after having missed the 3whs's final ACK */
        } else if ((ack_indicates_missed_3whs_ack_packet ||
                    (ssn->flags & STREAMTCP_FLAG_TCP_FAST_OPEN)) &&
                SEQ_EQ(TCP_GET_ACK(p), ssn->client.last_ack) &&
                SEQ_EQ(TCP_GET_SEQ(p), ssn->server.next_seq))
        {
            if (ack_indicates_missed_3whs_ack_packet) {
                SCLogDebug("ssn %p: packet fits perfectly after a missed 3whs-ACK", ssn);
            } else {
                SCLogDebug("ssn %p: (TFO) expected packet fits perfectly after SYN/ACK", ssn);
            }

            StreamTcpUpdateNextSeq(ssn, &ssn->server, (TCP_GET_SEQ(p) + p->payload_len));

            ssn->server.window = TCP_GET_WINDOW(p) << ssn->server.wscale;
            ssn->server.next_win = ssn->server.last_ack + ssn->server.window;

            StreamTcpPacketSetState(p, ssn, TCP_ESTABLISHED);
            SCLogDebug("ssn %p: =~ ssn state is now TCP_ESTABLISHED", ssn);

            StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn,
                    &ssn->server, p, pq);

        } else {
            SCLogDebug("ssn %p: wrong seq nr on packet", ssn);

            StreamTcpSetEvent(p, STREAM_3WHS_WRONG_SEQ_WRONG_ACK);
            return -1;
        }

        SCLogDebug("ssn %p: ssn->server.next_win %" PRIu32 ", "
                "ssn->server.last_ack %"PRIu32"", ssn,
                ssn->server.next_win, ssn->server.last_ack);
    } else {
        SCLogDebug("ssn %p: default case", ssn);
    }

    return 0;
}