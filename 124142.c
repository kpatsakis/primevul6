exo_open_uri (const gchar  *uri,
              GError      **error)
{
  GFile               *file;
  gchar               *scheme;
  GFileInfo           *file_info;
  gboolean             succeed = FALSE;
  gboolean             retval = FALSE;
  GFileType            file_type;
  const gchar         *content_type;
  GAppInfo            *app_info;
  gchar               *path;
  const gchar         *executable;
  GList                fake_list;
  const gchar * const *schemes;
  GError              *err = NULL;
  guint                i;

  g_return_val_if_fail (uri != NULL, FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

#ifndef NDEBUG
  schemes = g_vfs_get_supported_uri_schemes (g_vfs_get_default ());
  scheme = g_strjoinv (", ", (gchar **) schemes);
  g_debug ("vfs supported schemes: %s", scheme);
  g_free (scheme);
#endif

  file = g_file_new_for_uri (uri);
  scheme = g_file_get_uri_scheme (file);

  /* try to launch common schemes for know preferred applications */
  if (scheme != NULL && exo_open_uri_known_category (uri, scheme, &retval))
    {
      g_free (scheme);
      return retval;
    }

  /* handle the uri as a file, maybe we succeed... */
  file_info = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_TYPE ","
                                 G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
                                 G_FILE_QUERY_INFO_NONE, NULL, &err);
  if (file_info != NULL)
    {
      file_type = g_file_info_get_file_type (file_info);
      if (file_type == G_FILE_TYPE_DIRECTORY)
        {
#ifndef NDEBUG
          g_debug ("file is directory, use filemanager");
#endif
          /* directories should go fine with a file manager */
          retval = exo_open_launch_category ("FileManager", uri);
          succeed = TRUE;
        }
      else
        {
          content_type = g_file_info_get_content_type (file_info);
#ifndef NDEBUG
          g_debug ("content type=%s", content_type);
#endif
          if (G_LIKELY (content_type))
            {
              /* try to find a suitable application for this content type */
              path = g_file_get_path (file);
              app_info = g_app_info_get_default_for_type (content_type, path == NULL);
              g_free (path);

              if (app_info != NULL)
                {
                  /* make sure we don't loop somehow */
                  executable = g_app_info_get_executable (app_info);
#ifndef NDEBUG
                  g_debug ("default executable=%s", executable);
#endif
                  if (executable == NULL
                      || strcmp (executable, "exo-open") != 0)
                    {
                      fake_list.data = (gpointer) uri;
                      fake_list.prev = fake_list.next = NULL;

                      /* launch it */
                      retval = g_app_info_launch_uris (app_info, &fake_list, NULL, &err);
                      succeed = TRUE;
                    }

                  g_object_unref (G_OBJECT (app_info));
                }
            }
        }

      g_object_unref (G_OBJECT (file_info));
    }
  else if (err != NULL
           && scheme != NULL
           && err->code == G_IO_ERROR_NOT_MOUNTED)
    {
      /* check if the scheme is supported by gio */
      schemes = g_vfs_get_supported_uri_schemes (g_vfs_get_default ());
      if (G_LIKELY (schemes != NULL))
        {
          for (i = 0; schemes[i] != NULL; i++)
            {
              /* found scheme, open in file manager */
              if (strcmp (scheme, schemes[i]) == 0)
                {
                  retval = succeed = exo_open_launch_category ("FileManager", uri);
                  break;
                }
            }
        }
    }

  g_object_unref (G_OBJECT (file));

  /* our last try... */
  if (!succeed)
    {
#ifndef NDEBUG
          g_debug ("nothing worked, try ftp(s) or gtk_show_uri()");
#endif

      /* try ftp uris if the file manager/gio failed to recognize it */
      if (scheme != NULL
          && (strcmp (scheme, "ftp") == 0 || strcmp (scheme, "ftps") == 0))
        retval = exo_open_launch_category ("WebBrowser", uri);
      else
        retval = gtk_show_uri_on_window (NULL, uri, 0, error);
    }

  g_free (scheme);

  if (!retval && error != NULL)
    *error = err;
  else if (err != NULL)
    g_error_free (err);

  return retval;
}