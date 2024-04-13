flatpak_dir_remote_fetch_indexed_summary (FlatpakDir   *self,
                                          const char   *name_or_uri,
                                          const char   *arch,
                                          GVariant     *subsummary_info_v,
                                          gboolean      only_cached,
                                          GBytes      **out_summary,
                                          GCancellable *cancellable,
                                          GError      **error)
{
  g_autofree char *url = NULL;
  gboolean is_local;
  g_autoptr(GError) cache_error = NULL;
  g_autoptr(GBytes) summary_z = NULL;
  g_autoptr(GBytes) summary = NULL;
  g_autofree char *sha256 = NULL;
  VarSubsummaryRef subsummary_info;
  gsize checksum_bytes_len;
  const guchar *checksum_bytes;
  g_autofree char *checksum = NULL;
  g_autofree char *cache_name = NULL;

  ensure_soup_session (self);

  if (!ostree_repo_remote_get_url (self->repo, name_or_uri, &url, error))
    return FALSE;

  if (!g_str_has_prefix (name_or_uri, "file:") && flatpak_dir_get_remote_disabled (self, name_or_uri))
    {
      g_set_error (error, G_IO_ERROR, G_IO_ERROR_INVALID_DATA,
                   "Can't fetch summary from disabled remote ‘%s’", name_or_uri);
      return FALSE;
    }

  subsummary_info = var_subsummary_from_gvariant (subsummary_info_v);
  checksum_bytes = var_subsummary_peek_checksum (subsummary_info, &checksum_bytes_len);
  g_assert (checksum_bytes_len == OSTREE_SHA256_DIGEST_LEN); /* We verified this when scanning index */
  checksum = ostree_checksum_from_bytes (checksum_bytes);

  is_local = g_str_has_prefix (url, "file:");

  /* No in-memory caching for local files */
  if (!is_local)
    {
      if (flatpak_dir_lookup_cached_summary (self, out_summary, NULL, checksum, url))
        return TRUE;
    }

  cache_name = g_strconcat (name_or_uri, "-", arch, "-", checksum, NULL);

  /* First look for an on-disk cache */
  if (!flatpak_dir_remote_load_cached_summary (self, cache_name, ".sub", NULL,
                                               &summary, NULL, cancellable, &cache_error))
    {
      g_autofree char *old_checksum = NULL;
      g_autoptr(GBytes) old_summary = NULL;

      /* Else fetch it */
      if (only_cached)
        {
          g_propagate_error (error, g_steal_pointer (&cache_error));
          return FALSE;
        }

      /* Look for first applicable deltas */
      VarArrayofChecksumRef history = var_subsummary_get_history (subsummary_info);
      gsize history_len = var_arrayof_checksum_get_length (history);
      for (gsize i = 0; i < history_len; i++)
        {
          VarChecksumRef old = var_arrayof_checksum_get_at (history, i);
          g_autofree char *old_cache_name = NULL;

          if (var_checksum_get_length (old) != OSTREE_SHA256_DIGEST_LEN)
            continue;

          old_checksum = ostree_checksum_from_bytes (var_checksum_peek (old));
          old_cache_name = g_strconcat (name_or_uri, "-", arch, "-", old_checksum, NULL);
          if (flatpak_dir_remote_load_cached_summary (self, old_cache_name, ".sub", NULL,
                                                      &old_summary, NULL, cancellable, NULL))
            break;
        }

      if (old_summary)
        {
          g_autoptr(GError) delta_error = NULL;

          g_autofree char *delta_filename = g_strconcat (old_checksum, "-", checksum, ".delta", NULL);
          g_autofree char *delta_url = g_build_filename (url, "summaries", delta_filename, NULL);

          g_debug ("Fetching indexed summary delta %s for remote ‘%s’", delta_filename, name_or_uri);

          g_autoptr(GBytes) delta = flatpak_load_uri (self->soup_session, delta_url, 0, NULL,
                                                      NULL, NULL, NULL,
                                                      cancellable, &delta_error);
          if (delta == NULL)
            g_debug ("Failed to load delta, falling back: %s", delta_error->message);
          else
            {
              g_autoptr(GBytes) applied = flatpak_summary_apply_diff (old_summary, delta, &delta_error);

              if (applied == NULL)
                g_warning ("Failed to apply delta, falling back: %s", delta_error->message);
              else
                {
                  sha256 = g_compute_checksum_for_bytes (G_CHECKSUM_SHA256, applied);
                  if (strcmp (sha256, checksum) != 0)
                    g_warning ("Appliying delta gave wrong checksum, falling backn");
                  else
                    summary = g_steal_pointer (&applied);
                }
            }
        }

      if (summary == NULL)
        {
          g_autofree char *filename = g_strconcat (checksum, ".gz", NULL);
          g_debug ("Fetching indexed summary file %s for remote ‘%s’", filename, name_or_uri);
          g_autofree char *subsummary_url = g_build_filename (url, "summaries", filename, NULL);
          summary_z = flatpak_load_uri (self->soup_session, subsummary_url, 0, NULL,
                                        NULL, NULL, NULL,
                                        cancellable, error);
          if (summary_z == NULL)
            return FALSE;

          summary = flatpak_zlib_decompress_bytes (summary_z, error);
          if (summary == NULL)
            return FALSE;

          sha256 = g_compute_checksum_for_bytes (G_CHECKSUM_SHA256, summary);
          if (strcmp (sha256, checksum) != 0)
            return flatpak_fail_error (error, FLATPAK_ERROR_INVALID_DATA, _("Invalid checksum for indexed summary %s for remote '%s'"), checksum, name_or_uri);
        }

      /* Save to disk */
      if (!is_local)
        {
          if (!flatpak_dir_remote_save_cached_summary (self, cache_name, ".sub", NULL,
                                                       summary, NULL,
                                                       cancellable, error))
            return FALSE;

          if (!flatpak_dir_gc_cached_digested_summaries (self, name_or_uri, cache_name,
                                                         cancellable, error))
            return FALSE;
        }
    }
  else
    g_debug ("Loaded indexed summary file %s from cache for remote ‘%s’", checksum, name_or_uri);

  /* Cache in memory */
  if (!is_local && !only_cached)
    flatpak_dir_cache_summary (self, summary, NULL, checksum, url);

  *out_summary = g_steal_pointer (&summary);

  return TRUE;
}