flatpak_dir_get_config_strv (FlatpakDir *self, char *key) 
{
  GKeyFile *config = flatpak_dir_get_repo_config (self);
  g_auto(GStrv) lang = NULL;

  if (config)
    {
      if (g_key_file_has_key (config, "core", key, NULL))
        {
          lang = g_key_file_get_string_list (config, "core", key, NULL, NULL);
          return g_steal_pointer (&lang);
        }
    }
  return NULL;
}