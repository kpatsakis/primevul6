dirserv_get_name_status(const char *id_digest, const char *nickname)
{
  char fp[HEX_DIGEST_LEN+1];
  char *fp_by_name;

  base16_encode(fp, sizeof(fp), id_digest, DIGEST_LEN);

  if ((fp_by_name =
       strmap_get_lc(fingerprint_list->fp_by_name, nickname))) {
    if (!strcasecmp(fp, fp_by_name)) {
      return FP_NAMED;
    } else {
      return FP_UNNAMED; /* Wrong fingerprint. */
    }
  }
  return 0;
}