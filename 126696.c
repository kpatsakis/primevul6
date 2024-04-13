static void test_dns(void) {
  struct mg_dns_message dm;
  //       txid  flags numQ  numA  numAP numOP
  // 0000  00 01 81 80 00 01 00 01 00 00 00 00 07 63 65 73  .............ces
  // 0010  61 6e 74 61 03 63 6f 6d 00 00 01 00 01 c0 0c 00  anta.com........
  // 0020  01 00 01 00 00 02 57 00 04 94 fb 36 ec           ......W....6.
  uint8_t data[] = {0,    1,    0x81, 0x80, 0,    1,    0,    1,    0,
                    0,    0,    0,    7,    0x63, 0x65, 0x73, 0x61, 0x6e,
                    0x74, 0x61, 0x03, 0x63, 0x6f, 0x6d, 0,    0,    1,
                    0,    1,    0xc0, 0x0c, 0,    1,    0,    1,    0,
                    0,    2,    0x57, 0,    4,    0x94, 0xfb, 0x36, 0xec};
  ASSERT(mg_dns_parse(NULL, 0, &dm) == 0);
  ASSERT(mg_dns_parse(data, sizeof(data), &dm) == 1);
  ASSERT(strcmp(dm.name, "cesanta.com") == 0);
  data[30] = 29;  // Point a pointer to itself
  memset(&dm, 0, sizeof(dm));
  ASSERT(mg_dns_parse(data, sizeof(data), &dm) == 1);
  ASSERT(strcmp(dm.name, "") == 0);

  test_dns_timeout("udp://127.0.0.1:12345", "DNS timeout");
  test_dns_timeout("", "resolver");
  test_dns_timeout("tcp://0.0.0.0:0", "DNS error");
}