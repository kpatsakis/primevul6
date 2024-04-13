dirserv_load_fingerprint_file(void)
{
  char *fname;
  char *cf;
  char *nickname, *fingerprint;
  authdir_config_t *fingerprint_list_new;
  int result;
  config_line_t *front=NULL, *list;
  or_options_t *options = get_options();

  fname = get_datadir_fname("approved-routers");
  log_info(LD_GENERAL,
           "Reloading approved fingerprints from \"%s\"...", fname);

  cf = read_file_to_str(fname, RFTS_IGNORE_MISSING, NULL);
  if (!cf) {
    if (options->NamingAuthoritativeDir) {
      log_warn(LD_FS, "Cannot open fingerprint file '%s'. Failing.", fname);
      tor_free(fname);
      return -1;
    } else {
      log_info(LD_FS, "Cannot open fingerprint file '%s'. That's ok.", fname);
      tor_free(fname);
      return 0;
    }
  }
  tor_free(fname);

  result = config_get_lines(cf, &front);
  tor_free(cf);
  if (result < 0) {
    log_warn(LD_CONFIG, "Error reading from fingerprint file");
    return -1;
  }

  fingerprint_list_new = authdir_config_new();

  for (list=front; list; list=list->next) {
    char digest_tmp[DIGEST_LEN];
    nickname = list->key; fingerprint = list->value;
    if (strlen(nickname) > MAX_NICKNAME_LEN) {
      log_notice(LD_CONFIG,
                 "Nickname '%s' too long in fingerprint file. Skipping.",
                 nickname);
      continue;
    }
    if (!is_legal_nickname(nickname) &&
        strcasecmp(nickname, "!reject") &&
        strcasecmp(nickname, "!invalid") &&
        strcasecmp(nickname, "!badexit")) {
      log_notice(LD_CONFIG,
                 "Invalid nickname '%s' in fingerprint file. Skipping.",
                 nickname);
      continue;
    }
    tor_strstrip(fingerprint, " "); /* remove spaces */
    if (strlen(fingerprint) != HEX_DIGEST_LEN ||
        base16_decode(digest_tmp, sizeof(digest_tmp),
                      fingerprint, HEX_DIGEST_LEN) < 0) {
      log_notice(LD_CONFIG,
                 "Invalid fingerprint (nickname '%s', "
                 "fingerprint %s). Skipping.",
                 nickname, fingerprint);
      continue;
    }
    if (0==strcasecmp(nickname, DEFAULT_CLIENT_NICKNAME)) {
      /* If you approved an OR called "client", then clients who use
       * the default nickname could all be rejected.  That's no good. */
      log_notice(LD_CONFIG,
                 "Authorizing nickname '%s' would break "
                 "many clients; skipping.",
                 DEFAULT_CLIENT_NICKNAME);
      continue;
    }
    if (0==strcasecmp(nickname, UNNAMED_ROUTER_NICKNAME)) {
      /* If you approved an OR called "unnamed", then clients will be
       * confused. */
      log_notice(LD_CONFIG,
                 "Authorizing nickname '%s' is not allowed; skipping.",
                 UNNAMED_ROUTER_NICKNAME);
      continue;
    }
    if (add_fingerprint_to_dir(nickname, fingerprint, fingerprint_list_new)
        != 0)
      log_notice(LD_CONFIG, "Duplicate nickname '%s'.", nickname);
  }

  config_free_lines(front);
  dirserv_free_fingerprint_list();
  fingerprint_list = fingerprint_list_new;
  /* Delete any routers whose fingerprints we no longer recognize */
  directory_remove_invalid();
  return 0;
}