static void printdirentry(const char *name, void *userdata) {
  struct printdirentrydata *d = (struct printdirentrydata *) userdata;
  struct mg_fs *fs = d->opts->fs == NULL ? &mg_fs_posix : d->opts->fs;
  size_t size = 0;
  time_t t = 0;
  char path[MG_PATH_MAX], sz[64], mod[64];
  int flags, n = 0;

  // LOG(LL_DEBUG, ("[%s] [%s]", d->dir, name));
  if (snprintf(path, sizeof(path), "%s%c%s", d->dir, '/', name) < 0) {
    LOG(LL_ERROR, ("%s truncated", name));
  } else if ((flags = fs->stat(path, &size, &t)) == 0) {
    LOG(LL_ERROR, ("%lu stat(%s): %d", d->c->id, path, errno));
  } else {
    const char *slash = flags & MG_FS_DIR ? "/" : "";
    struct tm tm;
    if (flags & MG_FS_DIR) {
      snprintf(sz, sizeof(sz), "%s", "[DIR]");
    } else if (size < 1024) {
      snprintf(sz, sizeof(sz), "%d", (int) size);
    } else if (size < 0x100000) {
      snprintf(sz, sizeof(sz), "%.1fk", (double) size / 1024.0);
    } else if (size < 0x40000000) {
      snprintf(sz, sizeof(sz), "%.1fM", (double) size / 1048576);
    } else {
      snprintf(sz, sizeof(sz), "%.1fG", (double) size / 1073741824);
    }
    strftime(mod, sizeof(mod), "%d-%b-%Y %H:%M", localtime_r(&t, &tm));
    n = (int) mg_url_encode(name, strlen(name), path, sizeof(path));
    mg_printf(d->c,
              "  <tr><td><a href=\"%.*s%s\">%s%s</a></td>"
              "<td name=%lu>%s</td><td name=" MG_INT64_FMT ">%s</td></tr>\n",
              n, path, slash, name, slash, (unsigned long) t, mod,
              flags & MG_FS_DIR ? (int64_t) -1 : (int64_t) size, sz);
  }
}