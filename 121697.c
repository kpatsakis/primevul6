flatpak_dir_remote_fetch_summary_index (FlatpakDir   *self,
                                        const char   *name_or_uri,
                                        gboolean      only_cached,
                                        GBytes      **out_index,
                                        GBytes      **out_index_sig,
                                        GCancellable *cancellable,
                                        GError      **error)
{
  g_autofree char *url = NULL;
  gboolean is_local;
  g_autoptr(GError) local_error = NULL;
  g_autoptr(GError) cache_error = NULL;
  g_autoptr(GBytes) cached_index = NULL;
  g_autoptr(GBytes) cached_index_sig = NULL;
  g_autoptr(GBytes) index = NULL;
  g_autoptr(GBytes) index_sig = NULL;
  gboolean gpg_verify_summary;

  ensure_soup_session (self);

  if (!ostree_repo_remote_get_url (self->repo, name_or_uri, &url, error))
    return FALSE;

  if (!ostree_repo_remote_get_gpg_verify_summary (self->repo, name_or_uri, &gpg_verify_summary, NULL))
    return FALSE;

  if (!g_str_has_prefix (name_or_uri, "file:") && flatpak_dir_get_remote_disabled (self, name_or_uri))
    {
      g_set_error (error, G_IO_ERROR, G_IO_ERROR_INVALID_DATA,
                   "Can't fetch summary from disabled remote ‘%s’", name_or_uri);
      return FALSE;
    }

  if (flatpak_dir_get_remote_oci (self, name_or_uri))
    {
      g_set_error (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
                   "No index in OCI remote ‘%s’", name_or_uri);
      return FALSE;
    }

  is_local = g_str_has_prefix (url, "file:");

  /* Seems ostree asserts if this is NULL */
  if (error == NULL)
    error = &local_error;

  flatpak_dir_remote_load_cached_summary (self, name_or_uri, ".idx", ".idx.sig",
                                          &cached_index, &cached_index_sig, cancellable, &cache_error);

  if (only_cached)
    {
      if (cached_index == NULL)
        {
          g_propagate_error (error, g_steal_pointer (&cache_error));
          return FALSE;
        }
      g_debug ("Loaded summary index from cache for remote ‘%s’", name_or_uri);

      index = g_steal_pointer (&cached_index);
      if (gpg_verify_summary)
        index_sig = g_steal_pointer (&cached_index_sig);
    }
  else
    {
      g_autofree char *index_url = g_build_filename (url, "summary.idx", NULL);
      g_autoptr(GBytes) dl_index = NULL;
      gboolean used_download = FALSE;

      g_debug ("Fetching summary index file for remote ‘%s’", name_or_uri);

      dl_index = flatpak_load_uri (self->soup_session, index_url, 0, NULL,
                                   NULL, NULL, NULL,
                                   cancellable, error);
      if (dl_index == NULL)
        return FALSE;

      /* If the downloaded index is the same as the cached one we need not re-download or
       * re-verify, just use the cache (which we verified before) */
      if (cached_index != NULL && g_bytes_equal (cached_index, dl_index))
        {
          index = g_steal_pointer (&cached_index);
          if (gpg_verify_summary)
            index_sig = g_steal_pointer (&cached_index_sig);
        }
      else
        {
          index = g_steal_pointer (&dl_index);
          used_download = TRUE;
        }

      if (gpg_verify_summary && index_sig == NULL)
        {
          g_autofree char *index_digest = g_compute_checksum_for_bytes (G_CHECKSUM_SHA256, index);
          g_autofree char *index_sig_filename = g_strconcat (index_digest, ".idx.sig", NULL);
          g_autofree char *index_sig_url = g_build_filename (url, "summaries", index_sig_filename, NULL);
          g_autofree char *index_sig_url2 = g_build_filename (url, "summary.idx.sig", NULL);
          g_autoptr(GError) dl_sig_error = NULL;
          g_autoptr (GBytes) dl_index_sig = NULL;

          dl_index_sig = load_uri_with_fallback (self->soup_session, index_sig_url, index_sig_url2, 0, NULL,
                                                 cancellable, &dl_sig_error);
          if (dl_index_sig == NULL)
            {
              if (g_error_matches (dl_sig_error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
                g_set_error (error, OSTREE_GPG_ERROR, OSTREE_GPG_ERROR_NO_SIGNATURE,
                             "GPG verification enabled, but no summary signatures found (use gpg-verify-summary=false in remote config to disable)");
              else
                g_propagate_error (error, g_steal_pointer (&dl_sig_error));

              return FALSE;
            }

          if (!remote_verify_signature (self->repo, name_or_uri,
                                        index, dl_index_sig,
                                        cancellable, error))
            return FALSE;

          index_sig = g_steal_pointer (&dl_index_sig);
          used_download = TRUE;
        }

      g_assert (index != NULL);
      if (gpg_verify_summary)
        g_assert (index_sig != NULL);

      /* Update cache on disk if we downloaded anything, but never cache for file: repos */
      if (used_download && !is_local &&
          !flatpak_dir_remote_save_cached_summary (self, name_or_uri, ".idx", ".idx.sig",
                                                   index, index_sig, cancellable, error))
        return FALSE;
    }

  /* Cache in memory */
  if (!is_local && !only_cached)
    {
      g_autofree char *cache_key = g_strconcat ("index-", name_or_uri, NULL);
      flatpak_dir_cache_summary (self, index, index_sig, cache_key, url);
    }

  *out_index = g_steal_pointer (&index);
  if (out_index_sig)
    *out_index_sig = g_steal_pointer (&index_sig);

  return TRUE;
}