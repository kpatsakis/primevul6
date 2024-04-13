flatpak_context_parse_filesystem (const char             *filesystem_and_mode,
                                  char                  **filesystem_out,
                                  FlatpakFilesystemMode  *mode_out,
                                  GError                **error)
{
  g_autofree char *filesystem = parse_filesystem_flags (filesystem_and_mode, mode_out);
  char *slash;

  slash = strchr (filesystem, '/');

  /* Forbid /../ in paths */
  if (slash != NULL)
    {
      if (g_str_has_prefix (slash + 1, "../") ||
          g_str_has_suffix (slash + 1, "/..") ||
          strstr (slash + 1, "/../") != NULL)
        {
          g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
                       _("Filesystem location \"%s\" contains \"..\""),
                       filesystem);
          return FALSE;
        }

      /* Convert "//" and "/./" to "/" */
      for (; slash != NULL; slash = strchr (slash + 1, '/'))
        {
          while (TRUE)
            {
              if (slash[1] == '/')
                memmove (slash + 1, slash + 2, strlen (slash + 2) + 1);
              else if (slash[1] == '.' && slash[2] == '/')
                memmove (slash + 1, slash + 3, strlen (slash + 3) + 1);
              else
                break;
            }
        }

      /* Eliminate trailing "/." or "/". */
      while (TRUE)
        {
          slash = strrchr (filesystem, '/');

          if (slash != NULL &&
              ((slash != filesystem && slash[1] == '\0') ||
               (slash[1] == '.' && slash[2] == '\0')))
            *slash = '\0';
          else
            break;
        }

      if (filesystem[0] == '/' && filesystem[1] == '\0')
        {
          /* We don't allow --filesystem=/ as equivalent to host, because
           * it doesn't do what you'd think: --filesystem=host mounts some
           * host directories in /run/host, not in the root. */
          g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
                       _("--filesystem=/ is not available, "
                         "use --filesystem=host for a similar result"));
          return FALSE;
        }
    }

  if (g_strv_contains (flatpak_context_special_filesystems, filesystem) ||
      get_xdg_user_dir_from_string (filesystem, NULL, NULL, NULL) ||
      g_str_has_prefix (filesystem, "~/") ||
      g_str_has_prefix (filesystem, "/"))
    {
      if (filesystem_out != NULL)
        *filesystem_out = g_steal_pointer (&filesystem);

      return TRUE;
    }

  if (strcmp (filesystem, "~") == 0)
    {
      if (filesystem_out != NULL)
        *filesystem_out = g_strdup ("home");

      return TRUE;
    }

  if (g_str_has_prefix (filesystem, "home/"))
    {
      if (filesystem_out != NULL)
        *filesystem_out = g_strconcat ("~/", filesystem + 5, NULL);

      return TRUE;
    }

  g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED,
               _("Unknown filesystem location %s, valid locations are: host, host-os, host-etc, home, xdg-*[/…], ~/dir, /dir"), filesystem);
  return FALSE;
}