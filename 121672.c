get_locale_langs_from_localed_dbus (GDBusProxy *proxy, GPtrArray *langs)
{
  g_autoptr(GVariant) locale_variant = NULL;
  g_autofree const gchar **strv = NULL;
  gsize i, j;

  locale_variant = g_dbus_proxy_get_cached_property (proxy, "Locale");
  if (locale_variant == NULL)
    return;

  strv = g_variant_get_strv (locale_variant, NULL);

  for (i = 0; strv[i]; i++)
    {
      const gchar *locale = NULL;
      g_autofree char *lang = NULL;

      /* See locale(7) for these categories */
      const char * const categories[] = { "LANG=", "LC_ALL=", "LC_MESSAGES=", "LC_ADDRESS=",
                                          "LC_COLLATE=", "LC_CTYPE=", "LC_IDENTIFICATION=",
                                          "LC_MONETARY=", "LC_MEASUREMENT=", "LC_NAME=",
                                          "LC_NUMERIC=", "LC_PAPER=", "LC_TELEPHONE=",
                                          "LC_TIME=", NULL };

      for (j = 0; categories[j]; j++)
        {
          if (g_str_has_prefix (strv[i], categories[j]))
            {
              locale = strv[i] + strlen (categories[j]);
              break;
            }
        }

      if (locale == NULL || strcmp (locale, "") == 0)
        continue;

      lang = flatpak_get_lang_from_locale (locale);
      if (lang != NULL && !flatpak_g_ptr_array_contains_string (langs, lang))
        g_ptr_array_add (langs, g_steal_pointer (&lang));
    }
}