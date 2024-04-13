dirserv_get_status_impl(const char *id_digest, const char *nickname,
                        const char *address,
                        uint32_t addr, uint16_t or_port,
                        const char *platform, const char *contact,
                        const char **msg, int should_log)
{
  int reject_unlisted = get_options()->AuthDirRejectUnlisted;
  uint32_t result = 0;
  router_status_t *status_by_digest;

  if (!fingerprint_list)
    fingerprint_list = authdir_config_new();

  if (should_log)
    log_debug(LD_DIRSERV, "%d fingerprints, %d digests known.",
              strmap_size(fingerprint_list->fp_by_name),
              digestmap_size(fingerprint_list->status_by_digest));

  /* Tor 0.2.0.26-rc is the oldest version that currently caches the right
   * directory information.  Once more of them die off, we should raise this
   * minimum. */
  if (platform && !tor_version_as_new_as(platform,"0.2.0.26-rc")) {
    if (msg)
      *msg = "Tor version is far too old to work.";
    return FP_REJECT;
  } else if (platform && tor_version_as_new_as(platform,"0.2.1.3-alpha")
                      && !tor_version_as_new_as(platform, "0.2.1.19")) {
    /* These versions mishandled RELAY_EARLY cells on rend circuits. */
    if (msg)
      *msg = "Tor version is too buggy to work.";
    return FP_REJECT;
  }

  result = dirserv_get_name_status(id_digest, nickname);
  if (result & FP_NAMED) {
    if (should_log)
      log_debug(LD_DIRSERV,"Good fingerprint for '%s'",nickname);
  }
  if (result & FP_UNNAMED) {
    if (should_log) {
      char *esc_contact = esc_for_log(contact);
      log_info(LD_DIRSERV,
               "Mismatched fingerprint for '%s'. "
               "ContactInfo '%s', platform '%s'.)",
               nickname,
               esc_contact,
               platform ? escaped(platform) : "");
      tor_free(esc_contact);
    }
    if (msg)
      *msg = "Rejected: There is already a named server with this nickname "
        "and a different fingerprint.";
  }

  status_by_digest = digestmap_get(fingerprint_list->status_by_digest,
                                   id_digest);
  if (status_by_digest)
    result |= (status_by_digest->status & ~FP_NAMED);

  if (result & FP_REJECT) {
    if (msg)
      *msg = "Fingerprint is marked rejected";
    return FP_REJECT;
  } else if (result & FP_INVALID) {
    if (msg)
      *msg = "Fingerprint is marked invalid";
  }

  if (authdir_policy_baddir_address(addr, or_port)) {
    if (should_log)
      log_info(LD_DIRSERV,
               "Marking '%s' as bad directory because of address '%s'",
               nickname, address);
    result |= FP_BADDIR;
  }

  if (authdir_policy_badexit_address(addr, or_port)) {
    if (should_log)
      log_info(LD_DIRSERV, "Marking '%s' as bad exit because of address '%s'",
               nickname, address);
    result |= FP_BADEXIT;
  }

  if (!(result & FP_NAMED)) {
    if (!authdir_policy_permits_address(addr, or_port)) {
      if (should_log)
        log_info(LD_DIRSERV, "Rejecting '%s' because of address '%s'",
                 nickname, address);
      if (msg)
        *msg = "Authdir is rejecting routers in this range.";
      return FP_REJECT;
    }
    if (!authdir_policy_valid_address(addr, or_port)) {
      if (should_log)
        log_info(LD_DIRSERV, "Not marking '%s' valid because of address '%s'",
                 nickname, address);
      result |= FP_INVALID;
    }
    if (reject_unlisted) {
      if (msg)
        *msg = "Authdir rejects unknown routers.";
      return FP_REJECT;
    }
  }

  return result;
}