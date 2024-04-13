IsUnixSocket(const char *name)
{
  struct stat sb;
  if(stat(name, &sb) == 0 && (sb.st_mode & S_IFMT) == S_IFSOCK)
    return TRUE;
  return FALSE;
}