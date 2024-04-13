flatpak_dir_list_remote_refs (FlatpakDir         *self,
                              FlatpakRemoteState *state,
                              GHashTable        **refs,
                              GCancellable       *cancellable,
                              GError            **error)
{
  g_autoptr(GError) my_error = NULL;

  if (error == NULL)
    error = &my_error;

  if (!flatpak_dir_list_all_remote_refs (self, state, refs,
                                         cancellable, error))
    return FALSE;

  if (flatpak_dir_get_remote_noenumerate (self, state->remote_name))
    {
      g_autoptr(GHashTable) decomposed_local_refs =
        g_hash_table_new_full ((GHashFunc)flatpak_decomposed_hash, (GEqualFunc)flatpak_decomposed_equal, (GDestroyNotify)flatpak_decomposed_unref, NULL);
      g_autoptr(GHashTable) local_refs = NULL;
      GHashTableIter hash_iter;
      gpointer key;
      g_autofree char *refspec_prefix = g_strconcat (state->remote_name, ":.", NULL);

      /* For noenumerate remotes, only return data for already locally
       * available refs */

      if (!ostree_repo_list_refs (self->repo, refspec_prefix, &local_refs,
                                  cancellable, error))
        return FALSE;

      g_hash_table_iter_init (&hash_iter, local_refs);
      while (g_hash_table_iter_next (&hash_iter, &key, NULL))
        {
          const char *refspec = key;
          g_autoptr(FlatpakDecomposed) d = flatpak_decomposed_new_from_refspec (refspec, NULL);
          if (d)
            g_hash_table_insert (decomposed_local_refs, g_steal_pointer (&d), NULL);
        }

      /* Then we remove all remote refs not in the local refs set */
      g_hash_table_foreach_remove (*refs,
                                   remove_unless_decomposed_in_hash,
                                   decomposed_local_refs);
    }

  return TRUE;
}