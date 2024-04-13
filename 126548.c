void mg_resolve_cancel(struct mg_connection *c) {
  struct dns_data *tmp, *d;
  for (d = s_reqs; d != NULL; d = tmp) {
    tmp = d->next;
    if (d->c == c) mg_dns_free(d);
  }
}