flatpak_dir_find_local_refs (FlatpakDir           *self,
                             const char           *remote,
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
  g_autoptr(GHashTable) local_refs = NULL;
  g_autoptr(GHashTable) local_refspecs = NULL;
  g_autoptr(GError) my_error = NULL;
  g_autofree char *refspec_prefix = g_strconcat (remote, ":.", NULL);
  g_autoptr(GPtrArray) matched_refs = NULL;

  if (!flatpak_dir_ensure_repo (self, NULL, error))
    return NULL;

  if (!ostree_repo_list_refs (self->repo,
                              refspec_prefix,
                              &local_refspecs, cancellable, error))
    return NULL;

  local_refs = refspecs_decompose_steal (local_refspecs);

  matched_refs = find_matching_refs (local_refs,
                                     name,
                                     opt_branch,
                                     opt_default_branch,
                                     opt_arch,
                                     opt_default_arch,
                                     kinds,
                                     flags,
                                     &my_error);
  if (matched_refs == NULL)
    {
      if (g_error_matches (my_error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
        {
          g_set_error (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
                       _("Error searching local repository: %s"),
                       my_error->message);
          return NULL;
        }
      else
        {
          g_propagate_error (error, g_steal_pointer (&my_error));
          return NULL;
        }
    }

  return g_steal_pointer (&matched_refs);
}