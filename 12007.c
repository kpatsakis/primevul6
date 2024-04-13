add_fingerprint_to_dir(const char *nickname, const char *fp,
                       authdir_config_t *list)
{
  char *fingerprint;
  char d[DIGEST_LEN];
  router_status_t *status;
  tor_assert(nickname);
  tor_assert(fp);
  tor_assert(list);

  fingerprint = tor_strdup(fp);
  tor_strstrip(fingerprint, " ");
  if (base16_decode(d, DIGEST_LEN, fingerprint, strlen(fingerprint))) {
    log_warn(LD_DIRSERV, "Couldn't decode fingerprint \"%s\"",
             escaped(fp));
    tor_free(fingerprint);
    return 0;
  }

  if (!strcasecmp(nickname, UNNAMED_ROUTER_NICKNAME)) {
    log_warn(LD_DIRSERV, "Tried to add a mapping for reserved nickname %s",
             UNNAMED_ROUTER_NICKNAME);
    tor_free(fingerprint);
    return 0;
  }

  status = digestmap_get(list->status_by_digest, d);
  if (!status) {
    status = tor_malloc_zero(sizeof(router_status_t));
    digestmap_set(list->status_by_digest, d, status);
  }

  if (nickname[0] != '!') {
    char *old_fp = strmap_get_lc(list->fp_by_name, nickname);
    if (old_fp && !strcasecmp(fingerprint, old_fp)) {
      tor_free(fingerprint);
    } else {
      tor_free(old_fp);
      strmap_set_lc(list->fp_by_name, nickname, fingerprint);
    }
    status->status |= FP_NAMED;
    strlcpy(status->nickname, nickname, sizeof(status->nickname));
  } else {
    tor_free(fingerprint);
    if (!strcasecmp(nickname, "!reject")) {
      status->status |= FP_REJECT;
    } else if (!strcasecmp(nickname, "!invalid")) {
      status->status |= FP_INVALID;
    } else if (!strcasecmp(nickname, "!baddir")) {
      status->status |= FP_BADDIR;
    } else if (!strcasecmp(nickname, "!badexit")) {
      status->status |= FP_BADEXIT;
    }
  }
  return 0;
}