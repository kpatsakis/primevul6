routerstatus_format_entry(char *buf, size_t buf_len,
                          routerstatus_t *rs, const char *version,
                          routerstatus_format_type_t format)
{
  int r;
  struct in_addr in;
  char *cp;
  char *summary;

  char published[ISO_TIME_LEN+1];
  char ipaddr[INET_NTOA_BUF_LEN];
  char identity64[BASE64_DIGEST_LEN+1];
  char digest64[BASE64_DIGEST_LEN+1];

  format_iso_time(published, rs->published_on);
  digest_to_base64(identity64, rs->identity_digest);
  digest_to_base64(digest64, rs->descriptor_digest);
  in.s_addr = htonl(rs->addr);
  tor_inet_ntoa(&in, ipaddr, sizeof(ipaddr));

  r = tor_snprintf(buf, buf_len,
                   "r %s %s %s%s%s %s %d %d\n",
                   rs->nickname,
                   identity64,
                   (format==NS_V3_CONSENSUS_MICRODESC)?"":digest64,
                   (format==NS_V3_CONSENSUS_MICRODESC)?"":" ",
                   published,
                   ipaddr,
                   (int)rs->or_port,
                   (int)rs->dir_port);
  if (r<0) {
    log_warn(LD_BUG, "Not enough space in buffer.");
    return -1;
  }

  /* TODO: Maybe we want to pass in what we need to build the rest of
   * this here, instead of in the caller. Then we could use the
   * networkstatus_type_t values, with an additional control port value
   * added -MP */
  if (format == NS_V3_CONSENSUS || format == NS_V3_CONSENSUS_MICRODESC)
    return 0;

  cp = buf + strlen(buf);
  /* NOTE: Whenever this list expands, be sure to increase MAX_FLAG_LINE_LEN*/
  r = tor_snprintf(cp, buf_len - (cp-buf),
                   "s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
                  /* These must stay in alphabetical order. */
                   rs->is_authority?" Authority":"",
                   rs->is_bad_directory?" BadDirectory":"",
                   rs->is_bad_exit?" BadExit":"",
                   rs->is_exit?" Exit":"",
                   rs->is_fast?" Fast":"",
                   rs->is_possible_guard?" Guard":"",
                   rs->is_hs_dir?" HSDir":"",
                   rs->is_named?" Named":"",
                   rs->is_running?" Running":"",
                   rs->is_stable?" Stable":"",
                   rs->is_unnamed?" Unnamed":"",
                   rs->is_v2_dir?" V2Dir":"",
                   rs->is_valid?" Valid":"");
  if (r<0) {
    log_warn(LD_BUG, "Not enough space in buffer.");
    return -1;
  }
  cp += strlen(cp);

  /* length of "opt v \n" */
#define V_LINE_OVERHEAD 7
  if (version && strlen(version) < MAX_V_LINE_LEN - V_LINE_OVERHEAD) {
    if (tor_snprintf(cp, buf_len - (cp-buf), "opt v %s\n", version)<0) {
      log_warn(LD_BUG, "Unable to print router version.");
      return -1;
    }
    cp += strlen(cp);
  }

  if (format != NS_V2) {
    routerinfo_t* desc = router_get_by_digest(rs->identity_digest);
    uint32_t bw;

    if (format != NS_CONTROL_PORT) {
      /* Blow up more or less nicely if we didn't get anything or not the
       * thing we expected.
       */
      if (!desc) {
        char id[HEX_DIGEST_LEN+1];
        char dd[HEX_DIGEST_LEN+1];

        base16_encode(id, sizeof(id), rs->identity_digest, DIGEST_LEN);
        base16_encode(dd, sizeof(dd), rs->descriptor_digest, DIGEST_LEN);
        log_warn(LD_BUG, "Cannot get any descriptor for %s "
            "(wanted descriptor %s).",
            id, dd);
        return -1;
      };

      /* This assert can fire for the control port, because
       * it can request NS documents before all descriptors
       * have been fetched. */
      if (tor_memneq(desc->cache_info.signed_descriptor_digest,
            rs->descriptor_digest,
            DIGEST_LEN)) {
        char rl_d[HEX_DIGEST_LEN+1];
        char rs_d[HEX_DIGEST_LEN+1];
        char id[HEX_DIGEST_LEN+1];

        base16_encode(rl_d, sizeof(rl_d),
            desc->cache_info.signed_descriptor_digest, DIGEST_LEN);
        base16_encode(rs_d, sizeof(rs_d), rs->descriptor_digest, DIGEST_LEN);
        base16_encode(id, sizeof(id), rs->identity_digest, DIGEST_LEN);
        log_err(LD_BUG, "descriptor digest in routerlist does not match "
            "the one in routerstatus: %s vs %s "
            "(router %s)\n",
            rl_d, rs_d, id);

        tor_assert(tor_memeq(desc->cache_info.signed_descriptor_digest,
              rs->descriptor_digest,
              DIGEST_LEN));
      };
    }

    if (format == NS_CONTROL_PORT && rs->has_bandwidth) {
      bw = rs->bandwidth;
    } else {
      tor_assert(desc);
      bw = router_get_advertised_bandwidth_capped(desc) / 1000;
    }
    r = tor_snprintf(cp, buf_len - (cp-buf),
                     "w Bandwidth=%d\n", bw);

    if (r<0) {
      log_warn(LD_BUG, "Not enough space in buffer.");
      return -1;
    }
    cp += strlen(cp);
    if (format == NS_V3_VOTE && rs->has_measured_bw) {
      *--cp = '\0'; /* Kill "\n" */
      r = tor_snprintf(cp, buf_len - (cp-buf),
                       " Measured=%d\n", rs->measured_bw);
      if (r<0) {
        log_warn(LD_BUG, "Not enough space in buffer for weight line.");
        return -1;
      }
      cp += strlen(cp);
    }

    if (desc) {
      summary = policy_summarize(desc->exit_policy);
      r = tor_snprintf(cp, buf_len - (cp-buf), "p %s\n", summary);
      if (r<0) {
        log_warn(LD_BUG, "Not enough space in buffer.");
        tor_free(summary);
        return -1;
      }
      cp += strlen(cp);
      tor_free(summary);
    }
  }

  return 0;
}