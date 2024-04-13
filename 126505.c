static void test_sntp(void) {
  int64_t ms = 0;
  struct mg_mgr mgr;
  struct mg_connection *c = NULL;
  int i;

  mg_mgr_init(&mgr);
  c = mg_sntp_connect(&mgr, NULL, sntp_cb, &ms);
  ASSERT(c != NULL);
  ASSERT(c->is_udp == 1);
  mg_sntp_send(c, (unsigned long) time(NULL));
  for (i = 0; i < 300 && ms == 0; i++) mg_mgr_poll(&mgr, 10);
  ASSERT(ms > 0);
  mg_mgr_free(&mgr);

  {
    const unsigned char sntp_good[] =
        "\x24\x02\x00\xeb\x00\x00\x00\x1e\x00\x00\x07\xb6\x3e"
        "\xc9\xd6\xa2\xdb\xde\xea\x30\x91\x86\xb7\x10\xdb\xde"
        "\xed\x98\x00\x00\x00\xde\xdb\xde\xed\x99\x0a\xe2\xc7"
        "\x96\xdb\xde\xed\x99\x0a\xe4\x6b\xda";
    const unsigned char bad_good[] =
        "\x55\x02\x00\xeb\x00\x00\x00\x1e\x00\x00\x07\xb6\x3e"
        "\xc9\xd6\xa2\xdb\xde\xea\x30\x91\x86\xb7\x10\xdb\xde"
        "\xed\x98\x00\x00\x00\xde\xdb\xde\xed\x99\x0a\xe2\xc7"
        "\x96\xdb\xde\xed\x99\x0a\xe4\x6b\xda";
    struct tm tm;
    time_t t;
    ASSERT((ms = mg_sntp_parse(sntp_good, sizeof(sntp_good))) > 0);
    t = (time_t) (ms / 1000);
    gmtime_r(&t, &tm);
    ASSERT(tm.tm_year == 116);
    ASSERT(tm.tm_mon == 10);
    ASSERT(tm.tm_mday == 22);
    ASSERT(tm.tm_hour == 16);
    ASSERT(tm.tm_min == 15);
    ASSERT(tm.tm_sec == 21);
    ASSERT(mg_sntp_parse(bad_good, sizeof(bad_good)) < 0);
  }

  ASSERT(mg_sntp_parse(NULL, 0) == -1);
}