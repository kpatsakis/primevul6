static void test_url(void) {
  // Host
  ASSERT(vcmp(mg_url_host("foo"), "foo"));
  ASSERT(vcmp(mg_url_host("//foo"), "foo"));
  ASSERT(vcmp(mg_url_host("foo:1234"), "foo"));
  ASSERT(vcmp(mg_url_host(":1234"), ""));
  ASSERT(vcmp(mg_url_host("//foo:1234"), "foo"));
  ASSERT(vcmp(mg_url_host("p://foo"), "foo"));
  ASSERT(vcmp(mg_url_host("p://foo/"), "foo"));
  ASSERT(vcmp(mg_url_host("p://foo/x"), "foo"));
  ASSERT(vcmp(mg_url_host("p://foo/x/"), "foo"));
  ASSERT(vcmp(mg_url_host("p://foo/x//"), "foo"));
  ASSERT(vcmp(mg_url_host("p://foo//x"), "foo"));
  ASSERT(vcmp(mg_url_host("p://foo///x"), "foo"));
  ASSERT(vcmp(mg_url_host("p://foo///x//"), "foo"));
  ASSERT(vcmp(mg_url_host("p://bar:1234"), "bar"));
  ASSERT(vcmp(mg_url_host("p://bar:1234/"), "bar"));
  ASSERT(vcmp(mg_url_host("p://bar:1234/a"), "bar"));
  ASSERT(vcmp(mg_url_host("p://u@bar:1234/a"), "bar"));
  ASSERT(vcmp(mg_url_host("p://u:p@bar:1234/a"), "bar"));
  ASSERT(vcmp(mg_url_host("p://u:p@[::1]:1234/a"), "[::1]"));
  ASSERT(vcmp(mg_url_host("p://u:p@[1:2::3]:1234/a"), "[1:2::3]"));
  ASSERT(vcmp(mg_url_host("p://foo/x:y/z"), "foo"));

  // Port
  ASSERT(mg_url_port("foo:1234") == 1234);
  ASSERT(mg_url_port(":1234") == 1234);
  ASSERT(mg_url_port("x://foo:1234") == 1234);
  ASSERT(mg_url_port("x://foo:1234/") == 1234);
  ASSERT(mg_url_port("x://foo:1234/xx") == 1234);
  ASSERT(mg_url_port("x://foo:1234") == 1234);
  ASSERT(mg_url_port("p://bar:1234/a") == 1234);
  ASSERT(mg_url_port("p://bar:1234/a:b") == 1234);
  ASSERT(mg_url_port("http://bar") == 80);
  ASSERT(mg_url_port("http://localhost:1234") == 1234);
  ASSERT(mg_url_port("https://bar") == 443);
  ASSERT(mg_url_port("wss://bar") == 443);
  ASSERT(mg_url_port("wss://u:p@bar") == 443);
  ASSERT(mg_url_port("wss://u:p@bar:123") == 123);
  ASSERT(mg_url_port("wss://u:p@bar:123/") == 123);
  ASSERT(mg_url_port("wss://u:p@bar:123/abc") == 123);
  ASSERT(mg_url_port("http://u:p@[::1]/abc") == 80);
  ASSERT(mg_url_port("http://u:p@[::1]:2121/abc") == 2121);
  ASSERT(mg_url_port("http://u:p@[::1]:2121/abc/cd:ef") == 2121);

  // User / pass
  ASSERT(vcmp(mg_url_user("p://foo"), ""));
  ASSERT(vcmp(mg_url_pass("p://foo"), ""));
  ASSERT(vcmp(mg_url_user("p://:@foo"), ""));
  ASSERT(vcmp(mg_url_pass("p://:@foo"), ""));
  ASSERT(vcmp(mg_url_user("p://u@foo"), "u"));
  ASSERT(vcmp(mg_url_pass("p://u@foo"), ""));
  ASSERT(vcmp(mg_url_user("p://u:@foo"), "u"));
  ASSERT(vcmp(mg_url_pass("p://u:@foo"), ""));
  ASSERT(vcmp(mg_url_user("p://:p@foo"), ""));
  ASSERT(vcmp(mg_url_pass("p://:p@foo"), "p"));
  ASSERT(vcmp(mg_url_user("p://u:p@foo"), "u"));
  ASSERT(vcmp(mg_url_pass("p://u:p@foo"), "p"));
  ASSERT(vcmp(mg_url_pass("p://u:p@foo//a@b"), "p"));

  // URI
  ASSERT(strcmp(mg_url_uri("p://foo"), "/") == 0);
  ASSERT(strcmp(mg_url_uri("p://foo/"), "/") == 0);
  ASSERT(strcmp(mg_url_uri("p://foo:12/"), "/") == 0);
  ASSERT(strcmp(mg_url_uri("p://foo:12/abc"), "/abc") == 0);
  ASSERT(strcmp(mg_url_uri("p://foo:12/a/b/c"), "/a/b/c") == 0);
  ASSERT(strcmp(mg_url_uri("p://[::1]:12/a/b/c"), "/a/b/c") == 0);
  ASSERT(strcmp(mg_url_uri("p://[ab::1]:12/a/b/c"), "/a/b/c") == 0);
}