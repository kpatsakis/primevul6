flatpak_dir_install_bundle (FlatpakDir         *self,
                            GFile              *file,
                            const char         *remote,
                            FlatpakDecomposed **out_ref,
                            GCancellable       *cancellable,
                            GError            **error)
{
  g_autofree char *ref_str = NULL;
  g_autoptr(FlatpakDecomposed) ref = NULL;
  g_autoptr(GBytes) deploy_data = NULL;
  g_autoptr(GVariant) metadata = NULL;
  g_autofree char *origin = NULL;
  g_autofree char *to_checksum = NULL;
  gboolean gpg_verify;

  if (!flatpak_dir_check_add_remotes_config_dir (self, error))
    return FALSE;

  if (flatpak_dir_use_system_helper (self, NULL))
    {
      const char *installation = flatpak_dir_get_id (self);

      if (!flatpak_dir_system_helper_call_install_bundle (self,
                                                          flatpak_file_get_path_cached (file),
                                                          0, remote,
                                                          installation ? installation : "",
                                                          &ref_str,
                                                          cancellable,
                                                          error))
        return FALSE;


      ref = flatpak_decomposed_new_from_ref (ref_str, error);
      if (ref == NULL)
        return FALSE;

      if (out_ref)
        *out_ref = g_steal_pointer (&ref);

      return TRUE;
    }

  if (!flatpak_dir_ensure_repo (self, cancellable, error))
    return FALSE;

  metadata = flatpak_bundle_load (file, &to_checksum,
                                  &ref,
                                  &origin,
                                  NULL, NULL,
                                  NULL, NULL, NULL,
                                  error);
  if (metadata == NULL)
    return FALSE;

  deploy_data = flatpak_dir_get_deploy_data (self, ref, FLATPAK_DEPLOY_VERSION_ANY, cancellable, NULL);
  if (deploy_data != NULL)
    {
      if (strcmp (flatpak_deploy_data_get_commit (deploy_data), to_checksum) == 0)
        {
          g_autofree char *id = flatpak_decomposed_dup_id (ref);
          g_set_error (error, FLATPAK_ERROR, FLATPAK_ERROR_ALREADY_INSTALLED,
                       _("This version of %s is already installed"), id);
          return FALSE;
        }

      if (strcmp (remote, flatpak_deploy_data_get_origin (deploy_data)) != 0)
        {
          g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                       _("Can't change remote during bundle install"));
          return FALSE;
        }
    }

  if (!ostree_repo_remote_get_gpg_verify (self->repo, remote,
                                          &gpg_verify, error))
    return FALSE;

  if (!flatpak_pull_from_bundle (self->repo,
                                 file,
                                 remote,
                                 flatpak_decomposed_get_ref (ref),
                                 gpg_verify,
                                 cancellable,
                                 error))
    return FALSE;

  if (deploy_data != NULL)
    {
      g_autofree char *group = g_strdup_printf ("remote \"%s\"", remote);
      g_autofree char *old_url = NULL;
      g_autoptr(GKeyFile) new_config = NULL;

      /* The pull succeeded, and this is an update. So, we need to update the repo config
         if anything changed */
      ostree_repo_remote_get_url (self->repo,
                                  remote,
                                  &old_url,
                                  NULL);
      if (origin != NULL &&
          (old_url == NULL || strcmp (old_url, origin) != 0))
        {
          if (new_config == NULL)
            new_config = ostree_repo_copy_config (self->repo);

          g_key_file_set_value (new_config, group, "url", origin);
        }

      if (new_config)
        {
          if (!flatpak_dir_cleanup_remote_for_url_change (self, remote,
                                                          origin, cancellable, error))
            return FALSE;

          if (!ostree_repo_write_config (self->repo, new_config, error))
            return FALSE;
        }
    }

  if (deploy_data)
    {
      if (!flatpak_dir_deploy_update (self, ref, NULL, NULL, NULL, cancellable, error))
        return FALSE;
    }
  else
    {
      if (!flatpak_dir_deploy_install (self, ref, remote, NULL, NULL, FALSE, cancellable, error))
        return FALSE;
    }

  if (out_ref)
    *out_ref = g_steal_pointer (&ref);

  return TRUE;
}