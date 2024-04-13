irc_nick_is_nick (const char *string)
{
    const char *ptr;

    if (!string || !string[0])
        return 0;

    /* first char must not be a number or hyphen */
    ptr = string;
    if (strchr ("0123456789-", *ptr))
        return 0;

    while (ptr && ptr[0])
    {
        if (!strchr (IRC_NICK_VALID_CHARS, *ptr))
            return 0;
        ptr++;
    }

    return 1;
}