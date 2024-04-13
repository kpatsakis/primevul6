g_markup_collect_attributes (const gchar         *element_name,
                             const gchar        **attribute_names,
                             const gchar        **attribute_values,
                             GError             **error,
                             GMarkupCollectType   first_type,
                             const gchar         *first_attr,
                             ...)
{
  GMarkupCollectType type;
  const gchar *attr;
  guint64 collected;
  int written;
  va_list ap;
  int i;

  type = first_type;
  attr = first_attr;
  collected = 0;
  written = 0;

  va_start (ap, first_attr);
  while (type != G_MARKUP_COLLECT_INVALID)
    {
      gboolean mandatory;
      const gchar *value;

      mandatory = !(type & G_MARKUP_COLLECT_OPTIONAL);
      type &= (G_MARKUP_COLLECT_OPTIONAL - 1);

      /* tristate records a value != TRUE and != FALSE
       * for the case where the attribute is missing
       */
      if (type == G_MARKUP_COLLECT_TRISTATE)
        mandatory = FALSE;

      for (i = 0; attribute_names[i]; i++)
        if (i >= 40 || !(collected & (G_GUINT64_CONSTANT(1) << i)))
          if (!strcmp (attribute_names[i], attr))
            break;

      /* ISO C99 only promises that the user can pass up to 127 arguments.
       * Subtracting the first 4 arguments plus the final NULL and dividing
       * by 3 arguments per collected attribute, we are left with a maximum
       * number of supported attributes of (127 - 5) / 3 = 40.
       *
       * In reality, nobody is ever going to call us with anywhere close to
       * 40 attributes to collect, so it is safe to assume that if i > 40
       * then the user has given some invalid or repeated arguments.  These
       * problems will be caught and reported at the end of the function.
       *
       * We know at this point that we have an error, but we don't know
       * what error it is, so just continue...
       */
      if (i < 40)
        collected |= (G_GUINT64_CONSTANT(1) << i);

      value = attribute_values[i];

      if (value == NULL && mandatory)
        {
          g_set_error (error, G_MARKUP_ERROR,
                       G_MARKUP_ERROR_MISSING_ATTRIBUTE,
                       "element '%s' requires attribute '%s'",
                       element_name, attr);

          va_end (ap);
          goto failure;
        }

      switch (type)
        {
        case G_MARKUP_COLLECT_STRING:
          {
            const char **str_ptr;

            str_ptr = va_arg (ap, const char **);

            if (str_ptr != NULL)
              *str_ptr = value;
          }
          break;

        case G_MARKUP_COLLECT_STRDUP:
          {
            char **str_ptr;

            str_ptr = va_arg (ap, char **);

            if (str_ptr != NULL)
              *str_ptr = g_strdup (value);
          }
          break;

        case G_MARKUP_COLLECT_BOOLEAN:
        case G_MARKUP_COLLECT_TRISTATE:
          if (value == NULL)
            {
              gboolean *bool_ptr;

              bool_ptr = va_arg (ap, gboolean *);

              if (bool_ptr != NULL)
                {
                  if (type == G_MARKUP_COLLECT_TRISTATE)
                    /* constructivists rejoice!
                     * neither false nor true...
                     */
                    *bool_ptr = -1;

                  else /* G_MARKUP_COLLECT_BOOLEAN */
                    *bool_ptr = FALSE;
                }
            }
          else
            {
              if (!g_markup_parse_boolean (value, va_arg (ap, gboolean *)))
                {
                  g_set_error (error, G_MARKUP_ERROR,
                               G_MARKUP_ERROR_INVALID_CONTENT,
                               "element '%s', attribute '%s', value '%s' "
                               "cannot be parsed as a boolean value",
                               element_name, attr, value);

                  va_end (ap);
                  goto failure;
                }
            }

          break;

        default:
          g_assert_not_reached ();
        }

      type = va_arg (ap, GMarkupCollectType);
      attr = va_arg (ap, const char *);
      written++;
    }
  va_end (ap);

  /* ensure we collected all the arguments */
  for (i = 0; attribute_names[i]; i++)
    if ((collected & (G_GUINT64_CONSTANT(1) << i)) == 0)
      {
        /* attribute not collected:  could be caused by two things.
         *
         * 1) it doesn't exist in our list of attributes
         * 2) it existed but was matched by a duplicate attribute earlier
         *
         * find out.
         */
        int j;

        for (j = 0; j < i; j++)
          if (strcmp (attribute_names[i], attribute_names[j]) == 0)
            /* duplicate! */
            break;

        /* j is now the first occurrence of attribute_names[i] */
        if (i == j)
          g_set_error (error, G_MARKUP_ERROR,
                       G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE,
                       "attribute '%s' invalid for element '%s'",
                       attribute_names[i], element_name);
        else
          g_set_error (error, G_MARKUP_ERROR,
                       G_MARKUP_ERROR_INVALID_CONTENT,
                       "attribute '%s' given multiple times for element '%s'",
                       attribute_names[i], element_name);

        goto failure;
      }

  return TRUE;

failure:
  /* replay the above to free allocations */
  type = first_type;
  attr = first_attr;

  va_start (ap, first_attr);
  while (type != G_MARKUP_COLLECT_INVALID)
    {
      gpointer ptr;

      ptr = va_arg (ap, gpointer);

      if (ptr != NULL)
        {
          switch (type & (G_MARKUP_COLLECT_OPTIONAL - 1))
            {
            case G_MARKUP_COLLECT_STRDUP:
              if (written)
                g_free (*(char **) ptr);

            case G_MARKUP_COLLECT_STRING:
              *(char **) ptr = NULL;
              break;

            case G_MARKUP_COLLECT_BOOLEAN:
              *(gboolean *) ptr = FALSE;
              break;

            case G_MARKUP_COLLECT_TRISTATE:
              *(gboolean *) ptr = -1;
              break;
            }
        }

      type = va_arg (ap, GMarkupCollectType);
      attr = va_arg (ap, const char *);
    }
  va_end (ap);

  return FALSE;
}