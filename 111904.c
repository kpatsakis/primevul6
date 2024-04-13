flatpak_context_bitmask_to_string (guint32 enabled, guint32 valid, const char **names)
{
  guint32 i;
  GPtrArray *array;

  array = g_ptr_array_new ();

  for (i = 0; names[i] != NULL; i++)
    {
      guint32 bitmask = 1 << i;
      if (valid & bitmask)
        {
          if (enabled & bitmask)
            g_ptr_array_add (array, g_strdup (names[i]));
          else
            g_ptr_array_add (array, g_strdup_printf ("!%s", names[i]));
        }
    }

  g_ptr_array_add (array, NULL);
  return (char **) g_ptr_array_free (array, FALSE);
}