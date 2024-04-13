find_ref_for_refs_set (GHashTable   *refs,
                       const char   *name,
                       const char   *opt_branch,
                       const char   *opt_default_branch,
                       const char   *opt_arch,
                       FlatpakKinds  kinds,
                       GError      **error)
{
  g_autoptr(GError) my_error = NULL;
  g_autoptr(FlatpakDecomposed) ref = find_matching_ref (refs,
                                                        name,
                                                        opt_branch,
                                                        opt_default_branch,
                                                        opt_arch,
                                                        kinds,
                                                        &my_error);
  if (ref == NULL)
    {
      if (g_error_matches (my_error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
        g_clear_error (&my_error);
      else
        {
          g_propagate_error (error, g_steal_pointer (&my_error));
          return NULL;
        }
    }
  else
    {
      return g_steal_pointer (&ref);
    }

  g_set_error (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
               _("Can't find ref %s%s%s%s%s"), name,
               (opt_arch != NULL || opt_branch != NULL) ? "/" : "",
               opt_arch ? opt_arch : "",
               opt_branch ? "/" : "",
               opt_branch ? opt_branch : "");

  return NULL;
}