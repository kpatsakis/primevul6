irc_server_get_tags_to_send (const char *tags)
{
    int length;
    char *buf;

    if (!tags && !irc_server_send_default_tags)
        return NULL;

    if (!tags)
        return strdup (irc_server_send_default_tags);

    if (!irc_server_send_default_tags)
        return strdup (tags);

    /* concatenate tags and irc_server_send_default_tags */
    length = strlen (tags) + 1 + strlen (irc_server_send_default_tags) + 1;
    buf = malloc (length);
    if (buf)
        snprintf (buf, length, "%s,%s", tags, irc_server_send_default_tags);
    return buf;
}