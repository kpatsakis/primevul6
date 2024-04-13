irc_server_get_name_without_port (const char *name)
{
    char *pos;

    if (!name)
        return NULL;

    pos = strchr (name, '/');
    if (pos && (pos != name))
        return weechat_strndup (name, pos - name);

    return strdup (name);
}