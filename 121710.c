flatpak_dir_remote_make_oci_summary (FlatpakDir   *self,
                                     const char   *remote,
                                     gboolean      only_cached,
                                     GBytes      **out_summary,
                                     GCancellable *cancellable,
                                     GError      **error)
{
  g_autoptr(GVariant) summary = NULL;
  g_autoptr(GFile) index_cache = NULL;
  g_autofree char *index_uri = NULL;
  g_autoptr(GFile) summary_cache = NULL;
  g_autofree char *self_name = NULL;
  g_autoptr(GError) local_error = NULL;
  g_autoptr(GMappedFile) mfile = NULL;
  g_autoptr(GBytes) cache_bytes = NULL;
  g_autoptr(GBytes) summary_bytes = NULL;

  if (flatpak_dir_use_system_helper (self, NULL))
    {
      const char *installation = flatpak_dir_get_id (self);
      FlatpakHelperGenerateOciSummaryFlags flags = FLATPAK_HELPER_GENERATE_OCI_SUMMARY_FLAGS_NONE;

      if (only_cached)
        flags |= FLATPAK_HELPER_GENERATE_OCI_SUMMARY_FLAGS_ONLY_CACHED;

      if (!flatpak_dir_system_helper_call_generate_oci_summary (self,
                                                                flags,
                                                                remote,
                                                                installation ? installation : "",
                                                                cancellable, error))
        return FALSE;

      summary_cache = flatpak_dir_get_oci_summary_location (self, remote, error);
      if (summary_cache == NULL)
        return FALSE;
    }
  else
    {
      self_name = flatpak_dir_get_name (self);

      index_cache = flatpak_dir_update_oci_index (self, remote, &index_uri, cancellable, error);
      if (index_cache == NULL)
        return FALSE;

      summary_cache = flatpak_dir_get_oci_summary_location (self, remote, error);
      if (summary_cache == NULL)
        return FALSE;

      if (!only_cached && !check_destination_mtime (index_cache, summary_cache, cancellable))
        {
          summary = flatpak_oci_index_make_summary (index_cache, index_uri, cancellable, &local_error);
          if (summary == NULL)
            {
              g_propagate_error (error, g_steal_pointer (&local_error));
              return FALSE;
            }

          summary_bytes = g_variant_get_data_as_bytes (summary);

          if (!g_file_replace_contents (summary_cache,
                                        g_bytes_get_data (summary_bytes, NULL),
                                        g_bytes_get_size (summary_bytes),
                                        NULL, FALSE, 0, NULL, cancellable, error))
            {
              g_prefix_error (error, _("Failed to write summary cache: "));
              return FALSE;
            }

          if (out_summary)
            *out_summary = g_steal_pointer (&summary_bytes);
          return TRUE;
        }
    }

  if (out_summary)
    {
      mfile = g_mapped_file_new (flatpak_file_get_path_cached (summary_cache), FALSE, error);
      if (mfile == NULL)
        {
          if (only_cached)
            {
              g_clear_error (error);
              g_set_error (error, FLATPAK_ERROR, FLATPAK_ERROR_NOT_CACHED,
                           _("No oci summary cached for remote '%s'"), remote);
            }

          return FALSE;
        }

      cache_bytes = g_mapped_file_get_bytes (mfile);
      *out_summary = g_steal_pointer (&cache_bytes);
    }

  return TRUE;
}