static void test_http_parse(void) {
  struct mg_str *v;
  struct mg_http_message req;

  {
    const char *s = "GET / HTTP/1.0\n\n";
    ASSERT(mg_http_parse("\b23", 3, &req) == -1);
    ASSERT(mg_http_parse("get\n\n", 5, &req) == -1);
    ASSERT(mg_http_parse(s, strlen(s) - 1, &req) == 0);
    ASSERT(mg_http_parse(s, strlen(s), &req) == (int) strlen(s));
    ASSERT(req.message.len == strlen(s));
    ASSERT(req.body.len == 0);
  }

  {
    const char *s = "GET /blah HTTP/1.0\r\nFoo:  bar  \r\n\r\n";
    size_t idx, len = strlen(s);
    ASSERT(mg_http_parse(s, strlen(s), &req) == (int) len);
    ASSERT(mg_vcmp(&req.headers[0].name, "Foo") == 0);
    ASSERT(mg_vcmp(&req.headers[0].value, "bar") == 0);
    ASSERT(req.headers[1].name.len == 0);
    ASSERT(req.headers[1].name.ptr == NULL);
    ASSERT(req.query.len == 0);
    ASSERT(req.message.len == len);
    ASSERT(req.body.len == 0);
    for (idx = 0; idx < len; idx++) ASSERT(mg_http_parse(s, idx, &req) == 0);
  }

  {
    static const char *s = "get b c\nz :  k \nb: t\nvvv\n\n xx";
    ASSERT(mg_http_parse(s, strlen(s), &req) == (int) strlen(s) - 3);
    ASSERT(req.headers[2].name.len == 0);
    ASSERT(mg_vcmp(&req.headers[0].value, "k") == 0);
    ASSERT(mg_vcmp(&req.headers[1].value, "t") == 0);
    ASSERT(req.body.len == 0);
  }

  {
    const char *s = "a b c\r\nContent-Length: 21 \r\nb: t\r\nvvv\r\n\r\nabc";
    ASSERT(mg_http_parse(s, strlen(s), &req) == (int) strlen(s) - 3);
    ASSERT(req.body.len == 21);
    ASSERT(req.message.len == 21 - 3 + strlen(s));
    ASSERT(mg_http_get_header(&req, "foo") == NULL);
    ASSERT((v = mg_http_get_header(&req, "contENT-Length")) != NULL);
    ASSERT(mg_vcmp(v, "21") == 0);
    ASSERT((v = mg_http_get_header(&req, "B")) != NULL);
    ASSERT(mg_vcmp(v, "t") == 0);
  }

  {
    const char *s = "GET /foo?a=b&c=d HTTP/1.0\n\n";
    ASSERT(mg_http_parse(s, strlen(s), &req) == (int) strlen(s));
    ASSERT(mg_vcmp(&req.uri, "/foo") == 0);
    ASSERT(mg_vcmp(&req.query, "a=b&c=d") == 0);
  }

  {
    const char *s = "POST /x HTTP/1.0\n\n";
    ASSERT(mg_http_parse(s, strlen(s), &req) == (int) strlen(s));
    ASSERT(req.body.len == (size_t) ~0);
  }

  {
    const char *s = "WOHOO /x HTTP/1.0\n\n";
    ASSERT(mg_http_parse(s, strlen(s), &req) == (int) strlen(s));
    ASSERT(req.body.len == 0);
  }

  {
    const char *s = "HTTP/1.0 200 OK\n\n";
    ASSERT(mg_http_parse(s, strlen(s), &req) == (int) strlen(s));
    ASSERT(mg_vcmp(&req.method, "HTTP/1.0") == 0);
    ASSERT(mg_vcmp(&req.uri, "200") == 0);
    ASSERT(mg_vcmp(&req.proto, "OK") == 0);
    ASSERT(req.body.len == (size_t) ~0);
  }

  {
    static const char *s = "HTTP/1.0 999 OMGWTFBBQ\n\n";
    ASSERT(mg_http_parse(s, strlen(s), &req) == (int) strlen(s));
  }

  {
    const char *s =
        "GET / HTTP/1.0\r\nhost:127.0.0.1:18888\r\nCookie:\r\nX-PlayID: "
        "45455\r\nRange:  0-1 \r\n\r\n";
    ASSERT(mg_http_parse(s, strlen(s), &req) == (int) strlen(s));
    ASSERT((v = mg_http_get_header(&req, "Host")) != NULL);
    ASSERT(mg_vcmp(v, "127.0.0.1:18888") == 0);
    ASSERT((v = mg_http_get_header(&req, "Cookie")) != NULL);
    ASSERT(v->len == 0);
    ASSERT((v = mg_http_get_header(&req, "X-PlayID")) != NULL);
    ASSERT(mg_vcmp(v, "45455") == 0);
    ASSERT((v = mg_http_get_header(&req, "Range")) != NULL);
    ASSERT(mg_vcmp(v, "0-1") == 0);
  }

  {
    static const char *s = "a b c\na:1\nb:2\nc:3\nd:4\ne:5\nf:6\ng:7\nh:8\n\n";
    ASSERT(mg_http_parse(s, strlen(s), &req) == (int) strlen(s));
    ASSERT((v = mg_http_get_header(&req, "e")) != NULL);
    ASSERT(mg_vcmp(v, "5") == 0);
    ASSERT((v = mg_http_get_header(&req, "h")) == NULL);
  }

  {
    struct mg_connection c;
    struct mg_str s,
        res = mg_str("GET /\r\nAuthorization: Basic Zm9vOmJhcg==\r\n\r\n");
    memset(&c, 0, sizeof(c));
    mg_printf(&c, "%s", "GET /\r\n");
    mg_http_bauth(&c, "foo", "bar");
    mg_printf(&c, "%s", "\r\n");
    s = mg_str_n((char *) c.send.buf, c.send.len);
    ASSERT(mg_strcmp(s, res) == 0);
    mg_iobuf_free(&c.send);
  }

  {
    struct mg_http_message hm;
    const char *s = "GET /foo?bar=baz HTTP/1.0\n\n ";
    ASSERT(mg_http_parse(s, strlen(s), &hm) == (int) strlen(s) - 1);
    ASSERT(mg_strcmp(hm.uri, mg_str("/foo")) == 0);
    ASSERT(mg_strcmp(hm.query, mg_str("bar=baz")) == 0);
  }

  {
    struct mg_http_message hm;
    const char *s = "a b c\n\n";
    ASSERT(mg_http_parse(s, strlen(s), &hm) == (int) strlen(s));
    s = "a b\nc\n\n";
    ASSERT(mg_http_parse(s, strlen(s), &hm) == (int) strlen(s));
    s = "a\nb\nc\n\n";
    ASSERT(mg_http_parse(s, strlen(s), &hm) < 0);
  }
}