static CURLcode tftp_connect(struct connectdata *conn, bool *done)
{
  tftp_state_data_t *state;
  int blksize;
  int need_blksize;

  blksize = TFTP_BLKSIZE_DEFAULT;

  state = conn->proto.tftpc = calloc(1, sizeof(tftp_state_data_t));
  if(!state)
    return CURLE_OUT_OF_MEMORY;

  /* alloc pkt buffers based on specified blksize */
  if(conn->data->set.tftp_blksize) {
    blksize = (int)conn->data->set.tftp_blksize;
    if(blksize > TFTP_BLKSIZE_MAX || blksize < TFTP_BLKSIZE_MIN)
      return CURLE_TFTP_ILLEGAL;
  }

  need_blksize = blksize;
  /* default size is the fallback when no OACK is received */
  if(need_blksize < TFTP_BLKSIZE_DEFAULT)
    need_blksize = TFTP_BLKSIZE_DEFAULT;

  if(!state->rpacket.data) {
    state->rpacket.data = calloc(1, need_blksize + 2 + 2);

    if(!state->rpacket.data)
      return CURLE_OUT_OF_MEMORY;
  }

  if(!state->spacket.data) {
    state->spacket.data = calloc(1, need_blksize + 2 + 2);

    if(!state->spacket.data)
      return CURLE_OUT_OF_MEMORY;
  }

  /* we don't keep TFTP connections up basically because there's none or very
   * little gain for UDP */
  connclose(conn, "TFTP");

  state->conn = conn;
  state->sockfd = state->conn->sock[FIRSTSOCKET];
  state->state = TFTP_STATE_START;
  state->error = TFTP_ERR_NONE;
  state->blksize = TFTP_BLKSIZE_DEFAULT; /* Unless updated by OACK response */
  state->requested_blksize = blksize;

  ((struct sockaddr *)&state->local_addr)->sa_family =
    (CURL_SA_FAMILY_T)(conn->ip_addr->ai_family);

  tftp_set_timeouts(state);

  if(!conn->bits.bound) {
    /* If not already bound, bind to any interface, random UDP port. If it is
     * reused or a custom local port was desired, this has already been done!
     *
     * We once used the size of the local_addr struct as the third argument
     * for bind() to better work with IPv6 or whatever size the struct could
     * have, but we learned that at least Tru64, AIX and IRIX *requires* the
     * size of that argument to match the exact size of a 'sockaddr_in' struct
     * when running IPv4-only.
     *
     * Therefore we use the size from the address we connected to, which we
     * assume uses the same IP version and thus hopefully this works for both
     * IPv4 and IPv6...
     */
    int rc = bind(state->sockfd, (struct sockaddr *)&state->local_addr,
                  conn->ip_addr->ai_addrlen);
    if(rc) {
      char buffer[STRERROR_LEN];
      failf(conn->data, "bind() failed; %s",
            Curl_strerror(SOCKERRNO, buffer, sizeof(buffer)));
      return CURLE_COULDNT_CONNECT;
    }
    conn->bits.bound = TRUE;
  }

  Curl_pgrsStartNow(conn->data);

  *done = TRUE;

  return CURLE_OK;
}