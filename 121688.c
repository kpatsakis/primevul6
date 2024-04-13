parse_ref_file (GKeyFile *keyfile,
                char    **name_out,
                char    **branch_out,
                char    **url_out,
                char    **title_out,
                GBytes  **gpg_data_out,
                gboolean *is_runtime_out,
                char    **collection_id_out,
                GError  **error)
{
  g_autofree char *url = NULL;
  g_autofree char *title = NULL;
  g_autofree char *name = NULL;
  g_autofree char *branch = NULL;
  g_autofree char *version = NULL;
  g_autoptr(GBytes) gpg_data = NULL;
  gboolean is_runtime = FALSE;
  g_autofree char *collection_id = NULL;
  g_autofree char *str = NULL;

  *name_out = NULL;
  *branch_out = NULL;
  *url_out = NULL;
  *title_out = NULL;
  *gpg_data_out = NULL;
  *is_runtime_out = FALSE;

  if (!g_key_file_has_group (keyfile, FLATPAK_REF_GROUP))
    return flatpak_fail_error (error, FLATPAK_ERROR_INVALID_DATA, _("Invalid file format, no %s group"), FLATPAK_REF_GROUP);

  version = g_key_file_get_string (keyfile, FLATPAK_REF_GROUP,
                                   FLATPAK_REF_VERSION_KEY, NULL);
  if (version != NULL && strcmp (version, "1") != 0)
    return flatpak_fail_error (error, FLATPAK_ERROR_INVALID_DATA, _("Invalid version %s, only 1 supported"), version);

  url = g_key_file_get_string (keyfile, FLATPAK_REF_GROUP,
                               FLATPAK_REF_URL_KEY, NULL);
  if (url == NULL)
    return flatpak_fail_error (error, FLATPAK_ERROR_INVALID_DATA, _("Invalid file format, no %s specified"), FLATPAK_REF_URL_KEY);

  name = g_key_file_get_string (keyfile, FLATPAK_REF_GROUP,
                                FLATPAK_REF_NAME_KEY, NULL);
  if (name == NULL)
    return flatpak_fail_error (error, FLATPAK_ERROR_INVALID_DATA, _("Invalid file format, no %s specified"), FLATPAK_REF_NAME_KEY);

  branch = g_key_file_get_string (keyfile, FLATPAK_REF_GROUP,
                                  FLATPAK_REF_BRANCH_KEY, NULL);
  if (branch == NULL)
    branch = g_strdup ("master");

  title = g_key_file_get_string (keyfile, FLATPAK_REF_GROUP,
                                 FLATPAK_REF_TITLE_KEY, NULL);

  is_runtime = g_key_file_get_boolean (keyfile, FLATPAK_REF_GROUP,
                                       FLATPAK_REF_IS_RUNTIME_KEY, NULL);

  str = g_key_file_get_string (keyfile, FLATPAK_REF_GROUP,
                               FLATPAK_REF_GPGKEY_KEY, NULL);
  if (str != NULL)
    {
      g_autofree guchar *decoded = NULL;
      gsize decoded_len;

      str = g_strstrip (str);
      decoded = g_base64_decode (str, &decoded_len);
      if (decoded_len < 10) /* Check some minimal size so we don't get crap */
        return flatpak_fail_error (error, FLATPAK_ERROR_INVALID_DATA, _("Invalid file format, gpg key invalid"));

      gpg_data = g_bytes_new_take (g_steal_pointer (&decoded), decoded_len);
    }

  collection_id = g_key_file_get_string (keyfile, FLATPAK_REF_GROUP,
                                         FLATPAK_REF_DEPLOY_COLLECTION_ID_KEY, NULL);

  if (collection_id == NULL || *collection_id == '\0')
    {
      collection_id = g_key_file_get_string (keyfile, FLATPAK_REF_GROUP,
                                             FLATPAK_REF_COLLECTION_ID_KEY, NULL);
    }

  if (collection_id != NULL && *collection_id == '\0')
    collection_id = NULL;

  if (collection_id != NULL && gpg_data == NULL)
    return flatpak_fail_error (error, FLATPAK_ERROR_INVALID_DATA, _("Collection ID requires GPG key to be provided"));

  *name_out = g_steal_pointer (&name);
  *branch_out = g_steal_pointer (&branch);
  *url_out = g_steal_pointer (&url);
  *title_out = g_steal_pointer (&title);
  *gpg_data_out = g_steal_pointer (&gpg_data);
  *is_runtime_out = is_runtime;
  *collection_id_out = g_steal_pointer (&collection_id);

  return TRUE;
}