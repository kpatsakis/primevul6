irc_server_get_isupport_value (struct t_irc_server *server, const char *feature)
{
    char feature2[64], *pos_feature, *pos_equal, *pos_space;
    int length;
    static char value[256];

    if (!server || !server->isupport || !feature)
        return NULL;

    /* search feature with value */
    snprintf (feature2, sizeof (feature2), " %s=", feature);
    pos_feature = strstr (server->isupport, feature2);
    if (pos_feature)
    {
        /* feature found with value, return value */
        pos_feature++;
        pos_equal = strchr (pos_feature, '=');
        pos_space = strchr (pos_feature, ' ');
        if (pos_space)
            length = pos_space - pos_equal - 1;
        else
            length = strlen (pos_equal) + 1;
        if (length > (int)sizeof (value) - 1)
            length = (int)sizeof (value) - 1;
        memcpy (value, pos_equal + 1, length);
        value[length] = '\0';
        return value;
    }

    /* search feature without value */
    feature2[strlen (feature2) - 1] = ' ';
    pos_feature = strstr (server->isupport, feature2);
    if (pos_feature)
    {
        value[0] = '\0';
        return value;
    }

    /* feature not found in isupport */
    return NULL;
}