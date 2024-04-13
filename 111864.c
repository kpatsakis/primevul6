unparse_filesystem_flags (const char           *path,
                          FlatpakFilesystemMode mode)
{
  g_autoptr(GString) s = g_string_new ("");
  const char *p;

  for (p = path; *p != 0; p++)
    {
      if (*p == ':')
        g_string_append (s, "\\:");
      else if (*p == '\\')
        g_string_append (s, "\\\\");
      else
        g_string_append_c (s, *p);
    }

  switch (mode)
    {
    case FLATPAK_FILESYSTEM_MODE_READ_ONLY:
      g_string_append (s, ":ro");
      break;

    case FLATPAK_FILESYSTEM_MODE_CREATE:
      g_string_append (s, ":create");
      break;

    case FLATPAK_FILESYSTEM_MODE_READ_WRITE:
      break;

    case FLATPAK_FILESYSTEM_MODE_NONE:
      g_string_insert_c (s, 0, '!');
      break;

    default:
      g_warning ("Unexpected filesystem mode %d", mode);
      break;
    }

  return g_string_free (g_steal_pointer (&s), FALSE);
}