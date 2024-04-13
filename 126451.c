static int p_stat(const char *path, size_t *size, time_t *mtime) {
#if !defined(S_ISDIR)
  LOG(LL_ERROR, ("stat() API is not supported. %p %p %p", path, size, mtime));
  return 0;
#else
#if defined(_WIN32)
  struct _stati64 st;
  wchar_t tmp[PATH_MAX];
  MultiByteToWideChar(CP_UTF8, 0, path, -1, tmp, sizeof(tmp) / sizeof(tmp[0]));
  if (_wstati64(tmp, &st) != 0) return 0;
#else
  struct MG_STAT_STRUCT st;
  if (MG_STAT_FUNC(path, &st) != 0) return 0;
#endif
  if (size) *size = (size_t) st.st_size;
  if (mtime) *mtime = st.st_mtime;
  return MG_FS_READ | MG_FS_WRITE | (S_ISDIR(st.st_mode) ? MG_FS_DIR : 0);
#endif
}