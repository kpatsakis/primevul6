static CURLcode tftp_receive_packet(struct connectdata *conn)
{
  struct Curl_sockaddr_storage fromaddr;
  curl_socklen_t        fromlen;
  CURLcode              result = CURLE_OK;
  struct Curl_easy  *data = conn->data;
  tftp_state_data_t     *state = (tftp_state_data_t *)conn->proto.tftpc;
  struct SingleRequest  *k = &data->req;

  /* Receive the packet */
  fromlen = sizeof(fromaddr);
  state->rbytes = (int)recvfrom(state->sockfd,
                                (void *)state->rpacket.data,
                                state->blksize + 4,
                                0,
                                (struct sockaddr *)&fromaddr,
                                &fromlen);
  if(state->remote_addrlen == 0) {
    memcpy(&state->remote_addr, &fromaddr, fromlen);
    state->remote_addrlen = fromlen;
  }

  /* Sanity check packet length */
  if(state->rbytes < 4) {
    failf(data, "Received too short packet");
    /* Not a timeout, but how best to handle it? */
    state->event = TFTP_EVENT_TIMEOUT;
  }
  else {
    /* The event is given by the TFTP packet time */
    unsigned short event = getrpacketevent(&state->rpacket);
    state->event = (tftp_event_t)event;

    switch(state->event) {
    case TFTP_EVENT_DATA:
      /* Don't pass to the client empty or retransmitted packets */
      if(state->rbytes > 4 &&
         (NEXT_BLOCKNUM(state->block) == getrpacketblock(&state->rpacket))) {
        result = Curl_client_write(conn, CLIENTWRITE_BODY,
                                   (char *)state->rpacket.data + 4,
                                   state->rbytes-4);
        if(result) {
          tftp_state_machine(state, TFTP_EVENT_ERROR);
          return result;
        }
        k->bytecount += state->rbytes-4;
        Curl_pgrsSetDownloadCounter(data, (curl_off_t) k->bytecount);
      }
      break;
    case TFTP_EVENT_ERROR:
    {
      unsigned short error = getrpacketblock(&state->rpacket);
      char *str = (char *)state->rpacket.data + 4;
      size_t strn = state->rbytes - 4;
      state->error = (tftp_error_t)error;
      if(Curl_strnlen(str, strn) < strn)
        infof(data, "TFTP error: %s\n", str);
      break;
    }
    case TFTP_EVENT_ACK:
      break;
    case TFTP_EVENT_OACK:
      result = tftp_parse_option_ack(state,
                                     (const char *)state->rpacket.data + 2,
                                     state->rbytes-2);
      if(result)
        return result;
      break;
    case TFTP_EVENT_RRQ:
    case TFTP_EVENT_WRQ:
    default:
      failf(data, "%s", "Internal error: Unexpected packet");
      break;
    }

    /* Update the progress meter */
    if(Curl_pgrsUpdate(conn)) {
      tftp_state_machine(state, TFTP_EVENT_ERROR);
      return CURLE_ABORTED_BY_CALLBACK;
    }
  }
  return result;
}