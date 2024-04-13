add_related (FlatpakDir        *self,
             GPtrArray         *related,
             const char        *extension,
             FlatpakDecomposed *extension_ref,
             const char        *checksum,
             gboolean           no_autodownload,
             const char        *download_if,
             const char        *autoprune_unless,
             gboolean           autodelete,
             gboolean           locale_subset)
{
  g_autoptr(GBytes) deploy_data = NULL;
  g_autofree const char **old_subpaths = NULL;
  g_autofree const char *id = NULL;
  g_autofree const char *arch = NULL;
  g_autofree const char *branch = NULL;
  g_auto(GStrv) extra_subpaths = NULL;
  g_auto(GStrv) subpaths = NULL;
  FlatpakRelated *rel;
  gboolean download;
  gboolean delete = autodelete;
  gboolean auto_prune = FALSE;
  g_autoptr(GFile) unmaintained_path = NULL;

  deploy_data = flatpak_dir_get_deploy_data (self, extension_ref, FLATPAK_DEPLOY_VERSION_ANY, NULL, NULL);

  id = flatpak_decomposed_dup_id (extension_ref);
  arch = flatpak_decomposed_dup_arch (extension_ref);
  branch = flatpak_decomposed_dup_branch (extension_ref);

  if (deploy_data)
    old_subpaths = flatpak_deploy_data_get_subpaths (deploy_data);

  /* Only respect no-autodownload/download-if for uninstalled refs, we
     always want to update if you manually installed something */
  download =
    flatpak_extension_matches_reason (id, download_if, !no_autodownload) ||
    deploy_data != NULL;

  if (!flatpak_extension_matches_reason (id, autoprune_unless, TRUE))
    auto_prune = TRUE;

  /* Don't download if there is an unmaintained extension already installed */
  unmaintained_path =
    flatpak_find_unmaintained_extension_dir_if_exists (id, arch, branch, NULL);
  if (unmaintained_path != NULL && deploy_data == NULL)
    {
      g_debug ("Skipping related extension ‘%s’ because it is already "
               "installed as an unmaintained extension in ‘%s’.",
               id, flatpak_file_get_path_cached (unmaintained_path));
      download = FALSE;
    }

  if (g_str_has_suffix (extension, ".Debug"))
    {
      /* debug files only updated if already installed */
      if (deploy_data == NULL)
        download = FALSE;

      /* Always remove debug */
      delete = TRUE;
    }

  if (g_str_has_suffix (extension, ".Locale"))
    locale_subset = TRUE;

  if (locale_subset)
    {
      extra_subpaths = flatpak_dir_get_locale_subpaths (self);

      /* Always remove locale */
      delete = TRUE;
    }

  subpaths = flatpak_subpaths_merge ((char **) old_subpaths, extra_subpaths);

  rel = g_new0 (FlatpakRelated, 1);
  rel->ref = flatpak_decomposed_ref (extension_ref);
  rel->commit = g_strdup (checksum);
  rel->subpaths = g_steal_pointer (&subpaths);
  rel->download = download;
  rel->delete = delete;
  rel->auto_prune = auto_prune;

  g_ptr_array_add (related, rel);
}