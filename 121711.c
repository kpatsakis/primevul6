flatpak_dir_pull_oci (FlatpakDir          *self,
                      FlatpakRemoteState  *state,
                      const char          *ref,
                      const char          *opt_rev,
                      OstreeRepo          *repo,
                      FlatpakPullFlags     flatpak_flags,
                      OstreeRepoPullFlags  flags,
                      const char          *token,
                      FlatpakProgress     *progress,
                      GCancellable        *cancellable,
                      GError             **error)
{
  g_autoptr(FlatpakOciRegistry) registry = NULL;
  g_autoptr(FlatpakOciVersioned) versioned = NULL;
  g_autoptr(FlatpakOciImage) image_config = NULL;
  g_autofree char *full_ref = NULL;
  const char *oci_repository = NULL;
  const char *delta_url = NULL;
  g_autofree char *oci_digest = NULL;
  g_autofree char *checksum = NULL;
  VarRefInfoRef latest_rev_info;
  g_autofree char *latest_alt_commit = NULL;
  VarMetadataRef metadata;
  g_autofree char *latest_rev = NULL;
  G_GNUC_UNUSED g_autofree char *latest_commit =
    flatpak_dir_read_latest (self, state->remote_name, ref, &latest_alt_commit, cancellable, NULL);
  g_autofree char *name = NULL;

  /* We use the summary so that we can reuse any cached json */
  if (!flatpak_remote_state_lookup_ref (state, ref, &latest_rev, NULL, &latest_rev_info, NULL, error))
    return FALSE;
  if (latest_rev == NULL)
    return flatpak_fail_error (error, FLATPAK_ERROR_REF_NOT_FOUND,
                               _("Couldn't find latest checksum for ref %s in remote %s"),
                               ref, state->remote_name);

  metadata = var_ref_info_get_metadata (latest_rev_info);
  oci_repository = var_metadata_lookup_string (metadata, "xa.oci-repository", NULL);
  delta_url = var_metadata_lookup_string (metadata, "xa.delta-url", NULL);

  oci_digest = g_strconcat ("sha256:", opt_rev != NULL ? opt_rev : latest_rev, NULL);

  /* Short circuit if we've already got this commit */
  if (latest_alt_commit != NULL && strcmp (oci_digest + strlen ("sha256:"), latest_alt_commit) == 0)
    return TRUE;

  registry = flatpak_remote_state_new_oci_registry (state, token, cancellable, error);
  if (registry == NULL)
    return FALSE;

  versioned = flatpak_oci_registry_load_versioned (registry, oci_repository, oci_digest,
                                                   NULL, NULL, cancellable, error);
  if (versioned == NULL)
    return FALSE;

  if (!FLATPAK_IS_OCI_MANIFEST (versioned))
    return flatpak_fail_error (error, FLATPAK_ERROR_INVALID_DATA, _("Image is not a manifest"));

  image_config = flatpak_oci_registry_load_image_config (registry, oci_repository,
                                                         FLATPAK_OCI_MANIFEST (versioned)->config.digest,
                                                         (const char **)FLATPAK_OCI_MANIFEST (versioned)->config.urls,
                                                         NULL, cancellable, error);
  if (image_config == NULL)
    return FALSE;

  full_ref = g_strdup_printf ("%s:%s", state->remote_name, ref);

  if (repo == NULL)
    repo = self->repo;

  flatpak_progress_start_oci_pull (progress);

  g_debug ("Pulling OCI image %s", oci_digest);

  checksum = flatpak_pull_from_oci (repo, registry, oci_repository, oci_digest, delta_url, FLATPAK_OCI_MANIFEST (versioned), image_config,
                                    state->remote_name, ref, flatpak_flags, oci_pull_progress_cb, progress, cancellable, error);

  if (checksum == NULL)
    return FALSE;

  g_debug ("Imported OCI image as checksum %s", checksum);

  if (repo == self->repo)
    name = flatpak_dir_get_name (self);
  else
    {
      GFile *file = ostree_repo_get_path (repo);
      name = g_file_get_path (file);
    }

  (flatpak_dir_log) (self, __FILE__, __LINE__, __FUNCTION__, name,
                     "pull oci", flatpak_oci_registry_get_uri (registry), ref, NULL, NULL, NULL,
                     "Pulled %s from %s", ref, flatpak_oci_registry_get_uri (registry));

  return TRUE;
}