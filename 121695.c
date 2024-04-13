flatpak_dir_system_helper_call (FlatpakDir         *self,
                                const gchar        *method_name,
                                GVariant           *parameters,
                                const GVariantType *reply_type,
                                GUnixFDList       **out_fd_list,
                                GCancellable       *cancellable,
                                GError            **error)
{
  GVariant *res;

  if (g_once_init_enter (&self->system_helper_bus))
    {
      const char *on_session = g_getenv ("FLATPAK_SYSTEM_HELPER_ON_SESSION");
      GDBusConnection *system_helper_bus =
        g_bus_get_sync (on_session != NULL ? G_BUS_TYPE_SESSION : G_BUS_TYPE_SYSTEM,
                        cancellable, NULL);

      /* To ensure reverse mapping */
      flatpak_error_quark ();

      g_once_init_leave (&self->system_helper_bus, system_helper_bus ? system_helper_bus : (gpointer) 1 );
    }

  if (self->system_helper_bus == (gpointer) 1)
    {
      flatpak_fail (error, _("Unable to connect to system bus"));
      return NULL;
    }

  g_debug ("Calling system helper: %s", method_name);
  res = g_dbus_connection_call_with_unix_fd_list_sync (self->system_helper_bus,
                                                       "org.freedesktop.Flatpak.SystemHelper",
                                                       "/org/freedesktop/Flatpak/SystemHelper",
                                                       "org.freedesktop.Flatpak.SystemHelper",
                                                       method_name,
                                                       parameters,
                                                       reply_type,
                                                       G_DBUS_CALL_FLAGS_NONE, G_MAXINT,
                                                       NULL, out_fd_list,
                                                       cancellable,
                                                       error);

 if (res == NULL && error)
    g_dbus_error_strip_remote_error (*error);

  return res;
}