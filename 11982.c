dirserv_would_reject_router(routerstatus_t *rs)
{
  uint32_t res;

  res = dirserv_get_status_impl(rs->identity_digest, rs->nickname,
                                "", /* address is only used in logs */
                                rs->addr, rs->or_port,
                                NULL, NULL,
                                NULL, 0);

  return (res & FP_REJECT) != 0;
}