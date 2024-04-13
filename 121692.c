flatpak_dir_find_installed_refs (FlatpakDir           *self,
                                 const char           *opt_name,
                                 const char           *opt_branch,
                                 const char           *opt_arch,
                                 FlatpakKinds          kinds,
                                 FindMatchingRefsFlags flags,
                                 GError              **error)
{
  g_autoptr(GHashTable) local_refs = NULL;
  g_autoptr(GPtrArray) matched_refs = NULL;

  local_refs = flatpak_dir_get_all_installed_refs (self, kinds, error);
  if (local_refs == NULL)
    return NULL;

  matched_refs = find_matching_refs (local_refs,
                                     opt_name,
                                     opt_branch,
                                     NULL, /* default branch */
                                     opt_arch,
                                     NULL, /* default arch */
                                     kinds,
                                     flags,
                                     error);
  if (matched_refs == NULL)
    return NULL;

  return g_steal_pointer (&matched_refs);
}