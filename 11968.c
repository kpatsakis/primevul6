dirserv_generate_networkstatus_vote_obj(crypto_pk_env_t *private_key,
                                        authority_cert_t *cert)
{
  or_options_t *options = get_options();
  networkstatus_t *v3_out = NULL;
  uint32_t addr;
  char *hostname = NULL, *client_versions = NULL, *server_versions = NULL;
  const char *contact;
  smartlist_t *routers, *routerstatuses;
  char identity_digest[DIGEST_LEN];
  char signing_key_digest[DIGEST_LEN];
  int naming = options->NamingAuthoritativeDir;
  int listbadexits = options->AuthDirListBadExits;
  int listbaddirs = options->AuthDirListBadDirs;
  int vote_on_hsdirs = options->VoteOnHidServDirectoriesV2;
  routerlist_t *rl = router_get_routerlist();
  time_t now = time(NULL);
  time_t cutoff = now - ROUTER_MAX_AGE_TO_PUBLISH;
  networkstatus_voter_info_t *voter = NULL;
  vote_timing_t timing;
  digestmap_t *omit_as_sybil = NULL;
  const int vote_on_reachability = running_long_enough_to_decide_unreachable();
  smartlist_t *microdescriptors = NULL;

  tor_assert(private_key);
  tor_assert(cert);

  if (resolve_my_address(LOG_WARN, options, &addr, &hostname)<0) {
    log_warn(LD_NET, "Couldn't resolve my hostname");
    return NULL;
  }
  if (!strchr(hostname, '.')) {
    tor_free(hostname);
    hostname = tor_dup_ip(addr);
  }
  if (crypto_pk_get_digest(private_key, signing_key_digest)<0) {
    log_err(LD_BUG, "Error computing signing key digest");
    return NULL;
  }
  if (crypto_pk_get_digest(cert->identity_key, identity_digest)<0) {
    log_err(LD_BUG, "Error computing identity key digest");
    return NULL;
  }

  if (options->VersioningAuthoritativeDir) {
    client_versions = format_versions_list(options->RecommendedClientVersions);
    server_versions = format_versions_list(options->RecommendedServerVersions);
  }

  contact = get_options()->ContactInfo;
  if (!contact)
    contact = "(none)";

  /* precompute this part, since we need it to decide what "stable"
   * means. */
  SMARTLIST_FOREACH(rl->routers, routerinfo_t *, ri, {
    dirserv_set_router_is_running(ri, now);
  });

  dirserv_compute_performance_thresholds(rl);

  routers = smartlist_create();
  smartlist_add_all(routers, rl->routers);
  routers_sort_by_identity(routers);
  omit_as_sybil = get_possible_sybil_list(routers);

  routerstatuses = smartlist_create();
  microdescriptors = smartlist_create();

  SMARTLIST_FOREACH_BEGIN(routers, routerinfo_t *, ri) {
    if (ri->cache_info.published_on >= cutoff) {
      routerstatus_t *rs;
      vote_routerstatus_t *vrs;
      microdesc_t *md;

      vrs = tor_malloc_zero(sizeof(vote_routerstatus_t));
      rs = &vrs->status;
      set_routerstatus_from_routerinfo(rs, ri, now,
                                       naming, listbadexits, listbaddirs,
                                       vote_on_hsdirs);

      if (digestmap_get(omit_as_sybil, ri->cache_info.identity_digest))
        clear_status_flags_on_sybil(rs);

      if (!vote_on_reachability)
        rs->is_running = 0;

      vrs->version = version_from_platform(ri->platform);
      md = dirvote_create_microdescriptor(ri);
      if (md) {
        char buf[128];
        vote_microdesc_hash_t *h;
        dirvote_format_microdesc_vote_line(buf, sizeof(buf), md);
        h = tor_malloc(sizeof(vote_microdesc_hash_t));
        h->microdesc_hash_line = tor_strdup(buf);
        h->next = NULL;
        vrs->microdesc = h;
        md->last_listed = now;
        smartlist_add(microdescriptors, md);
      }

      smartlist_add(routerstatuses, vrs);
    }
  } SMARTLIST_FOREACH_END(ri);

  {
    smartlist_t *added =
      microdescs_add_list_to_cache(get_microdesc_cache(),
                                   microdescriptors, SAVED_NOWHERE, 0);
    smartlist_free(added);
    smartlist_free(microdescriptors);
  }

  smartlist_free(routers);
  digestmap_free(omit_as_sybil, NULL);

  if (options->V3BandwidthsFile) {
    dirserv_read_measured_bandwidths(options->V3BandwidthsFile,
                                     routerstatuses);
  }

  v3_out = tor_malloc_zero(sizeof(networkstatus_t));

  v3_out->type = NS_TYPE_VOTE;
  dirvote_get_preferred_voting_intervals(&timing);
  v3_out->published = now;
  {
    char tbuf[ISO_TIME_LEN+1];
    networkstatus_t *current_consensus =
      networkstatus_get_live_consensus(now);
    long last_consensus_interval; /* only used to pick a valid_after */
    if (current_consensus)
      last_consensus_interval = current_consensus->fresh_until -
        current_consensus->valid_after;
    else
      last_consensus_interval = options->TestingV3AuthInitialVotingInterval;
    v3_out->valid_after =
      dirvote_get_start_of_next_interval(now, (int)last_consensus_interval);
    format_iso_time(tbuf, v3_out->valid_after);
    log_notice(LD_DIR,"Choosing valid-after time in vote as %s: "
               "consensus_set=%d, last_interval=%d",
               tbuf, current_consensus?1:0, (int)last_consensus_interval);
  }
  v3_out->fresh_until = v3_out->valid_after + timing.vote_interval;
  v3_out->valid_until = v3_out->valid_after +
    (timing.vote_interval * timing.n_intervals_valid);
  v3_out->vote_seconds = timing.vote_delay;
  v3_out->dist_seconds = timing.dist_delay;
  tor_assert(v3_out->vote_seconds > 0);
  tor_assert(v3_out->dist_seconds > 0);
  tor_assert(timing.n_intervals_valid > 0);

  v3_out->client_versions = client_versions;
  v3_out->server_versions = server_versions;
  v3_out->known_flags = smartlist_create();
  smartlist_split_string(v3_out->known_flags,
                "Authority Exit Fast Guard Stable V2Dir Valid",
                0, SPLIT_SKIP_SPACE|SPLIT_IGNORE_BLANK, 0);
  if (vote_on_reachability)
    smartlist_add(v3_out->known_flags, tor_strdup("Running"));
  if (listbaddirs)
    smartlist_add(v3_out->known_flags, tor_strdup("BadDirectory"));
  if (listbadexits)
    smartlist_add(v3_out->known_flags, tor_strdup("BadExit"));
  if (naming) {
    smartlist_add(v3_out->known_flags, tor_strdup("Named"));
    smartlist_add(v3_out->known_flags, tor_strdup("Unnamed"));
  }
  if (vote_on_hsdirs)
    smartlist_add(v3_out->known_flags, tor_strdup("HSDir"));
  smartlist_sort_strings(v3_out->known_flags);

  if (options->ConsensusParams) {
    v3_out->net_params = smartlist_create();
    smartlist_split_string(v3_out->net_params,
                           options->ConsensusParams, NULL, 0, 0);
    smartlist_sort_strings(v3_out->net_params);
  }

  voter = tor_malloc_zero(sizeof(networkstatus_voter_info_t));
  voter->nickname = tor_strdup(options->Nickname);
  memcpy(voter->identity_digest, identity_digest, DIGEST_LEN);
  voter->sigs = smartlist_create();
  voter->address = hostname;
  voter->addr = addr;
  voter->dir_port = router_get_advertised_dir_port(options, 0);
  voter->or_port = router_get_advertised_or_port(options);
  voter->contact = tor_strdup(contact);
  if (options->V3AuthUseLegacyKey) {
    authority_cert_t *c = get_my_v3_legacy_cert();
    if (c) {
      if (crypto_pk_get_digest(c->identity_key, voter->legacy_id_digest)) {
        log_warn(LD_BUG, "Unable to compute digest of legacy v3 identity key");
        memset(voter->legacy_id_digest, 0, DIGEST_LEN);
      }
    }
  }

  v3_out->voters = smartlist_create();
  smartlist_add(v3_out->voters, voter);
  v3_out->cert = authority_cert_dup(cert);
  v3_out->routerstatus_list = routerstatuses;
  /* Note: networkstatus_digest is unset; it won't get set until we actually
   * format the vote. */

  return v3_out;
}