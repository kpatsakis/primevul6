flatpak_dir_find_remote_related_for_metadata (FlatpakDir         *self,
                                              FlatpakRemoteState *state,
                                              FlatpakDecomposed  *ref,
                                              GKeyFile           *metakey,
                                              GCancellable       *cancellable,
                                              GError            **error)
{
  int i;
  g_autoptr(GPtrArray) related = g_ptr_array_new_with_free_func ((GDestroyNotify) flatpak_related_free);
  g_autofree char *url = NULL;
  g_auto(GStrv) groups = NULL;
  g_autoptr(GRegex) masked = NULL;
  g_autofree char *ref_arch = flatpak_decomposed_dup_arch (ref);
  g_autofree char *ref_branch = flatpak_decomposed_dup_branch (ref);

  if (!ostree_repo_remote_get_url (self->repo,
                                   state->remote_name,
                                   &url,
                                   error))
    return NULL;

  if (*url == 0)
    return g_steal_pointer (&related);  /* Empty url, silently disables updates */

  masked = flatpak_dir_get_mask_regexp (self);

  groups = g_key_file_get_groups (metakey, NULL);
  for (i = 0; groups[i] != NULL; i++)
    {
      char *tagged_extension;

      if (g_str_has_prefix (groups[i], FLATPAK_METADATA_GROUP_PREFIX_EXTENSION) &&
          *(tagged_extension = (groups[i] + strlen (FLATPAK_METADATA_GROUP_PREFIX_EXTENSION))) != 0)
        {
          g_autofree char *extension = NULL;
          g_autofree char *version = g_key_file_get_string (metakey, groups[i],
                                                            FLATPAK_METADATA_KEY_VERSION, NULL);
          g_auto(GStrv) versions = g_key_file_get_string_list (metakey, groups[i],
                                                               FLATPAK_METADATA_KEY_VERSIONS,
                                                               NULL, NULL);
          gboolean subdirectories = g_key_file_get_boolean (metakey, groups[i],
                                                            FLATPAK_METADATA_KEY_SUBDIRECTORIES, NULL);
          gboolean no_autodownload = g_key_file_get_boolean (metakey, groups[i],
                                                             FLATPAK_METADATA_KEY_NO_AUTODOWNLOAD, NULL);
          g_autofree char *download_if = g_key_file_get_string (metakey, groups[i],
                                                                FLATPAK_METADATA_KEY_DOWNLOAD_IF, NULL);
          g_autofree char *autoprune_unless = g_key_file_get_string (metakey, groups[i],
                                                                     FLATPAK_METADATA_KEY_AUTOPRUNE_UNLESS, NULL);
          gboolean autodelete = g_key_file_get_boolean (metakey, groups[i],
                                                        FLATPAK_METADATA_KEY_AUTODELETE, NULL);
          gboolean locale_subset = g_key_file_get_boolean (metakey, groups[i],
                                                           FLATPAK_METADATA_KEY_LOCALE_SUBSET, NULL);
          const char *default_branches[] = { NULL, NULL};
          const char **branches;
          int branch_i;

          /* Parse actual extension name */
          flatpak_parse_extension_with_tag (tagged_extension, &extension, NULL);

          if (versions)
            branches = (const char **) versions;
          else
            {
              if (version)
                default_branches[0] = version;
              else
                default_branches[0] = ref_branch;
              branches = default_branches;
            }

          for (branch_i = 0; branches[branch_i] != NULL; branch_i++)
            {
              g_autoptr(FlatpakDecomposed) extension_ref = NULL;
              g_autofree char *checksum = NULL;
              const char *branch = branches[branch_i];

              extension_ref = flatpak_decomposed_new_from_parts (FLATPAK_KINDS_RUNTIME,
                                                                 extension, ref_arch, branch, NULL);
              if (extension_ref == NULL)
                continue;

              if (flatpak_remote_state_lookup_ref (state, flatpak_decomposed_get_ref (extension_ref), &checksum, NULL, NULL, NULL, NULL))
                {
                  if (flatpak_filters_allow_ref (NULL, masked, flatpak_decomposed_get_ref (extension_ref)))
                    add_related (self, related, extension, extension_ref, checksum,
                                 no_autodownload, download_if, autoprune_unless, autodelete, locale_subset);
                }
              else if (subdirectories)
                {
                  g_autoptr(GPtrArray) subref_refs = flatpak_remote_state_match_subrefs (state, extension_ref);
                  for (int j = 0; j < subref_refs->len; j++)
                    {
                      FlatpakDecomposed *subref_ref = g_ptr_array_index (subref_refs, j);
                      g_autofree char *subref_checksum = NULL;

                      if (flatpak_remote_state_lookup_ref (state, flatpak_decomposed_get_ref (subref_ref),
                                                           &subref_checksum, NULL, NULL, NULL, NULL) &&
                          flatpak_filters_allow_ref (NULL, masked,  flatpak_decomposed_get_ref (subref_ref)))
                        add_related (self, related, extension, subref_ref, subref_checksum,
                                     no_autodownload, download_if, autoprune_unless, autodelete, locale_subset);
                    }
                }
            }
        }
    }

  return g_steal_pointer (&related);
}