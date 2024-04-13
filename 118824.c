irc_server_fingerprint_search_algo_with_size (int size)
{
    int i;

    for (i = 0; i < IRC_FINGERPRINT_NUM_ALGOS; i++)
    {
        if (irc_fingerprint_digest_algos_size[i] == size)
            return i;
    }

    /* digest algorithm not found */
    return -1;
}