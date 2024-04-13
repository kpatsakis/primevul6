option_add_generic_policy_cb (const gchar *option_name,
                              const gchar *value,
                              gpointer     data,
                              GError     **error)
{
  FlatpakContext *context = data;
  char *t;
  g_autofree char *key = NULL;
  const char *policy_value;

  t = strchr (value, '=');
  if (t == NULL)
    {
      g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
                   _("--add-policy arguments must be in the form SUBSYSTEM.KEY=VALUE"));
      return FALSE;
    }
  policy_value = t + 1;
  key = g_strndup (value, t - value);
  if (strchr (key, '.') == NULL)
    {
      g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
                   _("--add-policy arguments must be in the form SUBSYSTEM.KEY=VALUE"));
      return FALSE;
    }

  if (policy_value[0] == '!')
    {
      g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
                   _("--add-policy values can't start with \"!\""));
      return FALSE;
    }

  flatpak_context_apply_generic_policy (context, key, policy_value);

  return TRUE;
}