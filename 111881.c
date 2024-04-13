option_allow_cb (const gchar *option_name,
                 const gchar *value,
                 gpointer     data,
                 GError     **error)
{
  FlatpakContext *context = data;
  FlatpakContextFeatures feature;

  feature = flatpak_context_feature_from_string (value, error);
  if (feature == 0)
    return FALSE;

  flatpak_context_add_features (context, feature);

  return TRUE;
}