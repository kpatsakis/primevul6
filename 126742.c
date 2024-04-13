void mg_http_serve_file(struct mg_connection *c, struct mg_http_message *hm,
                        const char *path, struct mg_http_serve_opts *opts) {
  char etag[64];
  struct mg_fs *fs = opts->fs == NULL ? &mg_fs_posix : opts->fs;
  struct mg_fd *fd = fs->open(path, MG_FS_READ);
  size_t size = 0;
  time_t mtime = 0;
  struct mg_str *inm = NULL;

  if (fd == NULL || fs->stat(path, &size, &mtime) == 0) {
    LOG(LL_DEBUG, ("404 [%s] %p", path, (void *) fd));
    mg_http_reply(c, 404, "", "%s", "Not found\n");
    fs->close(fd);
    // NOTE: mg_http_etag() call should go first!
  } else if (mg_http_etag(etag, sizeof(etag), size, mtime) != NULL &&
             (inm = mg_http_get_header(hm, "If-None-Match")) != NULL &&
             mg_vcasecmp(inm, etag) == 0) {
    fs->close(fd);
    mg_printf(c, "HTTP/1.1 304 Not Modified\r\nContent-Length: 0\r\n\r\n");
  } else {
    int n, status = 200;
    char range[100] = "";
    int64_t r1 = 0, r2 = 0, cl = (int64_t) size;
    struct mg_str mime = guess_content_type(mg_str(path), opts->mime_types);

    // Handle Range header
    struct mg_str *rh = mg_http_get_header(hm, "Range");
    if (rh != NULL && (n = getrange(rh, &r1, &r2)) > 0 && r1 >= 0 && r2 >= 0) {
      // If range is specified like "400-", set second limit to content len
      if (n == 1) r2 = cl - 1;
      if (r1 > r2 || r2 >= cl) {
        status = 416;
        cl = 0;
        snprintf(range, sizeof(range),
                 "Content-Range: bytes */" MG_INT64_FMT "\r\n", (int64_t) size);
      } else {
        status = 206;
        cl = r2 - r1 + 1;
        snprintf(range, sizeof(range),
                 "Content-Range: bytes " MG_INT64_FMT "-" MG_INT64_FMT
                 "/" MG_INT64_FMT "\r\n",
                 r1, r1 + cl - 1, (int64_t) size);
        fs->seek(fd->fd, (size_t) r1);
      }
    }

    mg_printf(c,
              "HTTP/1.1 %d %s\r\nContent-Type: %.*s\r\n"
              "Etag: %s\r\nContent-Length: " MG_INT64_FMT "\r\n%s%s\r\n",
              status, mg_http_status_code_str(status), (int) mime.len, mime.ptr,
              etag, cl, range, opts->extra_headers ? opts->extra_headers : "");
    if (mg_vcasecmp(&hm->method, "HEAD") == 0) {
      c->is_draining = 1;
      fs->close(fd);
    } else {
      c->pfn = static_cb;
      c->pfn_data = fd;
    }
  }
}