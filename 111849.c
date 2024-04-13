flatpak_context_append_bwrap_filesystem (FlatpakContext  *context,
                                         FlatpakBwrap    *bwrap,
                                         const char      *app_id,
                                         GFile           *app_id_dir,
                                         FlatpakExports  *exports,
                                         const char      *xdg_dirs_conf,
                                         gboolean         home_access)
{
  GHashTableIter iter;
  gpointer key, value;

  if (app_id_dir != NULL)
    flatpak_run_apply_env_appid (bwrap, app_id_dir);

  if (!home_access)
    {
      /* Enable persistent mapping only if no access to real home dir */

      g_hash_table_iter_init (&iter, context->persistent);
      while (g_hash_table_iter_next (&iter, &key, NULL))
        {
          const char *persist = key;
          g_autofree char *src = g_build_filename (g_get_home_dir (), ".var/app", app_id, persist, NULL);
          g_autofree char *dest = g_build_filename (g_get_home_dir (), persist, NULL);

          g_mkdir_with_parents (src, 0755);

          flatpak_bwrap_add_bind_arg (bwrap, "--bind", src, dest);
        }
    }

  if (app_id_dir != NULL)
    {
      g_autofree char *user_runtime_dir = flatpak_get_real_xdg_runtime_dir ();
      g_autofree char *run_user_app_dst = g_strdup_printf ("/run/flatpak/app/%s", app_id);
      g_autofree char *run_user_app_src = g_build_filename (user_runtime_dir, "app", app_id, NULL);

      if (glnx_shutil_mkdir_p_at (AT_FDCWD,
                                  run_user_app_src,
                                  0700,
                                  NULL,
                                  NULL))
        flatpak_bwrap_add_args (bwrap,
                                "--bind", run_user_app_src, run_user_app_dst,
                                NULL);

      /* Later, we'll make $XDG_RUNTIME_DIR/app a symlink to /run/flatpak/app */
      flatpak_bwrap_add_runtime_dir_member (bwrap, "app");
    }

  /* This actually outputs the args for the hide/expose operations
   * in the exports */
  flatpak_exports_append_bwrap_args (exports, bwrap);

  /* Special case subdirectories of the cache, config and data xdg
   * dirs.  If these are accessible explicitly, then we bind-mount
   * these in the app-id dir. This allows applications to explicitly
   * opt out of keeping some config/cache/data in the app-specific
   * directory.
   */
  if (app_id_dir)
    {
      g_hash_table_iter_init (&iter, context->filesystems);
      while (g_hash_table_iter_next (&iter, &key, &value))
        {
          const char *filesystem = key;
          FlatpakFilesystemMode mode = GPOINTER_TO_INT (value);
          g_autofree char *xdg_path = NULL;
          const char *rest, *where;

          xdg_path = get_xdg_dir_from_string (filesystem, &rest, &where);

          if (xdg_path != NULL && *rest != 0 &&
              mode >= FLATPAK_FILESYSTEM_MODE_READ_ONLY)
            {
              g_autoptr(GFile) app_version = g_file_get_child (app_id_dir, where);
              g_autoptr(GFile) app_version_subdir = g_file_resolve_relative_path (app_version, rest);

              if (g_file_test (xdg_path, G_FILE_TEST_IS_DIR) ||
                  g_file_test (xdg_path, G_FILE_TEST_IS_REGULAR))
                {
                  g_autofree char *xdg_path_in_app = g_file_get_path (app_version_subdir);
                  flatpak_bwrap_add_bind_arg (bwrap,
                                              mode == FLATPAK_FILESYSTEM_MODE_READ_ONLY ? "--ro-bind" : "--bind",
                                              xdg_path, xdg_path_in_app);
                }
            }
        }
    }

  if (home_access && app_id_dir != NULL)
    {
      g_autofree char *src_path = g_build_filename (g_get_user_config_dir (),
                                                    "user-dirs.dirs",
                                                    NULL);
      g_autofree char *path = g_build_filename (flatpak_file_get_path_cached (app_id_dir),
                                                "config/user-dirs.dirs", NULL);
      if (g_file_test (src_path, G_FILE_TEST_EXISTS))
        flatpak_bwrap_add_bind_arg (bwrap, "--ro-bind", src_path, path);
    }
  else if (xdg_dirs_conf != NULL && xdg_dirs_conf[0] != '\0' && app_id_dir != NULL)
    {
      g_autofree char *path =
        g_build_filename (flatpak_file_get_path_cached (app_id_dir),
                          "config/user-dirs.dirs", NULL);

      flatpak_bwrap_add_args_data (bwrap, "xdg-config-dirs",
                                   xdg_dirs_conf, strlen (xdg_dirs_conf), path, NULL);
    }
}