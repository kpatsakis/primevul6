dirserv_router_get_status(const routerinfo_t *router, const char **msg)
{
  char d[DIGEST_LEN];

  if (crypto_pk_get_digest(router->identity_pkey, d)) {
    log_warn(LD_BUG,"Error computing fingerprint");
    if (msg)
      *msg = "Bug: Error computing fingerprint";
    return FP_REJECT;
  }

  return dirserv_get_status_impl(d, router->nickname,
                                 router->address,
                                 router->addr, router->or_port,
                                 router->platform, router->contact_info,
                                 msg, 1);
}