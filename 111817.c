option_unset_env_cb (const gchar *option_name,
                     const gchar *value,
                     gpointer     data,
                     GError     **error)
{
  FlatpakContext *context = data;

  if (strchr (value, '=') != NULL)
    {
      g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED,
                   _("Environment variable name must not contain '=': %s"), value);
      return FALSE;
    }

  flatpak_context_set_env_var (context, value, NULL);
  return TRUE;
}