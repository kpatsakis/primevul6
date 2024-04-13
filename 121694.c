flatpak_dir_find_remote_by_uri (FlatpakDir *self,
                                const char *uri)
{
  g_auto(GStrv) remotes = NULL;

  g_return_val_if_fail (self != NULL, NULL);
  g_return_val_if_fail (uri != NULL, NULL);

  if (!flatpak_dir_ensure_repo (self, NULL, NULL))
    return NULL;

  remotes = flatpak_dir_list_enumerated_remotes (self, NULL, NULL);
  if (remotes)
    {
      int i;

      for (i = 0; remotes != NULL && remotes[i] != NULL; i++)
        {
          const char *remote = remotes[i];
          g_autofree char *remote_uri = NULL;

          if (!ostree_repo_remote_get_url (self->repo,
                                           remote,
                                           &remote_uri,
                                           NULL))
            continue;

          if (_flatpak_uri_equal (uri, remote_uri))
            return g_strdup (remote);
        }
    }

  return NULL;
}