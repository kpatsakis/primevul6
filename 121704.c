find_matching_ref (GHashTable  *refs,
                   const char  *name,
                   const char  *opt_branch,
                   const char  *opt_default_branch,
                   const char  *opt_arch,
                   FlatpakKinds kinds,
                   GError     **error)
{
  const char **arches = flatpak_get_arches ();
  const char *opt_arches[] = {opt_arch, NULL};
  int i;

  if (opt_arch != NULL)
    arches = opt_arches;

  /* We stop at the first arch (in prio order) that has a match */
  for (i = 0; arches[i] != NULL; i++)
    {
      g_autoptr(GPtrArray) matched_refs = NULL;
      int j;

      matched_refs = find_matching_refs (refs,
                                         name,
                                         opt_branch,
                                         opt_default_branch,
                                         arches[i],
                                         NULL,
                                         kinds,
                                         FIND_MATCHING_REFS_FLAGS_NONE,
                                         error);
      if (matched_refs == NULL)
        return NULL;

      if (matched_refs->len == 0)
        continue;

      if (matched_refs->len == 1)
        return flatpak_decomposed_ref (g_ptr_array_index (matched_refs, 0));

      /* Nothing to do other than reporting the different choices */
      g_autoptr(GString) err = g_string_new ("");
      g_string_printf (err, _("Multiple branches available for %s, you must specify one of: "), name);
      g_ptr_array_sort (matched_refs, flatpak_strcmp0_ptr);
      for (j = 0; j < matched_refs->len; j++)
        {
          FlatpakDecomposed *ref = g_ptr_array_index (matched_refs, j);
          if (j != 0)
            g_string_append (err, ", ");

          const char *branch = flatpak_decomposed_get_branch (ref);

          g_string_append (err,
                           g_strdup_printf ("%s/%s/%s",
                                            name,
                                            opt_arch ? opt_arch : "",
                                            branch));
        }

      flatpak_fail (error, "%s", err->str);
      return NULL;
    }

  g_set_error (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
               _("Nothing matches %s"), name);
  return NULL;
}