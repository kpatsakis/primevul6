generate_v2_networkstatus_opinion(void)
{
  cached_dir_t *r = NULL;
  size_t len, identity_pkey_len;
  char *status = NULL, *client_versions = NULL, *server_versions = NULL,
    *identity_pkey = NULL, *hostname = NULL;
  char *outp, *endp;
  or_options_t *options = get_options();
  char fingerprint[FINGERPRINT_LEN+1];
  char ipaddr[INET_NTOA_BUF_LEN];
  char published[ISO_TIME_LEN+1];
  char digest[DIGEST_LEN];
  struct in_addr in;
  uint32_t addr;
  crypto_pk_env_t *private_key;
  routerlist_t *rl = router_get_routerlist();
  time_t now = time(NULL);
  time_t cutoff = now - ROUTER_MAX_AGE_TO_PUBLISH;
  int naming = options->NamingAuthoritativeDir;
  int versioning = options->VersioningAuthoritativeDir;
  int listbaddirs = options->AuthDirListBadDirs;
  int listbadexits = options->AuthDirListBadExits;
  int vote_on_hsdirs = options->VoteOnHidServDirectoriesV2;
  const char *contact;
  char *version_lines = NULL;
  smartlist_t *routers = NULL;
  digestmap_t *omit_as_sybil = NULL;

  private_key = get_server_identity_key();

  if (resolve_my_address(LOG_WARN, options, &addr, &hostname)<0) {
    log_warn(LD_NET, "Couldn't resolve my hostname");
    goto done;
  }
  in.s_addr = htonl(addr);
  tor_inet_ntoa(&in, ipaddr, sizeof(ipaddr));

  format_iso_time(published, now);

  client_versions = format_versions_list(options->RecommendedClientVersions);
  server_versions = format_versions_list(options->RecommendedServerVersions);

  if (crypto_pk_write_public_key_to_string(private_key, &identity_pkey,
                                           &identity_pkey_len)<0) {
    log_warn(LD_BUG,"Writing public key to string failed.");
    goto done;
  }

  if (crypto_pk_get_fingerprint(private_key, fingerprint, 0)<0) {
    log_err(LD_BUG, "Error computing fingerprint");
    goto done;
  }

  contact = get_options()->ContactInfo;
  if (!contact)
    contact = "(none)";

  if (versioning) {
    size_t v_len = 64+strlen(client_versions)+strlen(server_versions);
    version_lines = tor_malloc(v_len);
    tor_snprintf(version_lines, v_len,
                 "client-versions %s\nserver-versions %s\n",
                 client_versions, server_versions);
  } else {
    version_lines = tor_strdup("");
  }

  len = 4096+strlen(client_versions)+strlen(server_versions);
  len += identity_pkey_len*2;
  len += (RS_ENTRY_LEN)*smartlist_len(rl->routers);

  status = tor_malloc(len);
  tor_snprintf(status, len,
               "network-status-version 2\n"
               "dir-source %s %s %d\n"
               "fingerprint %s\n"
               "contact %s\n"
               "published %s\n"
               "dir-options%s%s%s%s\n"
               "%s" /* client version line, server version line. */
               "dir-signing-key\n%s",
               hostname, ipaddr,
               (int)router_get_advertised_dir_port(options, 0),
               fingerprint,
               contact,
               published,
               naming ? " Names" : "",
               listbaddirs ? " BadDirectories" : "",
               listbadexits ? " BadExits" : "",
               versioning ? " Versions" : "",
               version_lines,
               identity_pkey);
  outp = status + strlen(status);
  endp = status + len;

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

  SMARTLIST_FOREACH(routers, routerinfo_t *, ri, {
    if (ri->cache_info.published_on >= cutoff) {
      routerstatus_t rs;
      char *version = version_from_platform(ri->platform);

      set_routerstatus_from_routerinfo(&rs, ri, now,
                                       naming, listbadexits, listbaddirs,
                                       vote_on_hsdirs);

      if (digestmap_get(omit_as_sybil, ri->cache_info.identity_digest))
        clear_status_flags_on_sybil(&rs);

      if (routerstatus_format_entry(outp, endp-outp, &rs, version, NS_V2)) {
        log_warn(LD_BUG, "Unable to print router status.");
        tor_free(version);
        goto done;
      }
      tor_free(version);
      outp += strlen(outp);
    }
  });

  if (tor_snprintf(outp, endp-outp, "directory-signature %s\n",
                   get_options()->Nickname)<0) {
    log_warn(LD_BUG, "Unable to write signature line.");
    goto done;
  }
  if (router_get_networkstatus_v2_hash(status, digest)<0) {
    log_warn(LD_BUG, "Unable to hash network status");
    goto done;
  }
  outp += strlen(outp);

  note_crypto_pk_op(SIGN_DIR);
  if (router_append_dirobj_signature(outp,endp-outp,digest,DIGEST_LEN,
                                     private_key)<0) {
    log_warn(LD_BUG, "Unable to sign router status.");
    goto done;
  }

  {
    networkstatus_v2_t *ns;
    if (!(ns = networkstatus_v2_parse_from_string(status))) {
      log_err(LD_BUG,"Generated a networkstatus we couldn't parse.");
      goto done;
    }
    networkstatus_v2_free(ns);
  }

  {
    cached_dir_t **ns_ptr = &the_v2_networkstatus;
    if (*ns_ptr)
      cached_dir_decref(*ns_ptr);
    *ns_ptr = new_cached_dir(status, now);
    status = NULL; /* So it doesn't get double-freed. */
    the_v2_networkstatus_is_dirty = 0;
    router_set_networkstatus_v2((*ns_ptr)->dir, now, NS_GENERATED, NULL);
    r = *ns_ptr;
  }

 done:
  tor_free(client_versions);
  tor_free(server_versions);
  tor_free(version_lines);
  tor_free(status);
  tor_free(hostname);
  tor_free(identity_pkey);
  smartlist_free(routers);
  digestmap_free(omit_as_sybil, NULL);
  return r;
}