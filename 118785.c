irc_server_sasl_enabled (struct t_irc_server *server)
{
    int sasl_mechanism, rc;
    char *sasl_username, *sasl_password;
    const char *sasl_key;

    sasl_mechanism = IRC_SERVER_OPTION_INTEGER(
        server, IRC_SERVER_OPTION_SASL_MECHANISM);
    sasl_username = irc_server_eval_expression (
        server,
        IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_SASL_USERNAME));
    sasl_password = irc_server_eval_expression (
        server,
        IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_SASL_PASSWORD));
    sasl_key = IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_SASL_KEY);

    /*
     * SASL is enabled if one of these conditions is true:
     * - mechanism is "external"
     * - mechanism is "ecdsa-nist256p-challenge" with username/key set
     * - another mechanism with username/password set
     */
    rc = ((sasl_mechanism == IRC_SASL_MECHANISM_EXTERNAL)
          || ((sasl_mechanism == IRC_SASL_MECHANISM_ECDSA_NIST256P_CHALLENGE)
              && sasl_username && sasl_username[0]
              && sasl_key && sasl_key[0])
          || (sasl_username && sasl_username[0]
              && sasl_password && sasl_password[0])) ? 1 : 0;

    if (sasl_username)
        free (sasl_username);
    if (sasl_password)
        free (sasl_password);

    return rc;
}