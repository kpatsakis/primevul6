static struct mg_str guess_content_type(struct mg_str path, const char *extra) {
  // clang-format off
  struct mimeentry { struct mg_str extension, value; };
  #define MIME_ENTRY(a, b) {{a, sizeof(a) - 1 }, { b, sizeof(b) - 1 }}
  // clang-format on
  const struct mimeentry tab[] = {
      MIME_ENTRY("html", "text/html; charset=utf-8"),
      MIME_ENTRY("htm", "text/html; charset=utf-8"),
      MIME_ENTRY("css", "text/css; charset=utf-8"),
      MIME_ENTRY("js", "text/javascript; charset=utf-8"),
      MIME_ENTRY("gif", "image/gif"),
      MIME_ENTRY("png", "image/png"),
      MIME_ENTRY("jpg", "image/jpeg"),
      MIME_ENTRY("jpeg", "image/jpeg"),
      MIME_ENTRY("woff", "font/woff"),
      MIME_ENTRY("ttf", "font/ttf"),
      MIME_ENTRY("svg", "image/svg+xml"),
      MIME_ENTRY("txt", "text/plain; charset=utf-8"),
      MIME_ENTRY("avi", "video/x-msvideo"),
      MIME_ENTRY("csv", "text/csv"),
      MIME_ENTRY("doc", "application/msword"),
      MIME_ENTRY("exe", "application/octet-stream"),
      MIME_ENTRY("gz", "application/gzip"),
      MIME_ENTRY("ico", "image/x-icon"),
      MIME_ENTRY("json", "application/json"),
      MIME_ENTRY("mov", "video/quicktime"),
      MIME_ENTRY("mp3", "audio/mpeg"),
      MIME_ENTRY("mp4", "video/mp4"),
      MIME_ENTRY("mpeg", "video/mpeg"),
      MIME_ENTRY("pdf", "application/pdf"),
      MIME_ENTRY("shtml", "text/html; charset=utf-8"),
      MIME_ENTRY("tgz", "application/tar-gz"),
      MIME_ENTRY("wav", "audio/wav"),
      MIME_ENTRY("webp", "image/webp"),
      MIME_ENTRY("zip", "application/zip"),
      MIME_ENTRY("3gp", "video/3gpp"),
      {{0, 0}, {0, 0}},
  };
  size_t i = 0;
  struct mg_str k, v, s = mg_str(extra);

  // Shrink path to its extension only
  while (i < path.len && path.ptr[path.len - i - 1] != '.') i++;
  path.ptr += path.len - i;
  path.len = i;

  // Process user-provided mime type overrides, if any
  while (mg_commalist(&s, &k, &v)) {
    if (mg_strcmp(path, k) == 0) return v;
  }

  // Process built-in mime types
  for (i = 0; tab[i].extension.ptr != NULL; i++) {
    if (mg_strcmp(path, tab[i].extension) == 0) return tab[i].value;
  }

  return mg_str("text/plain; charset=utf-8");
}