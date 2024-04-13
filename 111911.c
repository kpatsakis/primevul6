int _pam_account(pam_handle_t *pamh, int argc, const char **argv,
    int type, char *cmd) {

    int retval;
    static int ctrl;
    char *user = NULL;
    char *tty = NULL;
    char *r_addr = NULL;
    char *typemsg;
    int status = PAM_SESSION_ERR;
    int srv_i, tac_fd;

    typemsg = tac_acct_flag2str(type);
    ctrl = _pam_parse (argc, argv);

    if (ctrl & PAM_TAC_DEBUG) {
        syslog (LOG_DEBUG, "%s: [%s] called (pam_tacplus v%u.%u.%u)",
            __FUNCTION__, typemsg, PAM_TAC_VMAJ, PAM_TAC_VMIN, PAM_TAC_VPAT);
        syslog(LOG_DEBUG, "%s: tac_srv_no=%d", __FUNCTION__, tac_srv_no);
    }

    if ((user = _pam_get_user(pamh)) == NULL)
        return PAM_USER_UNKNOWN;

    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: username [%s] obtained", __FUNCTION__, user);

    tty = _pam_get_terminal(pamh);
    if(!strncmp(tty, "/dev/", 5))
        tty += 5;
    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: tty [%s] obtained", __FUNCTION__, tty);

    r_addr = _pam_get_rhost(pamh);
    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: rhost [%s] obtained", __FUNCTION__, r_addr);

    /* checks for specific data required by TACACS+, which should
       be supplied in command line  */
    if(*tac_service == '\0') {
        _pam_log (LOG_ERR, "ACC: TACACS+ service type not configured");
        return PAM_AUTH_ERR;
    }
    if(*tac_protocol == '\0') {
        _pam_log (LOG_ERR, "ACC: TACACS+ protocol type not configured (IGNORED)");
    }

    /* when this module is called from within pppd or other
       application dealing with serial lines, it is likely
       that we will get hit with signal caused by modem hangup;
       this is important only for STOP packets, it's relatively
       rare that modem hangs up on accounting start */
    if(type == TAC_PLUS_ACCT_FLAG_STOP) {
        signal(SIGALRM, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);
        signal(SIGHUP, SIG_IGN);
    }

    status = PAM_SESSION_ERR;
    for(srv_i = 0; srv_i < tac_srv_no; srv_i++) {
        tac_fd = tac_connect_single(tac_srv[srv_i].addr, tac_srv[srv_i].key, NULL, tac_timeout);
        if (tac_fd < 0) {
            _pam_log(LOG_WARNING, "%s: error sending %s (fd)",
                __FUNCTION__, typemsg);
            continue;
        }
        if (ctrl & PAM_TAC_DEBUG)
            syslog(LOG_DEBUG, "%s: connected with fd=%d (srv %d)", __FUNCTION__, tac_fd, srv_i);

        retval = _pam_send_account(tac_fd, type, user, tty, r_addr, cmd);
        if (retval < 0) {
            _pam_log(LOG_WARNING, "%s: error sending %s (acct)",
                __FUNCTION__, typemsg);
        } else {
            status = PAM_SUCCESS;
            if (ctrl & PAM_TAC_DEBUG)
                syslog(LOG_DEBUG, "%s: [%s] for [%s] sent", __FUNCTION__, typemsg, user);
        }
        close(tac_fd);

        if ((status == PAM_SUCCESS) && !(ctrl & PAM_TAC_ACCT)) {
            /* do not send acct start/stop packets to _all_ servers */
            break;
        }
    }

    if (type == TAC_PLUS_ACCT_FLAG_STOP) {
        signal(SIGALRM, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
        signal(SIGHUP, SIG_DFL);
    }
    return status;
}