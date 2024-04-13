generate_runningrouters(void)
{
  char *s=NULL;
  char digest[DIGEST_LEN];
  char published[ISO_TIME_LEN+1];
  size_t len;
  crypto_pk_env_t *private_key = get_server_identity_key();
  char *identity_pkey; /* Identity key, DER64-encoded. */
  size_t identity_pkey_len;

  if (crypto_pk_write_public_key_to_string(private_key,&identity_pkey,
                                           &identity_pkey_len)<0) {
    log_warn(LD_BUG,"write identity_pkey to string failed!");
    goto err;
  }
  format_iso_time(published, time(NULL));

  len = 2048;
  s = tor_malloc_zero(len);
  tor_snprintf(s, len,
               "network-status\n"
               "published %s\n"
               "router-status %s\n"
               "dir-signing-key\n%s"
               "directory-signature %s\n",
               published, "", identity_pkey,
               get_options()->Nickname);
  tor_free(identity_pkey);
  if (router_get_runningrouters_hash(s,digest)) {
    log_warn(LD_BUG,"couldn't compute digest");
    goto err;
  }
  note_crypto_pk_op(SIGN_DIR);
  if (router_append_dirobj_signature(s, len, digest, DIGEST_LEN,
                                     private_key)<0)
    goto err;

  set_cached_dir(&the_runningrouters, s, time(NULL));
  runningrouters_is_dirty = 0;

  return &the_runningrouters;
 err:
  tor_free(s);
  return NULL;
}