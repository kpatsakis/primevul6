static void test_util(void) {
  char buf[100], *s = mg_hexdump("abc", 3), *p;
  struct mg_addr a;
  ASSERT(s != NULL);
  free(s);
  memset(&a, 0, sizeof(a));
  ASSERT(mg_file_printf("data.txt", "%s", "hi") == true);
  ASSERT((p = mg_file_read("data.txt", NULL)) != NULL);
  ASSERT(strcmp(p, "hi") == 0);
  free(p);
  remove("data.txt");
  ASSERT(mg_aton(mg_str("0"), &a) == false);
  ASSERT(mg_aton(mg_str("0.0.0."), &a) == false);
  ASSERT(mg_aton(mg_str("0.0.0.256"), &a) == false);
  ASSERT(mg_aton(mg_str("0.0.0.-1"), &a) == false);
  ASSERT(mg_aton(mg_str("127.0.0.1"), &a) == true);
  ASSERT(a.is_ip6 == false);
  ASSERT(a.ip == 0x100007f);
  ASSERT(strcmp(mg_ntoa(&a, buf, sizeof(buf)), "127.0.0.1") == 0);

  ASSERT(mg_aton(mg_str("1:2:3:4:5:6:7:8"), &a) == true);
  ASSERT(a.is_ip6 == true);
  ASSERT(
      memcmp(a.ip6,
             "\x00\x01\x00\x02\x00\x03\x00\x04\x00\x05\x00\x06\x00\x07\x00\x08",
             sizeof(a.ip6)) == 0);

  memset(a.ip6, 0xaa, sizeof(a.ip6));
  ASSERT(mg_aton(mg_str("1::1"), &a) == true);
  ASSERT(a.is_ip6 == true);
  ASSERT(
      memcmp(a.ip6,
             "\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",
             sizeof(a.ip6)) == 0);

  memset(a.ip6, 0xaa, sizeof(a.ip6));
  ASSERT(mg_aton(mg_str("::fFff:1.2.3.4"), &a) == true);
  ASSERT(a.is_ip6 == true);
  ASSERT(memcmp(a.ip6,
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\xff\xff\x01\x02\x03\x04",
                sizeof(a.ip6)) == 0);

  memset(a.ip6, 0xaa, sizeof(a.ip6));
  ASSERT(mg_aton(mg_str("::1"), &a) == true);
  ASSERT(a.is_ip6 == true);
  ASSERT(
      memcmp(a.ip6,
             "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",
             sizeof(a.ip6)) == 0);

  memset(a.ip6, 0xaa, sizeof(a.ip6));
  ASSERT(mg_aton(mg_str("1::"), &a) == true);
  ASSERT(a.is_ip6 == true);
  ASSERT(
      memcmp(a.ip6,
             "\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
             sizeof(a.ip6)) == 0);

  memset(a.ip6, 0xaa, sizeof(a.ip6));
  ASSERT(mg_aton(mg_str("2001:4860:4860::8888"), &a) == true);
  ASSERT(a.is_ip6 == true);
  ASSERT(
      memcmp(a.ip6,
             "\x20\x01\x48\x60\x48\x60\x00\x00\x00\x00\x00\x00\x00\x00\x88\x88",
             sizeof(a.ip6)) == 0);

  ASSERT(strcmp(mg_hex("abc", 3, buf), "616263") == 0);
  ASSERT(mg_url_decode("a=%", 3, buf, sizeof(buf), 0) < 0);
  ASSERT(mg_url_decode("&&&a=%", 6, buf, sizeof(buf), 0) < 0);

  {
    size_t n;
    ASSERT((n = mg_url_encode("", 0, buf, sizeof(buf))) == 0);
    ASSERT((n = mg_url_encode("a", 1, buf, 0)) == 0);
    ASSERT((n = mg_url_encode("a", 1, buf, sizeof(buf))) == 1);
    ASSERT(strncmp(buf, "a", n) == 0);
    ASSERT((n = mg_url_encode("._-~", 4, buf, sizeof(buf))) == 4);
    ASSERT(strncmp(buf, "._-~", n) == 0);
    ASSERT((n = mg_url_encode("a@%>", 4, buf, sizeof(buf))) == 10);
    ASSERT(strncmp(buf, "a%40%25%3e", n) == 0);
    ASSERT((n = mg_url_encode("a@b.c", 5, buf, sizeof(buf))) == 7);
    ASSERT(strncmp(buf, "a%40b.c", n) == 0);
  }

  {
    s = buf;
    mg_asprintf(&s, sizeof(buf), "%s", "%3d", 123);
    ASSERT(s == buf);
    ASSERT(strcmp(buf, "%3d") == 0);
    mg_asprintf(&s, sizeof(buf), "%.*s", 7, "a%40b.c");
    ASSERT(s == buf);
    ASSERT(strcmp(buf, "a%40b.c") == 0);
  }

  ASSERT(mg_to64(mg_str("-9223372036854775809")) == 0);
  ASSERT(mg_to64(mg_str("9223372036854775800")) == 0);
  ASSERT(mg_to64(mg_str("9223372036854775700")) > 0);
}