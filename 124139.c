exo_open_launch_category (const gchar *category,
                          const gchar *parameters)
{
  GtkWidget *dialog;
  GError    *error = NULL;

#ifndef NDEBUG
  g_debug ("category='%s', wd='%s', parameters='%s'", category, opt_working_directory, parameters);
#endif

  /* run the preferred application */
  if (!exo_execute_preferred_application (category, parameters, opt_working_directory, NULL, &error))
    {
      /* display an error dialog */
      dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                       _("Failed to launch preferred application for category \"%s\"."), category);
      if (startup_id != NULL)
        gtk_window_set_startup_id (GTK_WINDOW (dialog), startup_id);
      gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s.", error->message);
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      g_error_free (error);

      return FALSE;
    }

  return TRUE;
}