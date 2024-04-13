flatpak_dir_find_local_related_for_metadata (FlatpakDir        *self,
                                             FlatpakDecomposed *ref,
                                             const char        *remote_name, /* nullable */
                                             GKeyFile          *metakey,
                                             GCancellable      *cancellable,
                                             GError           **error)
{
  int i;
  g_autoptr(GPtrArray) related = g_ptr_array_new_with_free_func ((GDestroyNotify) flatpak_related_free);
  g_auto(GStrv) groups = NULL;
  g_autofree char *ref_arch = flatpak_decomposed_dup_arch (ref);
  g_autofree char *ref_branch = flatpak_decomposed_dup_branch (ref);

  if (!flatpak_dir_ensure_repo (self, cancellable, error))
    return NULL;

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
              g_autoptr(GBytes) deploy_data = NULL;
              const char *branch = branches[branch_i];

              extension_ref = flatpak_decomposed_new_from_parts (FLATPAK_KINDS_RUNTIME,
                                                                 extension, ref_arch, branch, NULL);
              if (remote_name != NULL &&
                  flatpak_repo_resolve_rev (self->repo,
                                            NULL,
                                            remote_name,
                                            flatpak_decomposed_get_ref (extension_ref),
                                            FALSE,
                                            &checksum,
                                            NULL,
                                            NULL))
                {
                  add_related (self, related, extension, extension_ref,
                               checksum, no_autodownload, download_if, autoprune_unless, autodelete, locale_subset);
                }
              else if ((deploy_data = flatpak_dir_get_deploy_data (self, extension_ref,
                                                                   FLATPAK_DEPLOY_VERSION_ANY,
                                                                   NULL, NULL)) != NULL &&
                       (remote_name == NULL || g_strcmp0 (flatpak_deploy_data_get_origin (deploy_data), remote_name) == 0))
                {
                  /* Here we're including extensions that are deployed but might
                   * not have a ref in the repo, as happens with remote-delete
                   * --force
                   */
                  checksum = g_strdup (flatpak_deploy_data_get_commit (deploy_data));
                  add_related (self, related, extension, extension_ref,
                               checksum, no_autodownload, download_if, autoprune_unless, autodelete, locale_subset);
                }
              else if (subdirectories)
                {
                  g_autoptr(GHashTable) matches = local_match_prefix (self, extension_ref, remote_name);
                  GLNX_HASH_TABLE_FOREACH (matches, FlatpakDecomposed *, match)
                    {
                      g_autofree char *match_checksum = NULL;
                      g_autoptr(GBytes) match_deploy_data = NULL;

                      if (remote_name != NULL &&
                          flatpak_repo_resolve_rev (self->repo,
                                                    NULL,
                                                    remote_name,
                                                    flatpak_decomposed_get_ref (match),
                                                    FALSE,
                                                    &match_checksum,
                                                    NULL,
                                                    NULL))
                        {
                          add_related (self, related, extension, match, match_checksum,
                                       no_autodownload, download_if, autoprune_unless, autodelete, locale_subset);
                        }
                      else if ((match_deploy_data = flatpak_dir_get_deploy_data (self, match,
                                                                                 FLATPAK_DEPLOY_VERSION_ANY,
                                                                                 NULL, NULL)) != NULL &&
                               (remote_name == NULL || g_strcmp0 (flatpak_deploy_data_get_origin (match_deploy_data), remote_name) == 0))
                        {
                          /* Here again we're including extensions that are deployed but might
                           * not have a ref in the repo
                           */
                          match_checksum = g_strdup (flatpak_deploy_data_get_commit (match_deploy_data));
                          add_related (self, related, extension, match, match_checksum,
                                       no_autodownload, download_if, autoprune_unless, autodelete, locale_subset);
                        }
                    }
                }
            }
        }
    }

  return g_steal_pointer (&related);
}