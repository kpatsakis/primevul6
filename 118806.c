irc_server_compare_fingerprints (const char *fingerprint,
                                 const unsigned char *fingerprint_server,
                                 ssize_t fingerprint_size)
{
    ssize_t i;
    unsigned int value;

    if ((ssize_t)strlen (fingerprint) != fingerprint_size * 2)
        return -1;

    for (i = 0; i < fingerprint_size; i++)
    {
        if (sscanf (&fingerprint[i * 2], "%02x", &value) != 1)
            return -1;
        if (value != fingerprint_server[i])
            return -1;
    }

    /* fingerprints are the same */
    return 0;
}