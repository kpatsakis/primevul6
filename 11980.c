dirserv_free_fingerprint_list(void)
{
  if (!fingerprint_list)
    return;

  strmap_free(fingerprint_list->fp_by_name, _tor_free);
  digestmap_free(fingerprint_list->status_by_digest, _tor_free);
  tor_free(fingerprint_list);
}