flatpak_dir_create_remote_for_ref_file (FlatpakDir         *self,
                                        GKeyFile           *keyfile,
                                        const char         *default_arch,
                                        char              **remote_name_out,
                                        char              **collection_id_out,
                                        FlatpakDecomposed **ref_out,
                                        GError            **error)
{
  g_autoptr(GBytes) gpg_data = NULL;
  g_autofree char *name = NULL;
  g_autofree char *branch = NULL;
  g_autofree char *url = NULL;
  g_autofree char *title = NULL;
  g_autofree char *remote = NULL;
  gboolean is_runtime = FALSE;
  g_autofree char *collection_id = NULL;
  g_autoptr(GFile) deploy_dir = NULL;
  g_autoptr(FlatpakDecomposed) ref = NULL;

  if (!parse_ref_file (keyfile, &name, &branch, &url, &title, &gpg_data, &is_runtime, &collection_id, error))
    return FALSE;

  ref = flatpak_decomposed_new_from_parts (is_runtime ? FLATPAK_KINDS_RUNTIME : FLATPAK_KINDS_APP,
                                           name, default_arch, branch, error);
  if (ref == NULL)
    return FALSE;

  deploy_dir = flatpak_dir_get_if_deployed (self, ref, NULL, NULL);
  if (deploy_dir != NULL)
    {
      g_set_error (error, FLATPAK_ERROR, FLATPAK_ERROR_ALREADY_INSTALLED,
                   is_runtime ? _("Runtime %s, branch %s is already installed") :
                   _("App %s, branch %s is already installed"),
                   name, branch);
      return FALSE;
    }

  /* First try to reuse existing remote */
  remote = flatpak_dir_find_remote_by_uri (self, url);

  if (remote == NULL)
    {
      remote = flatpak_dir_create_origin_remote (self, url, name, title, flatpak_decomposed_get_ref (ref),
                                                 gpg_data, collection_id, NULL, NULL, error);
      if (remote == NULL)
        return FALSE;
    }

  if (collection_id_out != NULL)
    *collection_id_out = g_steal_pointer (&collection_id);

  *remote_name_out = g_steal_pointer (&remote);
  *ref_out = g_steal_pointer (&ref);
  return TRUE;
}