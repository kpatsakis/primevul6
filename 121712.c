_flatpak_dir_get_remote_state (FlatpakDir   *self,
                               const char   *remote_or_uri,
                               gboolean      optional,
                               gboolean      local_only,
                               gboolean      only_cached,
                               gboolean      opt_summary_is_index,
                               GBytes       *opt_summary,
                               GBytes       *opt_summary_sig,
                               GCancellable *cancellable,
                               GError      **error)
{
  g_autoptr(FlatpakRemoteState) state = flatpak_remote_state_new ();
  g_autoptr(GPtrArray) sideload_paths = NULL;
  g_autofree char *url = NULL;
  g_autoptr(GError) my_error = NULL;
  gboolean is_local;
  gboolean got_summary = FALSE;
  const char *arch = flatpak_get_default_arch ();
  g_autoptr(GBytes) index_bytes = NULL;
  g_autoptr(GBytes) index_sig_bytes = NULL;
  g_autoptr(GBytes) summary_bytes = NULL;
  g_autoptr(GBytes) summary_sig_bytes = NULL;

  if (error == NULL)
    error = &my_error;

  if (!flatpak_dir_ensure_repo (self, cancellable, error))
    return NULL;

  state->remote_name = g_strdup (remote_or_uri);
  state->is_file_uri = is_local = g_str_has_prefix (remote_or_uri, "file:");
  if (!is_local)
    {
      if (!flatpak_dir_has_remote (self, remote_or_uri, error))
        return NULL;
      if (!repo_get_remote_collection_id (self->repo, remote_or_uri, &state->collection_id, error))
        return NULL;
      if (!flatpak_dir_lookup_remote_filter (self, remote_or_uri, FALSE, NULL, &state->allow_refs, &state->deny_refs, error))
        return NULL;
      if (!ostree_repo_remote_get_url (self->repo, remote_or_uri, &url, error))
        return FALSE;

      state->default_token_type = flatpak_dir_get_remote_default_token_type (self, remote_or_uri);
    }

  sideload_paths = flatpak_dir_get_sideload_repo_paths (self);
  for (int i = 0; i < sideload_paths->len; i++)
    flatpak_remote_state_add_sideload_repo (state, g_ptr_array_index (sideload_paths, i));

  if (local_only)
    {
      flatpak_fail (&state->summary_fetch_error, "Internal error, local_only state");
      return g_steal_pointer (&state);
    }

  if (opt_summary)
    {
      if (opt_summary_sig)
        {
          /* If specified, must be valid signature */
          g_autoptr(OstreeGpgVerifyResult) gpg_result =
            ostree_repo_verify_summary (self->repo,
                                        state->remote_name,
                                        opt_summary,
                                        opt_summary_sig,
                                        NULL, error);
          if (gpg_result == NULL ||
              !ostree_gpg_verify_result_require_valid_signature (gpg_result, error))
            return NULL;
        }

      if (opt_summary_is_index)
        {
          if (opt_summary_sig)
            index_sig_bytes = g_bytes_ref (opt_summary_sig);
          index_bytes = g_bytes_ref (opt_summary);
        }
      else
        {
          if (opt_summary_sig)
            summary_sig_bytes = g_bytes_ref (opt_summary_sig);
          summary_bytes = g_bytes_ref (opt_summary);
        }

      got_summary = TRUE;
    }

  /* First try the memory cache. Note: No in-memory caching for local files. */
  if (!is_local)
    {
      if (!got_summary)
        {
          g_autofree char *index_cache_key = g_strconcat ("index-", remote_or_uri, NULL);
          if (flatpak_dir_lookup_cached_summary (self, &index_bytes, &index_sig_bytes, index_cache_key, url))
            got_summary = TRUE;
        }

      if (!got_summary)
        {
          g_autofree char *summary_cache_key = g_strconcat ("summary-", remote_or_uri, NULL);
          if (flatpak_dir_lookup_cached_summary (self, &summary_bytes, &summary_sig_bytes, summary_cache_key, url))
            got_summary = TRUE;
        }
    }

  /* Then look for an indexed summary on disk/network */
  if (!got_summary)
    {
      g_autoptr(GError) local_error = NULL;

      if (flatpak_dir_remote_fetch_summary_index (self, remote_or_uri, only_cached, &index_bytes, &index_sig_bytes,
                                                  cancellable, &local_error))
        {
          got_summary = TRUE;
        }
      else
        {
          if (!g_error_matches (local_error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND) &&
              !g_error_matches (local_error, FLATPAK_ERROR, FLATPAK_ERROR_NOT_CACHED))
            {
              /* We got an error other than not-found, assume we're indexed but there is some network error */
              got_summary = TRUE;
              if (optional && !g_cancellable_is_cancelled (cancellable))
                {
                  g_debug ("Failed to download optional summary index: %s", local_error->message);
                  state->summary_fetch_error = g_steal_pointer (&local_error);
                }
              else
                {
                  g_propagate_error (error, g_steal_pointer (&local_error));
                  return NULL;
                }
            }
        }
    }

  if (!got_summary)
    {
      /* No index, fall back to full summary */
      g_autoptr(GError) local_error = NULL;

      if (flatpak_dir_remote_fetch_summary (self, remote_or_uri, only_cached, &summary_bytes, &summary_sig_bytes,
                                            cancellable, &local_error))
        {
          got_summary = TRUE;
        }
      else
        {
          if (optional && !g_cancellable_is_cancelled (cancellable))
            {
              g_debug ("Failed to download optional summary: %s", local_error->message);
              state->summary_fetch_error = g_steal_pointer (&local_error);
            }
          else
            {
              g_propagate_error (error, g_steal_pointer (&local_error));
              return NULL;
            }
        }
    }

  if (index_bytes)
    {
      state->index = g_variant_ref_sink (g_variant_new_from_bytes (FLATPAK_SUMMARY_INDEX_GVARIANT_FORMAT,
                                                                   index_bytes, FALSE));
      state->index_sig_bytes = g_steal_pointer (&index_sig_bytes);
    }
  else if (summary_bytes)
    {
      state->summary = g_variant_ref_sink (g_variant_new_from_bytes (OSTREE_SUMMARY_GVARIANT_FORMAT,
                                                                     summary_bytes, FALSE));
      state->summary_bytes = g_steal_pointer (&summary_bytes);
      state->summary_sig_bytes = g_steal_pointer (&summary_sig_bytes);
    }

  if (state->index)
    {
      g_autofree char *require_subset = flatpak_dir_get_remote_subset (self, state->remote_name);
      VarSummaryIndexRef index = var_summary_index_from_gvariant (state->index);
      VarSummaryIndexSubsummariesRef subsummaries = var_summary_index_get_subsummaries (index);
      gsize n_subsummaries = var_summary_index_subsummaries_get_length (subsummaries);

      state->index_ht = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, (GDestroyNotify)g_variant_unref);

      for (gsize i = 0; i < n_subsummaries; i++)
        {
          VarSummaryIndexSubsummariesEntryRef entry = var_summary_index_subsummaries_get_at (subsummaries, i);
          const char *name = var_summary_index_subsummaries_entry_get_key (entry);
          VarSubsummaryRef subsummary = var_summary_index_subsummaries_entry_get_value (entry);
          gsize checksum_bytes_len;
          const guchar *checksum_bytes;
          g_autofree char *digest = NULL;
          const char *dash, *subsummary_arch;

          dash = strchr (name, '-');
          subsummary_arch = dash == NULL ? name : dash + 1;

          if (dash == NULL) /* No subset */
            {
              if (require_subset != NULL)
                continue;
            }
          else /* Subset */
            {
              if (require_subset == NULL)
                continue;
              else
                {
                  g_autofree char *subset = g_strndup (name, dash - name);
                  if (strcmp (require_subset, subset) != 0)
                    continue;
                }
            }

          checksum_bytes = var_subsummary_peek_checksum (subsummary, &checksum_bytes_len);
          if (G_UNLIKELY (checksum_bytes_len != OSTREE_SHA256_DIGEST_LEN))
            {
              g_debug ("Invalid checksum for digested summary, not using cache");
              continue;
            }
          digest = ostree_checksum_from_bytes (checksum_bytes);

          g_hash_table_insert (state->index_ht, g_strdup (subsummary_arch), var_subsummary_to_owned_gvariant (subsummary, state->index));
        }

      /* Always load default (or specified) arch subsummary. Further arches can be manually loaded with flatpak_remote_state_ensure_subsummary. */
      if (opt_summary == NULL &&
          !flatpak_remote_state_ensure_subsummary (state, self, arch, only_cached, cancellable, error))
        return NULL;
    }

  if (state->collection_id != NULL &&
      state->summary != NULL &&
      !_validate_summary_for_collection_id (state->summary, state->collection_id, error))
    return NULL;

  if (flatpak_dir_get_remote_oci (self, remote_or_uri))
    {
      state->default_token_type = 1;
    }

  if (state->summary != NULL || state->index != NULL) /* In the optional case we might not have a summary */
    {
      VarMetadataRef meta = flatpak_remote_state_get_main_metadata (state);
      VarVariantRef res;

      if (var_metadata_lookup (meta, "xa.default-token-type", NULL, &res) &&
          var_variant_is_type (res, G_VARIANT_TYPE_INT32))
        state->default_token_type = GINT32_FROM_LE (var_variant_get_int32 (res));
    }

  return g_steal_pointer (&state);
}