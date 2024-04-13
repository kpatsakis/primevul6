irc_server_fingerprint_str_sizes ()
{
    char str_sizes[1024], str_one_size[128];
    int i;

    str_sizes[0] = '\0';

    for (i = IRC_FINGERPRINT_NUM_ALGOS - 1; i >= 0; i--)
    {
        snprintf (str_one_size, sizeof (str_one_size),
                  "%d=%s%s",
                  irc_fingerprint_digest_algos_size[i] / 4,
                  irc_fingerprint_digest_algos_name[i],
                  (i > 0) ? ", " : "");
        strcat (str_sizes, str_one_size);
    }

    return strdup (str_sizes);
}