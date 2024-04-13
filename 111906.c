int pam_sm_acct_mgmt (pam_handle_t * pamh, int flags,
    int argc, const char **argv) {

    int retval, ctrl, status=PAM_AUTH_ERR;
    char *user;
    char *tty;
    char *r_addr;
    struct areply arep;
    struct tac_attrib *attr = NULL;
    int tac_fd;

    user = tty = r_addr = NULL;
    memset(&arep, 0, sizeof(arep));

    /* this also obtains service name for authorization
       this should be normally performed by pam_get_item(PAM_SERVICE)
       but since PAM service names are incompatible TACACS+
       we have to pass it via command line argument until a better
       solution is found ;) */
    ctrl = _pam_parse (argc, argv);

    if (ctrl & PAM_TAC_DEBUG)
        syslog (LOG_DEBUG, "%s: called (pam_tacplus v%u.%u.%u)"
            , __FUNCTION__, PAM_TAC_VMAJ, PAM_TAC_VMIN, PAM_TAC_VPAT);

    if ((user = _pam_get_user(pamh)) == NULL)
        return PAM_USER_UNKNOWN;

    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: username obtained [%s]", __FUNCTION__, user);

    tty = _pam_get_terminal(pamh);
    if(!strncmp(tty, "/dev/", 5))
        tty += 5;
    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: tty obtained [%s]", __FUNCTION__, tty);

    r_addr = _pam_get_rhost(pamh);
    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: rhost obtained [%s]", __FUNCTION__, r_addr);

    /* checks if user has been successfully authenticated
       by TACACS+; we cannot solely authorize user if it hasn't
       been authenticated or has been authenticated by method other
       than TACACS+ */
    if(active_server.addr == NULL) {
        _pam_log (LOG_ERR, "user not authenticated by TACACS+");
        return PAM_AUTH_ERR;
    }
    if (ctrl & PAM_TAC_DEBUG)
        syslog (LOG_DEBUG, "%s: active server is [%s]", __FUNCTION__,
            tac_ntop(active_server.addr->ai_addr));

    /* checks for specific data required by TACACS+, which should
       be supplied in command line  */
    if(!*tac_service) {
        _pam_log (LOG_ERR, "SM: TACACS+ service type not configured");
        return PAM_AUTH_ERR;
    }
    if(!*tac_protocol) {
        _pam_log (LOG_ERR, "SM: TACACS+ protocol type not configured (IGNORED)");
    }

    tac_add_attrib(&attr, "service", tac_service);
    if(tac_protocol[0] != '\0')
      tac_add_attrib(&attr, "protocol", tac_protocol);

    tac_fd = tac_connect_single(active_server.addr, active_server.key, NULL, tac_timeout);
    if(tac_fd < 0) {
        _pam_log (LOG_ERR, "TACACS+ server unavailable");
        if(arep.msg != NULL)
            free (arep.msg);
        return PAM_AUTH_ERR;
    }

    retval = tac_author_send(tac_fd, user, tty, r_addr, attr);

    tac_free_attrib(&attr);

    if(retval < 0) {
        _pam_log (LOG_ERR, "error getting authorization");
        if(arep.msg != NULL)
            free (arep.msg);

        close(tac_fd);
        return PAM_AUTH_ERR;
    }

    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: sent authorization request", __FUNCTION__);

    tac_author_read(tac_fd, &arep);

    if(arep.status != AUTHOR_STATUS_PASS_ADD &&
        arep.status != AUTHOR_STATUS_PASS_REPL) {

        _pam_log (LOG_ERR, "TACACS+ authorisation failed for [%s]", user);
        if(arep.msg != NULL)
            free (arep.msg);

        close(tac_fd);
        return PAM_PERM_DENIED;
    }

    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: user [%s] successfully authorized", __FUNCTION__, user);

    status = PAM_SUCCESS;

    attr = arep.attr;
    while (attr != NULL)  {
        char attribute[attr->attr_len];
        char value[attr->attr_len];
        char *sep;

        sep = index(attr->attr, '=');
        if(sep == NULL)
            sep = index(attr->attr, '*');
        if(sep != NULL) {
            bcopy(attr->attr, attribute, attr->attr_len-strlen(sep));
            attribute[attr->attr_len-strlen(sep)] = '\0';
            bcopy(sep, value, strlen(sep));
            value[strlen(sep)] = '\0';

            size_t i;
            for (i = 0; attribute[i] != '\0'; i++) {
                attribute[i] = toupper(attribute[i]);
                if (attribute[i] == '-')
                    attribute[i] = '_';
            }

            if (ctrl & PAM_TAC_DEBUG)
                syslog(LOG_DEBUG, "%s: returned attribute `%s%s' from server", __FUNCTION__, attribute, value);

            /* make returned attributes available for other PAM modules via PAM environment */
            if (pam_putenv(pamh, strncat(attribute, value, strlen(value))) != PAM_SUCCESS)
                syslog(LOG_WARNING, "%s: unable to set PAM environment", __FUNCTION__);

        } else {
            syslog(LOG_WARNING, "%s: invalid attribute `%s', no separator", __FUNCTION__, attr->attr);
        }
        attr = attr->next;
    }

    /* free returned attributes */
    if(arep.attr != NULL)
        tac_free_attrib(&arep.attr);

    if(arep.msg != NULL)
        free (arep.msg);

    close(tac_fd);

    return status;
}    /* pam_sm_acct_mgmt */