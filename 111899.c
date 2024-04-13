option_share_cb (const gchar *option_name,
                 const gchar *value,
                 gpointer     data,
                 GError     **error)
{
  FlatpakContext *context = data;
  FlatpakContextShares share;

  share = flatpak_context_share_from_string (value, error);
  if (share == 0)
    return FALSE;

  flatpak_context_add_shares (context, share);

  return TRUE;
}