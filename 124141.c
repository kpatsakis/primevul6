exo_open_uri_known_category (const gchar  *uri,
                             const gchar  *scheme,
                             gboolean     *succeed)
{
  guint        i;
  const gchar *category = NULL;

  g_return_val_if_fail (uri != NULL, FALSE);
  g_return_val_if_fail (scheme != NULL, FALSE);

  /* check if the scheme matches a known preferred application type */
  for (i = 0; category == NULL && i < G_N_ELEMENTS (known_schemes); i++)
    {
      if (g_regex_match_simple (known_schemes[i].pattern, scheme, G_REGEX_CASELESS, 0))
        {
          /* launch the preferred application */
          *succeed = exo_open_launch_category (known_schemes[i].category, uri);

          /* we always return, because we found a matching scheme */
          return TRUE;
        }
    }

  return FALSE;
}