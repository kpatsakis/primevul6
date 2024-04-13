static void test_check_ip_acl(void) {
  uint32_t ip = 0x01020304;
  ASSERT(mg_check_ip_acl(mg_str(NULL), ip) == 1);
  ASSERT(mg_check_ip_acl(mg_str(""), ip) == 1);
  ASSERT(mg_check_ip_acl(mg_str("invalid"), ip) == -1);
  ASSERT(mg_check_ip_acl(mg_str("+hi"), ip) == -2);
  ASSERT(mg_check_ip_acl(mg_str("+//"), ip) == -2);
  ASSERT(mg_check_ip_acl(mg_str("-0.0.0.0/0"), ip) == 0);
  ASSERT(mg_check_ip_acl(mg_str("-0.0.0.0/0,+1.0.0.0/8"), ip) == 1);
  ASSERT(mg_check_ip_acl(mg_str("-0.0.0.0/0,+1.2.3.4"), ip) == 1);
  ASSERT(mg_check_ip_acl(mg_str("-0.0.0.0/0,+1.0.0.0/16"), ip) == 0);
}