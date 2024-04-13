main (gint argc, gchar **argv)
{
  GOptionContext  *context;
  GtkWidget       *dialog;
  GtkWidget       *message_area;
  GtkWidget       *label;
  GError          *err = NULL;
  gchar           *parameter, *quoted;
  gint             result = EXIT_SUCCESS;
  GString         *join;
  guint            i;
  gchar           *uri;

#ifdef GETTEXT_PACKAGE
  /* setup i18n support */
  xfce_textdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");
#endif

  /* steal the startup id, before gtk tries to grab it */
  startup_id = g_strdup (g_getenv ("DESKTOP_STARTUP_ID"));
  if (startup_id != NULL)
    g_unsetenv ("DESKTOP_STARTUP_ID");

  /* try to parse the command line parameters */
  context = g_option_context_new (NULL);
  g_option_context_set_help_enabled (context, FALSE);
  g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
  g_option_context_add_group (context, gtk_get_option_group (TRUE));
  g_option_context_set_ignore_unknown_options (context, TRUE);
  if (!g_option_context_parse (context, &argc, &argv, &err))
    {
      g_fprintf (stderr, "exo-open: %s.\n", err->message);
      g_error_free (err);
      return EXIT_FAILURE;
    }

  /* restore the startup-id for the child environment */
  if (startup_id != NULL)
    g_setenv ("DESKTOP_STARTUP_ID", startup_id, TRUE);

  /* setup default icon for windows */
  gtk_window_set_default_icon_name ("preferences-desktop-default-applications");

  /* check what to do */
  if (G_LIKELY (opt_launch != NULL))
    {
      if (argc > 1)
        {
          /* NOTE: see the comment at the top of this document! */

          /* combine all specified parameters to one parameter string */
          join = g_string_new (NULL);
          for (i = 1; argv[i] != NULL; i++)
            {
              /* separate the arguments */
              if (i > 1)
                join = g_string_append_c (join, ' ');

              /* only quote arguments with spaces if there are multiple
               * arguments to be merged, this is a bit of magic to make
               * common cares work property, see sample above with xfrun4 */
              if (argc > 2 && strchr (argv[i], ' ') != NULL)
                {
                  quoted = g_shell_quote (argv[i]);
                  join = g_string_append (join, quoted);
                  g_free (quoted);
                }
              else
                {
                  join = g_string_append (join, argv[i]);
                }
            }
          parameter = g_string_free (join, FALSE);
        }
      else
        {
          parameter = NULL;
        }

      /* run the preferred application */
      if (!exo_open_launch_category (opt_launch, parameter))
        result = EXIT_FAILURE;

      g_free (parameter);
    }
  else if (argc > 1)
    {
      /* open all specified urls */
      for (argv += 1; result == EXIT_SUCCESS && *argv != NULL; ++argv)
        {
          if (g_str_has_suffix (*argv, ".desktop")
              && exo_open_launch_desktop_file (*argv))
            {
              /* successfully launched a desktop file */
              continue;
            }
          else if (exo_str_looks_like_an_uri (*argv))
            {
              /* use the argument directly */
              uri = g_strdup (*argv);
            }
          else
            {
              /* try to build a valid uri */
              uri = exo_open_find_scheme (*argv);
            }

#ifndef NDEBUG
          g_debug ("opening the following uri: %s", uri);
#endif

          if (uri == NULL)
            {
              /* display an error dialog */
              dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                               _("Unable to detect the URI-scheme of \"%s\"."), *argv);
              if (startup_id != NULL)
                gtk_window_set_startup_id (GTK_WINDOW (dialog), startup_id);
              gtk_dialog_run (GTK_DIALOG (dialog));
              gtk_widget_destroy (dialog);

              result = EXIT_FAILURE;
            }
          else if (!exo_open_uri (uri, &err))
            {
              if (err != NULL)
                {
                  /* display an error dialog */
                  dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                                   _("Failed to open URI."));
                  if (startup_id != NULL)
                    gtk_window_set_startup_id (GTK_WINDOW (dialog), startup_id);
                  gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s.", err->message);
                  g_error_free (err);

                  /* add full URI to message area */
                  message_area = gtk_message_dialog_get_message_area (GTK_MESSAGE_DIALOG (dialog));
                  label = gtk_label_new (uri);
                  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
                  gtk_label_set_line_wrap_mode (GTK_LABEL (label), PANGO_WRAP_CHAR);
                  gtk_label_set_max_width_chars (GTK_LABEL (label), 50);
                  gtk_container_add (GTK_CONTAINER (message_area), label);
                  gtk_widget_show (label);

                  gtk_dialog_run (GTK_DIALOG (dialog));
                  gtk_widget_destroy (dialog);
                }

              result = EXIT_FAILURE;
            }

          g_free (uri);
        }
    }
  else if (G_UNLIKELY (opt_help))
    {
      usage ();
    }
  else if (G_UNLIKELY (opt_version))
    {
      g_print ("%s %s\n\n", g_get_prgname (), PACKAGE_VERSION);
      g_print (_("Copyright (c) %s\n"
                 "        os-cillation e.K. All rights reserved.\n\n"
                 "Written by Benedikt Meurer <benny@xfce.org>.\n\n"),
               "2005-2007");
      g_print (_("%s comes with ABSOLUTELY NO WARRANTY,\n"
                 "You may redistribute copies of %s under the terms of\n"
                 "the GNU Lesser General Public License which can be found in the\n"
                 "%s source package.\n\n"), g_get_prgname (), g_get_prgname (), PACKAGE_TARNAME);
      g_print (_("Please report bugs to <%s>.\n"), PACKAGE_BUGREPORT);
    }
  else
    {
      result = EXIT_FAILURE;
      usage ();
    }

  return result;
}