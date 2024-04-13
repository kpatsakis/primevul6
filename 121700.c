find_matching_refs (GHashTable           *refs,
                    const char           *opt_name,
                    const char           *opt_branch,
                    const char           *opt_default_branch,
                    const char           *opt_arch,
                    const char           *opt_default_arch,
                    FlatpakKinds          kinds,
                    FindMatchingRefsFlags flags,
                    GError              **error)
{
  g_autoptr(GPtrArray) matched_refs = NULL;
  const char **arches = flatpak_get_arches ();
  const char *opt_arches[] = {opt_arch, NULL};
  g_autoptr(GError) local_error = NULL;
  gboolean found_exact_name_match = FALSE;
  gboolean found_default_branch_match = FALSE;
  gboolean found_default_arch_match = FALSE;

  if (opt_arch != NULL)
    arches = opt_arches;

  if (opt_name && !(flags & FIND_MATCHING_REFS_FLAGS_FUZZY) &&
      !flatpak_is_valid_name (opt_name, -1, &local_error))
    {
      flatpak_fail_error (error, FLATPAK_ERROR_INVALID_REF, _("'%s' is not a valid name: %s"), opt_name, local_error->message);
      return NULL;
    }

  if (opt_branch && !flatpak_is_valid_branch (opt_branch, -1, &local_error))
    {
      flatpak_fail_error (error, FLATPAK_ERROR_INVALID_REF, _("'%s' is not a valid branch name: %s"), opt_branch, local_error->message);
      return NULL;
    }

  matched_refs = g_ptr_array_new_with_free_func ((GDestroyNotify)flatpak_decomposed_unref);

  GLNX_HASH_TABLE_FOREACH (refs, FlatpakDecomposed *, ref)
    {
      if ((flatpak_decomposed_get_kinds (ref) & kinds) == 0)
        continue;

      if (opt_name)
        {
          if ((flags & FIND_MATCHING_REFS_FLAGS_FUZZY) && !flatpak_decomposed_id_is_subref (ref))
            {
              if (!flatpak_decomposed_is_id_fuzzy (ref, opt_name))
                continue;
            }
          else
            {
              if (!flatpak_decomposed_is_id (ref, opt_name))
                continue;
           }
        }

      if (!flatpak_decomposed_is_arches (ref, -1, arches))
        continue;

      if (opt_branch != NULL && !flatpak_decomposed_is_branch (ref, opt_branch))
        continue;

      if (opt_name != NULL && flatpak_decomposed_is_id (ref, opt_name))
        found_exact_name_match = TRUE;

      if (opt_default_arch != NULL && flatpak_decomposed_is_arch (ref, opt_default_arch))
        found_default_arch_match = TRUE;

      if (opt_default_branch != NULL && flatpak_decomposed_is_branch (ref, opt_default_branch))
        found_default_branch_match = TRUE;

      g_ptr_array_add (matched_refs, flatpak_decomposed_ref (ref));
    }

  /* Don't show fuzzy matches if we found at least one exact name match, and
   * enforce the default arch/branch */
  if (found_exact_name_match || found_default_arch_match || found_default_branch_match)
    {
      guint i;

      /* Walk through the array backwards so we can safely remove */
      for (i = matched_refs->len; i > 0; i--)
        {
          FlatpakDecomposed *matched_ref = g_ptr_array_index (matched_refs, i - 1);

          if (found_exact_name_match && !flatpak_decomposed_is_id (matched_ref, opt_name))
            g_ptr_array_remove_index (matched_refs, i - 1);
          else if (found_default_arch_match && !flatpak_decomposed_is_arch (matched_ref, opt_default_arch))
            g_ptr_array_remove_index (matched_refs, i - 1);
          else if (found_default_branch_match && !flatpak_decomposed_is_branch (matched_ref, opt_default_branch))
            g_ptr_array_remove_index (matched_refs, i - 1);
        }
    }

  return g_steal_pointer (&matched_refs);
}