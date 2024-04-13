flatpak_dir_remote_load_cached_summary (FlatpakDir   *self,
                                        const char   *basename,
                                        const char   *main_ext,
                                        const char   *sig_ext,
                                        GBytes      **out_main,
                                        GBytes      **out_sig,
                                        GCancellable *cancellable,
                                        GError      **error)
{
  g_autofree char *main_file_name = g_strconcat (basename, main_ext, NULL);
  g_autofree char *sig_file_name = g_strconcat (basename, sig_ext, NULL);
  g_autoptr(GFile) main_cache_file = flatpak_build_file (self->cache_dir, "summaries", main_file_name, NULL);
  g_autoptr(GFile) sig_cache_file = flatpak_build_file (self->cache_dir, "summaries", sig_file_name, NULL);
  g_autoptr(GMappedFile) mfile = NULL;
  g_autoptr(GMappedFile) sig_mfile = NULL;

  mfile = g_mapped_file_new (flatpak_file_get_path_cached (main_cache_file), FALSE, NULL);
  if (mfile == NULL)
    {
      g_set_error (error, FLATPAK_ERROR, FLATPAK_ERROR_NOT_CACHED,
                   _("No cached summary for remote '%s'"), basename);
      return FALSE;
    }

  if (out_sig)
    sig_mfile = g_mapped_file_new (flatpak_file_get_path_cached (sig_cache_file), FALSE, NULL);

  *out_main = g_mapped_file_get_bytes (mfile);
  if (sig_mfile)
    *out_sig = g_mapped_file_get_bytes (sig_mfile);

  return TRUE;
}