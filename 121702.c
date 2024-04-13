flatpak_dir_remote_fetch_summary (FlatpakDir   *self,
                                  const char   *name_or_uri,
                                  gboolean      only_cached,
                                  GBytes      **out_summary,
                                  GBytes      **out_summary_sig,
                                  GCancellable *cancellable,
                                  GError      **error)
{
  g_autofree char *url = NULL;
  gboolean is_local;
  g_autoptr(GError) local_error = NULL;
  g_autoptr(GBytes) summary = NULL;
  g_autoptr(GBytes) summary_sig = NULL;

  if (!ostree_repo_remote_get_url (self->repo, name_or_uri, &url, error))
    return FALSE;

  if (!g_str_has_prefix (name_or_uri, "file:") && flatpak_dir_get_remote_disabled (self, name_or_uri))
    {
      g_set_error (error, G_IO_ERROR, G_IO_ERROR_INVALID_DATA,
                   "Can't fetch summary from disabled remote ‘%s’", name_or_uri);
      return FALSE;
    }

  is_local = g_str_has_prefix (url, "file:");

  /* Seems ostree asserts if this is NULL */
  if (error == NULL)
    error = &local_error;

  if (flatpak_dir_get_remote_oci (self, name_or_uri))
    {
      if (!flatpak_dir_remote_make_oci_summary (self, name_or_uri,
                                                only_cached,
                                                &summary,
                                                cancellable,
                                                error))
        return FALSE;
    }
  else
    {
      if (only_cached)
        {
          if (!flatpak_dir_remote_load_cached_summary (self, name_or_uri, NULL, ".sig",
                                                       &summary, &summary_sig, cancellable, error))
            return FALSE;
          g_debug ("Loaded summary from cache for remote ‘%s’", name_or_uri);
        }
      else
        {
          g_debug ("Fetching summary file for remote ‘%s’", name_or_uri);
          if (!ostree_repo_remote_fetch_summary (self->repo, name_or_uri,
                                                 &summary, &summary_sig,
                                                 cancellable,
                                                 error))
            return FALSE;
        }
    }

  if (summary == NULL)
    return flatpak_fail_error (error, FLATPAK_ERROR_INVALID_DATA, _("Remote listing for %s not available; server has no summary file. Check the URL passed to remote-add was valid."), name_or_uri);

  if (!is_local && !only_cached)
    {
      g_autofree char *cache_key = g_strconcat ("summary-", name_or_uri, NULL);
      flatpak_dir_cache_summary (self, summary, summary_sig, cache_key, url);
    }

  *out_summary = g_steal_pointer (&summary);
  if (out_summary_sig)
    *out_summary_sig = g_steal_pointer (&summary_sig);

  return TRUE;
}