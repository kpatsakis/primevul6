flatpak_dir_deploy_install (FlatpakDir        *self,
                            FlatpakDecomposed *ref,
                            const char        *origin,
                            const char       **subpaths,
                            const char       **previous_ids,
                            gboolean           reinstall,
                            GCancellable      *cancellable,
                            GError           **error)
{
  g_auto(GLnxLockFile) lock = { 0, };
  g_autoptr(GFile) deploy_base = NULL;
  g_autoptr(GFile) old_deploy_dir = NULL;
  gboolean created_deploy_base = FALSE;
  gboolean ret = FALSE;
  g_autoptr(GError) local_error = NULL;
  g_autofree char *remove_ref_from_remote = NULL;
  g_autofree char *commit = NULL;
  g_autofree char *old_active = NULL;

  if (!flatpak_dir_lock (self, &lock,
                         cancellable, error))
    goto out;

  old_deploy_dir = flatpak_dir_get_if_deployed (self, ref, NULL, cancellable);
  if (old_deploy_dir != NULL)
    {
      old_active = flatpak_dir_read_active (self, ref, cancellable);

      if (reinstall)
        {
          g_autoptr(GBytes) old_deploy = NULL;
          const char *old_origin;

          old_deploy = flatpak_load_deploy_data (old_deploy_dir, ref, self->repo, FLATPAK_DEPLOY_VERSION_ANY, cancellable, error);
          if (old_deploy == NULL)
            goto out;

          /* If the old install was from a different remote, remove the ref */
          old_origin = flatpak_deploy_data_get_origin (old_deploy);
          if (strcmp (old_origin, origin) != 0)
            remove_ref_from_remote = g_strdup (old_origin);

          g_debug ("Removing old deployment for reinstall");
          if (!flatpak_dir_undeploy (self, ref, old_active,
                                     TRUE, FALSE,
                                     cancellable, error))
            goto out;
        }
      else
        {
          g_autofree char *id = flatpak_decomposed_dup_id (ref);
          g_autofree char *branch = flatpak_decomposed_dup_branch (ref);
          g_set_error (error, FLATPAK_ERROR, FLATPAK_ERROR_ALREADY_INSTALLED,
                       _("%s branch %s already installed"), id, branch);
          goto out;
        }
    }

  deploy_base = flatpak_dir_get_deploy_dir (self, ref);
  if (!g_file_make_directory_with_parents (deploy_base, cancellable, &local_error))
    {
      if (!g_error_matches (local_error, G_IO_ERROR, G_IO_ERROR_EXISTS))
        {
          g_propagate_error (error, g_steal_pointer (&local_error));
          goto out;
        }
    }

  /* After we create the deploy base we must goto out on errors */
  created_deploy_base = TRUE;

  if (!flatpak_dir_deploy (self, origin, ref, NULL, (const char * const *) subpaths,
                           previous_ids, cancellable, error))
    goto out;

  if (flatpak_decomposed_is_app (ref))
    {
      g_autofree char *id = flatpak_decomposed_dup_id (ref);

      if (!flatpak_dir_make_current_ref (self, ref, cancellable, error))
        goto out;

      if (!flatpak_dir_update_exports (self, id, cancellable, error))
        goto out;
    }

  /* Remove old ref if the reinstalled was from a different remote */
  if (remove_ref_from_remote != NULL)
    {
      if (!flatpak_dir_remove_ref (self, remove_ref_from_remote, flatpak_decomposed_get_ref (ref), cancellable, error))
        goto out;

      flatpak_dir_prune_origin_remote (self, remove_ref_from_remote);
    }

  /* Release lock before doing possibly slow prune */
  glnx_release_lock_file (&lock);

  flatpak_dir_cleanup_removed (self, cancellable, NULL);

  if (!flatpak_dir_mark_changed (self, error))
    goto out;

  ret = TRUE;

  commit = flatpak_dir_read_active (self, ref, cancellable);
  flatpak_dir_log (self, "deploy install", origin, flatpak_decomposed_get_ref (ref), commit, old_active, NULL,
                   "Installed %s from %s", flatpak_decomposed_get_ref (ref), origin);

out:
  if (created_deploy_base && !ret)
    flatpak_rm_rf (deploy_base, cancellable, NULL);

  return ret;
}