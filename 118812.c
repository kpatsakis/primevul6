irc_server_get_max_modes (struct t_irc_server *server)
{
    const char *support_modes;
    char *error;
    long number;
    int max_modes;

    max_modes = 4;

    support_modes = irc_server_get_isupport_value (server, "MODES");
    if (support_modes)
    {
        error = NULL;
        number = strtol (support_modes, &error, 10);
        if (error && !error[0])
        {
            max_modes = number;
            if (max_modes < 1)
                max_modes = 1;
            if (max_modes > 128)
                max_modes = 128;
        }
    }

    return max_modes;
}