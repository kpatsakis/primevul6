flatpak_dir_deploy_update (FlatpakDir        *self,
                           FlatpakDecomposed *ref,
                           const char        *checksum_or_latest,
                           const char       **opt_subpaths,
                           const char       **opt_previous_ids,
                           GCancellable      *cancellable,
                           GError           **error)
{
  g_autoptr(GBytes) old_deploy_data = NULL;
  g_auto(GLnxLockFile) lock = { 0, };
  g_autofree const char **old_subpaths = NULL;
  g_autofree char *old_active = NULL;
  const char *old_origin;
  g_autofree char *commit = NULL;
  g_auto(GStrv) previous_ids = NULL;

  if (!flatpak_dir_lock (self, &lock,
                         cancellable, error))
    return FALSE;

  old_deploy_data = flatpak_dir_get_deploy_data (self, ref,
                                                 FLATPAK_DEPLOY_VERSION_ANY,
                                                 cancellable, error);
  if (old_deploy_data == NULL)
    return FALSE;

  old_active = flatpak_dir_read_active (self, ref, cancellable);

  old_origin = flatpak_deploy_data_get_origin (old_deploy_data);
  old_subpaths = flatpak_deploy_data_get_subpaths (old_deploy_data);

  previous_ids = g_strdupv ((char **) flatpak_deploy_data_get_previous_ids (old_deploy_data, NULL));
  if (opt_previous_ids)
    {
      g_auto(GStrv) old_previous_ids = previous_ids;
      previous_ids = flatpak_strv_merge (old_previous_ids, (char **) opt_previous_ids);
    }

  if (!flatpak_dir_deploy (self,
                           old_origin,
                           ref,
                           checksum_or_latest,
                           opt_subpaths ? opt_subpaths : old_subpaths,
                           (const char * const *) previous_ids,
                           cancellable, error))
    return FALSE;

  if (old_active &&
      !flatpak_dir_undeploy (self, ref, old_active,
                             TRUE, FALSE,
                             cancellable, error))
    return FALSE;

  if (flatpak_decomposed_is_app (ref))
    {
      g_autofree char *id = flatpak_decomposed_dup_id (ref);

      if (!flatpak_dir_update_exports (self, id, cancellable, error))
        return FALSE;
    }

  /* Release lock before doing possibly slow prune */
  glnx_release_lock_file (&lock);

  if (!flatpak_dir_mark_changed (self, error))
    return FALSE;

  flatpak_dir_cleanup_removed (self, cancellable, NULL);

  commit = flatpak_dir_read_active (self, ref, cancellable);
  flatpak_dir_log (self, "deploy update", old_origin, flatpak_decomposed_get_ref (ref), commit, old_active, NULL,
                   "Updated %s from %s", flatpak_decomposed_get_ref (ref), old_origin);

  return TRUE;
}