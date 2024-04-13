static void mg_dns_free(struct dns_data *d) {
  LIST_DELETE(struct dns_data, &s_reqs, d);
  free(d);
}