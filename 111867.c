flatpak_context_get_exports_full (FlatpakContext *context,
                                  GFile          *app_id_dir,
                                  GPtrArray      *extra_app_id_dirs,
                                  gboolean        do_create,
                                  gboolean        include_default_dirs,
                                  gchar         **xdg_dirs_conf_out,
                                  gboolean       *home_access_out)
{
  g_autoptr(FlatpakExports) exports = flatpak_exports_new ();

  flatpak_context_export (context, exports,
                          app_id_dir, extra_app_id_dirs,
                          do_create, xdg_dirs_conf_out, home_access_out);

  if (include_default_dirs)
    {
      g_autoptr(GFile) user_flatpak_dir = NULL;

      /* Hide the flatpak dir by default (unless explicitly made visible) */
      user_flatpak_dir = flatpak_get_user_base_dir_location ();
      flatpak_exports_add_path_tmpfs (exports, flatpak_file_get_path_cached (user_flatpak_dir));

      /* Ensure we always have a homedir */
      flatpak_exports_add_path_dir (exports, g_get_home_dir ());
    }

  return g_steal_pointer (&exports);
}