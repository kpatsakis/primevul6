flatpak_dir_find_remote_refs (FlatpakDir           *self,
                              FlatpakRemoteState   *state,
                              const char           *name,
                              const char           *opt_branch,
                              const char           *opt_default_branch,
                              const char           *opt_arch,
                              const char           *opt_default_arch,
                              FlatpakKinds          kinds,
                              FindMatchingRefsFlags flags,
                              GCancellable         *cancellable,
                              GError              **error)
{
  g_autoptr(GHashTable) remote_refs = NULL;
  g_autoptr(GPtrArray) matched_refs = NULL;

  if (!flatpak_dir_list_all_remote_refs (self, state,
                                         &remote_refs, cancellable, error))
    return NULL;


  matched_refs = find_matching_refs (remote_refs,
                                     name,
                                     opt_branch,
                                     opt_default_branch,
                                     opt_arch,
                                     opt_default_arch,
                                     kinds,
                                     flags,
                                     error);
  if (matched_refs == NULL)
    return NULL;

  /* If we can't match anything and we had an error downloading (offline?), report that as its more helpful */
  if (matched_refs->len == 0 && state->summary_fetch_error)
    {
      g_propagate_error (error, g_error_copy (state->summary_fetch_error));
      return NULL;
    }

  return g_steal_pointer (&matched_refs);
}