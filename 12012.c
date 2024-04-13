dirserv_single_reachability_test(time_t now, routerinfo_t *router)
{
  tor_addr_t router_addr;
  log_debug(LD_OR,"Testing reachability of %s at %s:%u.",
            router->nickname, router->address, router->or_port);
  /* Remember when we started trying to determine reachability */
  if (!router->testing_since)
    router->testing_since = now;
  tor_addr_from_ipv4h(&router_addr, router->addr);
  connection_or_connect(&router_addr, router->or_port,
                        router->cache_info.identity_digest);
}