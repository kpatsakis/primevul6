flatpak_dir_install (FlatpakDir          *self,
                     gboolean             no_pull,
                     gboolean             no_deploy,
                     gboolean             no_static_deltas,
                     gboolean             reinstall,
                     gboolean             app_hint,
                     FlatpakRemoteState  *state,
                     FlatpakDecomposed   *ref,
                     const char          *opt_commit,
                     const char         **opt_subpaths,
                     const char         **opt_previous_ids,
                     GFile               *sideload_repo,
                     GBytes              *require_metadata,
                     const char          *token,
                     FlatpakProgress     *progress,
                     GCancellable        *cancellable,
                     GError             **error)
{
  FlatpakPullFlags flatpak_flags;

  flatpak_flags = FLATPAK_PULL_FLAGS_DOWNLOAD_EXTRA_DATA;
  if (no_static_deltas)
    flatpak_flags |= FLATPAK_PULL_FLAGS_NO_STATIC_DELTAS;

  if (flatpak_dir_use_system_helper (self, NULL))
    {
      g_autoptr(OstreeRepo) child_repo = NULL;
      g_auto(GLnxLockFile) child_repo_lock = { 0, };
      const char *installation = flatpak_dir_get_id (self);
      const char *empty_subpaths[] = {NULL};
      const char **subpaths;
      g_autofree char *child_repo_path = NULL;
      FlatpakHelperDeployFlags helper_flags = 0;
      g_autofree char *url = NULL;
      gboolean gpg_verify_summary;
      gboolean gpg_verify;
      gboolean is_oci;
      gboolean is_revokefs_pull = FALSE;

      if (opt_subpaths)
        subpaths = opt_subpaths;
      else
        subpaths = empty_subpaths;

      if (!ostree_repo_remote_get_url (self->repo,
                                       state->remote_name,
                                       &url,
                                       error))
        return FALSE;

      if (!ostree_repo_remote_get_gpg_verify_summary (self->repo, state->remote_name,
                                                      &gpg_verify_summary, error))
        return FALSE;

      if (!ostree_repo_remote_get_gpg_verify (self->repo, state->remote_name,
                                              &gpg_verify, error))
        return FALSE;

      is_oci = flatpak_dir_get_remote_oci (self, state->remote_name);
      if (no_pull)
        {
          /* Do nothing */
        }
      else if (is_oci)
        {
          g_autoptr(FlatpakOciRegistry) registry = NULL;
          g_autoptr(GFile) registry_file = NULL;

          registry = flatpak_dir_create_system_child_oci_registry (self, &child_repo_lock, token, error);
          if (registry == NULL)
            return FALSE;

          registry_file = g_file_new_for_uri (flatpak_oci_registry_get_uri (registry));

          child_repo_path = g_file_get_path (registry_file);

          if (!flatpak_dir_mirror_oci (self, registry, state, flatpak_decomposed_get_ref (ref), opt_commit, NULL, token, progress, cancellable, error))
            return FALSE;
        }
      else if (!gpg_verify_summary || !gpg_verify)
        {
          /* The remote is not gpg verified, so we don't want to allow installation via
             a download in the home directory, as there is no way to verify you're not
             injecting anything into the remote. However, in the case of a remote
             configured to a local filesystem we can just let the system helper do
             the installation, as it can then avoid network i/o and be certain the
             data comes from the right place. */
          if (g_str_has_prefix (url, "file:"))
            helper_flags |= FLATPAK_HELPER_DEPLOY_FLAGS_LOCAL_PULL;
          else
            return flatpak_fail_error (error, FLATPAK_ERROR_UNTRUSTED, _("Can't pull from untrusted non-gpg verified remote"));
        }
      else
        {
          /* For system pulls, the pull has to be made in a child repo first,
             which is then pulled into the system's one. The pull from child
             repo into the system repo can occur in one of the two following ways:
                1) Hard-link the child repo into system's one.
                2) Copy and verify each object from the child repo to the system's one.

             2) poses the problem of using double disk-space which might fail the
             installation of very big applications. For e.g. at endless, the encyclopedia app
             is about ~6GB, hence ~12GB of free disk-space is required to get it installed.

             For 1), we need to make sure that we address all the security concerns that
             might escalate during the pull from a remote into child repo and subsequently,
             hard-linking it into the (root-owned)system repo. This is taken care of by a
             special FUSE process(revokefs-fuse) which guards all the writes made to the
             child repo and ensures that no file descriptors remain open to the child repo
             before the hard-linkable pull is made into the system's repo.
             More details about the security issues dealt here are present at
             https://github.com/flatpak/flatpak/wiki/Noncopying-system-app-installation

             In case we fail to apply pull approach 1), the pull automatically fallbacks to use 2). */
          g_autofree gchar *src_dir = NULL;
          g_autofree gchar *mnt_dir = NULL;
          g_autoptr(GError) local_error = NULL;

          if (!flatpak_dir_setup_revokefs_fuse_mount (self,
                                                      ref,
                                                      installation,
                                                      &src_dir, &mnt_dir,
                                                      cancellable))
            {
              flatpak_dir_unmount_and_cancel_pull (self, FLATPAK_HELPER_CANCEL_PULL_FLAGS_NONE,
                                                   cancellable,
                                                   &child_repo, &child_repo_lock,
                                                   mnt_dir, src_dir);
            }
          else
            {
              g_autofree gchar *repo_basename = NULL;
              g_autoptr(GFile) mnt_dir_file = NULL;

              mnt_dir_file = g_file_new_for_path (mnt_dir);
              child_repo = flatpak_dir_create_child_repo (self, mnt_dir_file, &child_repo_lock, opt_commit, &local_error);
              if (child_repo == NULL)
                {
                  g_warning ("Cannot create repo on revokefs mountpoint %s: %s", mnt_dir, local_error->message);
                  flatpak_dir_unmount_and_cancel_pull (self,
                                                       FLATPAK_HELPER_CANCEL_PULL_FLAGS_NONE,
                                                       cancellable,
                                                       &child_repo, &child_repo_lock,
                                                       mnt_dir, src_dir);
                  g_clear_error (&local_error);
                }
              else
                {
                  repo_basename = g_file_get_basename (ostree_repo_get_path (child_repo));
                  child_repo_path = g_build_filename (src_dir, repo_basename, NULL);
                  is_revokefs_pull = TRUE;
                }
            }

          /* Fallback if revokefs-fuse setup does not succeed. This makes the pull
           * temporarily use double disk-space. */
          if (!is_revokefs_pull)
            {
             /* We're pulling from a remote source, we do the network mirroring pull as a
                user and hand back the resulting data to the system-helper, that trusts us
                due to the GPG signatures in the repo */
              child_repo = flatpak_dir_create_system_child_repo (self, &child_repo_lock, NULL, error);
              if (child_repo == NULL)
                return FALSE;
              else
                child_repo_path = g_file_get_path (ostree_repo_get_path (child_repo));
            }

          flatpak_flags |= FLATPAK_PULL_FLAGS_SIDELOAD_EXTRA_DATA;

          if (!flatpak_dir_pull (self, state, flatpak_decomposed_get_ref (ref), opt_commit, subpaths, sideload_repo, require_metadata, token,
                                 child_repo,
                                 flatpak_flags,
                                 0,
                                 progress, cancellable, error))
            {
              if (is_revokefs_pull)
                {
                  flatpak_dir_unmount_and_cancel_pull (self,
                                                       FLATPAK_HELPER_CANCEL_PULL_FLAGS_PRESERVE_PULL,
                                                       cancellable,
                                                       &child_repo, &child_repo_lock,
                                                       mnt_dir, src_dir);
                }

              return FALSE;
            }

          g_assert (child_repo_path != NULL);

          if (is_revokefs_pull &&
              !flatpak_dir_revokefs_fuse_unmount (&child_repo, &child_repo_lock, mnt_dir, &local_error))
            {
              g_propagate_prefixed_error (error, g_steal_pointer (&local_error), 
                      _("Could not unmount revokefs-fuse filesystem at %s: "), mnt_dir);

              if (src_dir &&
                  !flatpak_dir_system_helper_call_cancel_pull (self,
                                                               FLATPAK_HELPER_CANCEL_PULL_FLAGS_PRESERVE_PULL,
                                                               installation ? installation : "",
                                                               src_dir, cancellable, &local_error))
                g_warning ("Error cancelling ongoing pull at %s: %s", src_dir, local_error->message);
              return FALSE;
            }
        }

      if (no_deploy)
        helper_flags |= FLATPAK_HELPER_DEPLOY_FLAGS_NO_DEPLOY;

      if (reinstall)
        helper_flags |= FLATPAK_HELPER_DEPLOY_FLAGS_REINSTALL;

      if (app_hint)
        helper_flags |= FLATPAK_HELPER_DEPLOY_FLAGS_APP_HINT;

      helper_flags |= FLATPAK_HELPER_DEPLOY_FLAGS_INSTALL_HINT;

      if (!flatpak_dir_system_helper_call_deploy (self,
                                                  child_repo_path ? child_repo_path : "",
                                                  helper_flags, flatpak_decomposed_get_ref (ref), state->remote_name,
                                                  (const char * const *) subpaths,
                                                  (const char * const *) opt_previous_ids,
                                                  installation ? installation : "",
                                                  cancellable,
                                                  error))
        return FALSE;

      if (child_repo_path && !is_revokefs_pull)
        (void) glnx_shutil_rm_rf_at (AT_FDCWD, child_repo_path, NULL, NULL);

      return TRUE;
    }

  if (!no_pull)
    {
      if (!flatpak_dir_pull (self, state, flatpak_decomposed_get_ref (ref), opt_commit, opt_subpaths, sideload_repo, require_metadata, token, NULL,
                             flatpak_flags, OSTREE_REPO_PULL_FLAGS_NONE,
                             progress, cancellable, error))
        return FALSE;
    }

  if (!no_deploy)
    {
      if (!flatpak_dir_deploy_install (self, ref, state->remote_name, opt_subpaths,
                                       opt_previous_ids, reinstall, cancellable, error))
        return FALSE;
    }

  return TRUE;
}