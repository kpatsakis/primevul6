irc_server_apply_command_line_options (struct t_irc_server *server,
                                       int argc, char **argv)
{
    int i, index_option;
    char *pos, *option_name, *ptr_value, *value_boolean[2] = { "off", "on" };

    for (i = 0; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            pos = strchr (argv[i], '=');
            if (pos)
            {
                option_name = weechat_strndup (argv[i] + 1, pos - argv[i] - 1);
                ptr_value = pos + 1;
            }
            else
            {
                option_name = strdup (argv[i] + 1);
                ptr_value = value_boolean[1];
            }
            if (option_name)
            {
                if (weechat_strcasecmp (option_name, "temp") == 0)
                {
                    /* temporary server, not saved */
                    server->temp_server = 1;
                }
                else
                {
                    index_option = irc_server_search_option (option_name);
                    if (index_option < 0)
                    {
                        /* look if option is negative, like "-noxxx" */
                        if (weechat_strncasecmp (argv[i], "-no", 3) == 0)
                        {
                            free (option_name);
                            option_name = strdup (argv[i] + 3);
                            index_option = irc_server_search_option (option_name);
                            ptr_value = value_boolean[0];
                        }
                    }
                    if (index_option >= 0)
                    {
                        weechat_config_option_set (server->options[index_option],
                                                   ptr_value, 1);
                    }
                }
                free (option_name);
            }
        }
    }
}