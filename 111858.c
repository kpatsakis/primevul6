flatpak_context_export (FlatpakContext *context,
                        FlatpakExports *exports,
                        GFile          *app_id_dir,
                        GPtrArray       *extra_app_id_dirs,
                        gboolean        do_create,
                        gchar         **xdg_dirs_conf_out,
                        gboolean       *home_access_out)
{
  gboolean home_access = FALSE;
  g_autoptr(GString) xdg_dirs_conf = NULL;
  FlatpakFilesystemMode fs_mode, os_mode, etc_mode, home_mode;
  GHashTableIter iter;
  gpointer key, value;

  if (xdg_dirs_conf_out != NULL)
    xdg_dirs_conf = g_string_new ("");

  fs_mode = GPOINTER_TO_INT (g_hash_table_lookup (context->filesystems, "host"));
  if (fs_mode != FLATPAK_FILESYSTEM_MODE_NONE)
    {
      DIR *dir;
      struct dirent *dirent;

      g_debug ("Allowing host-fs access");
      home_access = TRUE;

      /* Bind mount most dirs in / into the new root */
      dir = opendir ("/");
      if (dir != NULL)
        {
          while ((dirent = readdir (dir)))
            {
              g_autofree char *path = NULL;

              if (g_strv_contains (dont_mount_in_root, dirent->d_name))
                continue;

              path = g_build_filename ("/", dirent->d_name, NULL);
              flatpak_exports_add_path_expose (exports, fs_mode, path);
            }
          closedir (dir);
        }
      flatpak_exports_add_path_expose (exports, fs_mode, "/run/media");
    }

  os_mode = MAX (GPOINTER_TO_INT (g_hash_table_lookup (context->filesystems, "host-os")),
                   fs_mode);

  if (os_mode != FLATPAK_FILESYSTEM_MODE_NONE)
    flatpak_exports_add_host_os_expose (exports, os_mode);

  etc_mode = MAX (GPOINTER_TO_INT (g_hash_table_lookup (context->filesystems, "host-etc")),
                   fs_mode);

  if (etc_mode != FLATPAK_FILESYSTEM_MODE_NONE)
    flatpak_exports_add_host_etc_expose (exports, etc_mode);

  home_mode = GPOINTER_TO_INT (g_hash_table_lookup (context->filesystems, "home"));
  if (home_mode != FLATPAK_FILESYSTEM_MODE_NONE)
    {
      g_debug ("Allowing homedir access");
      home_access = TRUE;

      flatpak_exports_add_path_expose (exports, MAX (home_mode, fs_mode), g_get_home_dir ());
    }

  g_hash_table_iter_init (&iter, context->filesystems);
  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      const char *filesystem = key;
      FlatpakFilesystemMode mode = GPOINTER_TO_INT (value);

      if (g_strv_contains (flatpak_context_special_filesystems, filesystem))
        continue;

      if (g_str_has_prefix (filesystem, "xdg-"))
        {
          const char *path, *rest = NULL;
          const char *config_key = NULL;
          g_autofree char *subpath = NULL;

          if (!get_xdg_user_dir_from_string (filesystem, &config_key, &rest, &path))
            {
              g_warning ("Unsupported xdg dir %s", filesystem);
              continue;
            }

          if (path == NULL)
            continue; /* Unconfigured, ignore */

          if (strcmp (path, g_get_home_dir ()) == 0)
            {
              /* xdg-user-dirs sets disabled dirs to $HOME, and its in general not a good
                 idea to set full access to $HOME other than explicitly, so we ignore
                 these */
              g_debug ("Xdg dir %s is $HOME (i.e. disabled), ignoring", filesystem);
              continue;
            }

          subpath = g_build_filename (path, rest, NULL);

          if (mode == FLATPAK_FILESYSTEM_MODE_CREATE && do_create)
            g_mkdir_with_parents (subpath, 0755);

          if (g_file_test (subpath, G_FILE_TEST_EXISTS))
            {
              if (config_key && xdg_dirs_conf)
                g_string_append_printf (xdg_dirs_conf, "%s=\"%s\"\n",
                                        config_key, path);

              flatpak_exports_add_path_expose_or_hide (exports, mode, subpath);
            }
        }
      else if (g_str_has_prefix (filesystem, "~/"))
        {
          g_autofree char *path = NULL;

          path = g_build_filename (g_get_home_dir (), filesystem + 2, NULL);

          if (mode == FLATPAK_FILESYSTEM_MODE_CREATE && do_create)
            g_mkdir_with_parents (path, 0755);

          if (g_file_test (path, G_FILE_TEST_EXISTS))
            flatpak_exports_add_path_expose_or_hide (exports, mode, path);
        }
      else if (g_str_has_prefix (filesystem, "/"))
        {
          if (mode == FLATPAK_FILESYSTEM_MODE_CREATE && do_create)
            g_mkdir_with_parents (filesystem, 0755);

          if (g_file_test (filesystem, G_FILE_TEST_EXISTS))
            flatpak_exports_add_path_expose_or_hide (exports, mode, filesystem);
        }
      else
        {
          g_warning ("Unexpected filesystem arg %s", filesystem);
        }
    }

  if (app_id_dir)
    {
      g_autoptr(GFile) apps_dir = g_file_get_parent (app_id_dir);
      int i;
      /* Hide the .var/app dir by default (unless explicitly made visible) */
      flatpak_exports_add_path_tmpfs (exports, flatpak_file_get_path_cached (apps_dir));
      /* But let the app write to the per-app dir in it */
      flatpak_exports_add_path_expose (exports, FLATPAK_FILESYSTEM_MODE_READ_WRITE,
                                       flatpak_file_get_path_cached (app_id_dir));

      if (extra_app_id_dirs != NULL)
        {
          for (i = 0; i < extra_app_id_dirs->len; i++)
            {
              GFile *extra_app_id_dir = g_ptr_array_index (extra_app_id_dirs, i);
              flatpak_exports_add_path_expose (exports, FLATPAK_FILESYSTEM_MODE_READ_WRITE,
                                               flatpak_file_get_path_cached (extra_app_id_dir));
            }
        }
    }

  if (home_access_out != NULL)
    *home_access_out = home_access;

  if (xdg_dirs_conf_out != NULL)
    {
      g_assert (xdg_dirs_conf != NULL);
      *xdg_dirs_conf_out = g_string_free (g_steal_pointer (&xdg_dirs_conf), FALSE);
    }
}