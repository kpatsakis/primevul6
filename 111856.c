flatpak_context_parse_env_block (FlatpakContext *context,
                                 const char *data,
                                 gsize length,
                                 GError **error)
{
  const char *p = data;
  gsize remaining = length;

  /* env_block might not be \0-terminated */
  while (remaining > 0)
    {
      size_t len = strnlen (p, remaining);
      const char *equals;
      g_autofree char *env_var = NULL;
      g_autofree char *env_value = NULL;

      g_assert (len <= remaining);

      equals = memchr (p, '=', len);

      if (equals == NULL || equals == p)
        return glnx_throw (error,
                           "Environment variable must be given in the form VARIABLE=VALUE, not %.*s", (int) len, p);

      env_var = g_strndup (p, equals - p);
      env_value = g_strndup (equals + 1, len - (equals - p) - 1);
      flatpak_context_set_env_var (context, env_var, env_value);
      p += len;
      remaining -= len;

      if (remaining > 0)
        {
          g_assert (*p == '\0');
          p += 1;
          remaining -= 1;
        }
    }

  return TRUE;
}