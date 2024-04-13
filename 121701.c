flatpak_dir_list_all_remote_refs (FlatpakDir         *self,
                                  FlatpakRemoteState *state,
                                  GHashTable        **out_all_refs,
                                  GCancellable       *cancellable,
                                  GError            **error)
{
  g_autoptr(GHashTable) ret_all_refs = NULL;
  VarSummaryRef summary;
  VarMetadataRef exts;
  VarRefMapRef ref_map;
  VarVariantRef v;

  /* This is  ref->commit */
  ret_all_refs = g_hash_table_new_full ((GHashFunc)flatpak_decomposed_hash, (GEqualFunc)flatpak_decomposed_equal, (GDestroyNotify)flatpak_decomposed_unref, g_free);

  if (state->index != NULL)
    {
      /* We're online, so report only the refs from the summary */
      GLNX_HASH_TABLE_FOREACH_KV (state->subsummaries, const char *, arch, GVariant *, subsummary)
        {
          summary = var_summary_from_gvariant (subsummary);
          ref_map = var_summary_get_ref_map (summary);
          populate_hash_table_from_refs_map (ret_all_refs, NULL, ref_map, NULL, state);
        }
    }
  else if (state->summary != NULL)
    {
      /* We're online, so report only the refs from the summary */
      const char *main_collection_id = NULL;

      summary = var_summary_from_gvariant (state->summary);

      exts = var_summary_get_metadata (summary);

      if (state->is_file_uri)
        {
          /* This is a local repo, generally this means we gave a file: uri to a sideload repo so
           * we can enumerate it. We special case this by also adding all the collection_ref maps,
           * with collection_id set on the decomposed refs and setting the right collection id for
           * the main ref_map.
           */
          main_collection_id = var_metadata_lookup_string (exts, "ostree.summary.collection-id", NULL);
          if (var_metadata_lookup (exts, "ostree.summary.collection-map", NULL, &v))
            {
              VarCollectionMapRef map = var_collection_map_from_variant (v);

              gsize len = var_collection_map_get_length (map);
              for (gsize i = 0; i < len; i++)
                {
                  VarCollectionMapEntryRef entry = var_collection_map_get_at (map, i);
                  const char *collection_id = var_collection_map_entry_get_key (entry);
                  ref_map = var_collection_map_entry_get_value (entry);

                  populate_hash_table_from_refs_map (ret_all_refs, NULL, ref_map, collection_id, state);
                }
            }
        }

      /* refs that match the main collection-id,
         NOTE: We only set collection id if this is a file: uri remote */
      ref_map = var_summary_get_ref_map (summary);
      populate_hash_table_from_refs_map (ret_all_refs, NULL, ref_map, main_collection_id, state);
    }
  else if (state->collection_id)
    {
      g_autoptr(GHashTable) ref_mtimes = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

      /* No main summary, add just all sideloded refs, with the latest version of each checksum */

      for (int i = 0; i < state->sideload_repos->len; i++)
        {
          FlatpakSideloadState *ss = g_ptr_array_index (state->sideload_repos, i);

          summary = var_summary_from_gvariant (ss->summary);
          exts = var_summary_get_metadata (summary);

          if (var_metadata_lookup (exts, "ostree.summary.collection-map", NULL, &v))
            {
              VarCollectionMapRef map = var_collection_map_from_variant (v);

              if (var_collection_map_lookup (map, state->collection_id, NULL, &ref_map))
                populate_hash_table_from_refs_map (ret_all_refs, ref_mtimes, ref_map, NULL, state);
            }
        }
    }

  /* If no sideloaded refs, might as well return the summary error if set */
  if (g_hash_table_size (ret_all_refs) == 0 &&
      !flatpak_remote_state_ensure_summary (state, error))
    return FALSE;

  *out_all_refs = g_steal_pointer (&ret_all_refs);

  return TRUE;
}