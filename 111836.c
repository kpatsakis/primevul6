parse_filesystem_flags (const char            *filesystem,
                        FlatpakFilesystemMode *mode_out)
{
  g_autoptr(GString) s = g_string_new ("");
  const char *p, *suffix;
  FlatpakFilesystemMode mode;

  p = filesystem;
  while (*p != 0 && *p != ':')
    {
      if (*p == '\\')
        {
          p++;
          if (*p != 0)
            g_string_append_c (s, *p++);
        }
      else
        g_string_append_c (s, *p++);
    }

  mode = FLATPAK_FILESYSTEM_MODE_READ_WRITE;

  if (*p == ':')
    {
      suffix = p + 1;

      if (strcmp (suffix, "ro") == 0)
        mode = FLATPAK_FILESYSTEM_MODE_READ_ONLY;
      else if (strcmp (suffix, "rw") == 0)
        mode = FLATPAK_FILESYSTEM_MODE_READ_WRITE;
      else if (strcmp (suffix, "create") == 0)
        mode = FLATPAK_FILESYSTEM_MODE_CREATE;
      else if (*suffix != 0)
        g_warning ("Unexpected filesystem suffix %s, ignoring", suffix);
    }

  if (mode_out)
    *mode_out = mode;

  return g_string_free (g_steal_pointer (&s), FALSE);
}