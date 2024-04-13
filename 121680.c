local_match_prefix (FlatpakDir        *self,
                    FlatpakDecomposed *extension_ref,
                    const char        *remote)
{
  GHashTable *matches = g_hash_table_new_full ((GHashFunc)flatpak_decomposed_hash, (GEqualFunc)flatpak_decomposed_equal, (GDestroyNotify)flatpak_decomposed_unref, NULL);
  FlatpakKinds kind;
  g_autofree char *id = NULL;
  g_autofree char *arch = NULL;
  g_autofree char *branch = NULL;
  g_autofree char *id_prefix = NULL;
  g_autoptr(GHashTable) refs = NULL;
  g_autofree char *list_prefix = NULL;
  const char *kind_str;

  kind = flatpak_decomposed_get_kinds (extension_ref);
  id = flatpak_decomposed_dup_id (extension_ref);
  arch = flatpak_decomposed_dup_arch (extension_ref);
  branch = flatpak_decomposed_dup_branch (extension_ref);

  id_prefix = g_strconcat (id, ".", NULL);

  kind_str = flatpak_decomposed_get_kind_str (extension_ref);

  if (remote)
    list_prefix = g_strdup_printf ("%s:%s", remote, kind_str);

  if (ostree_repo_list_refs (self->repo, list_prefix, &refs, NULL, NULL))
    {
      GHashTableIter hash_iter;
      gpointer key;

      g_hash_table_iter_init (&hash_iter, refs);
      while (g_hash_table_iter_next (&hash_iter, &key, NULL))
        {
          const char *partial_ref_and_origin = key;
          g_autofree char *partial_ref_store = NULL;
          const char *partial_ref;
          g_autoptr(FlatpakDecomposed) matched = NULL;

          ostree_parse_refspec (partial_ref_and_origin, NULL, &partial_ref_store, NULL);
          if (remote == NULL)
            {
              /* If we're not filtering via list_prefix we need to filter by part[0] manually */
              char *slash = strchr (partial_ref_store, '/');
              if (slash == NULL)
                continue;
              *slash = 0;
              if (strcmp (partial_ref_store, kind_str) != 0)
                continue;
              partial_ref = slash + 1;
            }
          else
            partial_ref = partial_ref_store;

          matched = flatpak_decomposed_new_from_pref (kind, partial_ref, NULL);
          if (matched == NULL)
            continue;

          /* Must match type, arch, branch */
          if (!flatpak_decomposed_is_arch (matched, arch) ||
              !flatpak_decomposed_is_branch (matched, branch))
            continue;

          /* But only prefix of id */
          if (!flatpak_decomposed_id_has_prefix (matched, id_prefix))
            continue;

          g_hash_table_add (matches, g_steal_pointer (&matched));
        }
    }

  /* Also check deploys. In case remote-delete --force is run, we can end up
   * with a deploy without a corresponding ref in the repo. */
  flatpak_dir_collect_deployed_refs (self, kind_str, id_prefix, arch, branch, matches, NULL, NULL);

  return matches;
}