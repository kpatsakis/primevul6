flatpak_context_add_bus_filters (FlatpakContext *context,
                                 const char     *app_id,
                                 gboolean        session_bus,
                                 gboolean        sandboxed,
                                 FlatpakBwrap   *bwrap)
{
  GHashTable *ht;
  GHashTableIter iter;
  gpointer key, value;

  flatpak_bwrap_add_arg (bwrap, "--filter");
  if (app_id && session_bus)
    {
      if (!sandboxed)
        {
          flatpak_bwrap_add_arg_printf (bwrap, "--own=%s.*", app_id);
          flatpak_bwrap_add_arg_printf (bwrap, "--own=org.mpris.MediaPlayer2.%s.*", app_id);
        }
      else
        flatpak_bwrap_add_arg_printf (bwrap, "--own=%s.Sandboxed.*", app_id);
    }

  if (session_bus)
    ht = context->session_bus_policy;
  else
    ht = context->system_bus_policy;

  g_hash_table_iter_init (&iter, ht);
  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      FlatpakPolicy policy = GPOINTER_TO_INT (value);

      if (policy > 0)
        flatpak_bwrap_add_arg_printf (bwrap, "--%s=%s",
                                      flatpak_policy_to_string (policy),
                                      (char *) key);
    }
}