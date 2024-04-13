flatpak_dir_find_remote_related (FlatpakDir         *self,
                                 FlatpakRemoteState *state,
                                 FlatpakDecomposed  *ref,
                                 GCancellable       *cancellable,
                                 GError            **error)
{
  g_autofree char *metadata = NULL;
  g_autoptr(GKeyFile) metakey = g_key_file_new ();
  g_autoptr(GPtrArray) related = g_ptr_array_new_with_free_func ((GDestroyNotify) flatpak_related_free);
  g_autofree char *url = NULL;

  if (!ostree_repo_remote_get_url (self->repo,
                                   state->remote_name,
                                   &url,
                                   error))
    return NULL;

  if (*url == 0)
    return g_steal_pointer (&related);  /* Empty url, silently disables updates */

  if (flatpak_remote_state_load_data (state, flatpak_decomposed_get_ref (ref),
                                      NULL, NULL, &metadata,
                                      NULL) &&
      g_key_file_load_from_data (metakey, metadata, -1, 0, NULL))
    {
      g_ptr_array_unref (related);
      related = flatpak_dir_find_remote_related_for_metadata (self, state, ref, metakey, cancellable, error);
    }

  return g_steal_pointer (&related);
}