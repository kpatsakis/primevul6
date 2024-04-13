flatpak_export_dir (GFile        *source,
                    GFile        *destination,
                    const char   *symlink_prefix,
                    GCancellable *cancellable,
                    GError      **error)
{
  const char *exported_subdirs[] = {
    "share/applications",                  "../..",
    "share/icons",                         "../..",
    "share/dbus-1/services",               "../../..",
    "share/gnome-shell/search-providers",  "../../..",
    "share/mime/packages",                 "../../..",
    "bin",                                 "..",
  };
  int i;

  for (i = 0; i < G_N_ELEMENTS (exported_subdirs); i = i + 2)
    {
      /* The fds are closed by this call */
      g_autoptr(GFile) sub_source = g_file_resolve_relative_path (source, exported_subdirs[i]);
      g_autoptr(GFile) sub_destination = g_file_resolve_relative_path (destination, exported_subdirs[i]);
      g_autofree char *sub_symlink_prefix = g_build_filename (exported_subdirs[i + 1], symlink_prefix, exported_subdirs[i], NULL);

      if (!g_file_query_exists (sub_source, cancellable))
        continue;

      if (!flatpak_mkdir_p (sub_destination, cancellable, error))
        return FALSE;

      if (!export_dir (AT_FDCWD, flatpak_file_get_path_cached (sub_source), sub_symlink_prefix, "",
                       AT_FDCWD, flatpak_file_get_path_cached (sub_destination),
                       cancellable, error))
        return FALSE;
    }

  return TRUE;
}