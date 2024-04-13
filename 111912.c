int pam_sm_authenticate (pam_handle_t * pamh, int flags,
    int argc, const char **argv) {

    int ctrl, retval;
    char *user;
    char *pass;
    char *tty;
    char *r_addr;
    int srv_i;
    int tac_fd, status, msg, communicating;

    user = pass = tty = r_addr = NULL;

    ctrl = _pam_parse(argc, argv);

    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: called (pam_tacplus v%u.%u.%u)",
            __FUNCTION__, PAM_TAC_VMAJ, PAM_TAC_VMIN, PAM_TAC_VPAT);

    if ((user = _pam_get_user(pamh)) == NULL)
        return PAM_USER_UNKNOWN;

    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: user [%s] obtained", __FUNCTION__, user);

    retval = tacacs_get_password (pamh, flags, ctrl, &pass);
    if (retval != PAM_SUCCESS || pass == NULL || *pass == '\0') {
        _pam_log(LOG_ERR, "unable to obtain password");
        free(pass);
        return PAM_CRED_INSUFFICIENT;
    }

    retval = pam_set_item (pamh, PAM_AUTHTOK, pass);
    if (retval != PAM_SUCCESS) {
        _pam_log(LOG_ERR, "unable to set password");
        free(pass);
        return PAM_CRED_INSUFFICIENT;
    }

    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: password obtained", __FUNCTION__);

    tty = _pam_get_terminal(pamh);
    if (!strncmp(tty, "/dev/", 5))
        tty += 5;
    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: tty [%s] obtained", __FUNCTION__, tty);

    r_addr = _pam_get_rhost(pamh);
    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: rhost [%s] obtained", __FUNCTION__, r_addr);

    status = PAM_AUTHINFO_UNAVAIL;
    for (srv_i = 0; srv_i < tac_srv_no; srv_i++) {
        if (ctrl & PAM_TAC_DEBUG)
            syslog(LOG_DEBUG, "%s: trying srv %d", __FUNCTION__, srv_i );

        tac_fd = tac_connect_single(tac_srv[srv_i].addr, tac_srv[srv_i].key, NULL, tac_timeout);
        if (tac_fd < 0) {
            _pam_log(LOG_ERR, "connection failed srv %d: %m", srv_i);
            continue;
        }
        if (tac_authen_send(tac_fd, user, pass, tty, r_addr, TAC_PLUS_AUTHEN_LOGIN) < 0) {
            close(tac_fd);
            _pam_log(LOG_ERR, "error sending auth req to TACACS+ server");
            continue;
        }
        communicating = 1;
        while (communicating) {
            struct areply re = { .attr = NULL, .msg = NULL, status = 0, flags = 0 };
            struct pam_message conv_msg = { .msg_style = 0, .msg = NULL };
            struct pam_response *resp = NULL;

            msg = tac_authen_read(tac_fd, &re);

            if (NULL != re.msg) {
                conv_msg.msg = re.msg;
            }

            /* talk the protocol */
            switch (msg) {
                case TAC_PLUS_AUTHEN_STATUS_PASS:
                    /* success */
                    if (ctrl & PAM_TAC_DEBUG)
                        syslog(LOG_DEBUG, "tacacs status: TAC_PLUS_AUTHEN_STATUS_PASS");
  
                    if (NULL != conv_msg.msg) {
                        int retval = -1;

                        conv_msg.msg_style = PAM_TEXT_INFO;
                        retval = converse(pamh, 1, &conv_msg, &resp);
                        if (PAM_SUCCESS == retval) {
                            if (PAM_TAC_DEBUG == (ctrl & PAM_TAC_DEBUG))
                                syslog(LOG_DEBUG, "send msg=\"%s\"", conv_msg.msg);
                        }
                        else {
                            _pam_log(LOG_WARNING, "%s: error sending msg=\"%s\", retval=%d",
                                     __FUNCTION__, conv_msg.msg, retval);
                        }

                    }
                    status = PAM_SUCCESS;
                    communicating = 0;
                    active_server.addr = tac_srv[srv_i].addr;
                    active_server.key = tac_srv[srv_i].key;

                    if (ctrl & PAM_TAC_DEBUG)
                        syslog(LOG_DEBUG, "%s: active srv %d", __FUNCTION__, srv_i);

                    break;

                case TAC_PLUS_AUTHEN_STATUS_FAIL:
                    if (ctrl & PAM_TAC_DEBUG)
                        syslog(LOG_DEBUG, "tacacs status: TAC_PLUS_AUTHEN_STATUS_FAIL");

                    if (NULL != conv_msg.msg) {
                        int retval = -1;

                        conv_msg.msg_style = PAM_ERROR_MSG;
                        retval = converse(pamh, 1, &conv_msg, &resp);
                        if (PAM_SUCCESS == retval) {
                            if (PAM_TAC_DEBUG == (ctrl & PAM_TAC_DEBUG))
                                syslog(LOG_DEBUG, "send msg=\"%s\"", conv_msg.msg);
                        }
                        else {
                            _pam_log(LOG_WARNING, "%s: error sending msg=\"%s\", retval=%d",
                                     __FUNCTION__, conv_msg.msg, retval);
                        }

                    }
                    status = PAM_AUTH_ERR;
                    communicating = 0;

                    _pam_log(LOG_ERR, "auth failed: %d", msg);

                    break;

                case TAC_PLUS_AUTHEN_STATUS_GETDATA:
                    if (PAM_TAC_DEBUG == (ctrl & PAM_TAC_DEBUG))
                        syslog(LOG_DEBUG, "tacacs status: TAC_PLUS_AUTHEN_STATUS_GETDATA");

                    if (NULL != conv_msg.msg) {
                        int retval = -1;
                        int echo_off = (0x1 == (re.flags & 0x1));
                        
                        conv_msg.msg_style = echo_off ? PAM_PROMPT_ECHO_OFF : PAM_PROMPT_ECHO_ON;
                        retval = converse(pamh, 1, &conv_msg, &resp);
                        if (PAM_SUCCESS == retval) {
                            if (PAM_TAC_DEBUG == (ctrl & PAM_TAC_DEBUG)) 
                                syslog(LOG_DEBUG, "sent msg=\"%s\", resp=\"%s\"",
                                       conv_msg.msg, resp->resp);

                            if (PAM_TAC_DEBUG == (ctrl & PAM_TAC_DEBUG))
                                syslog(LOG_DEBUG, "%s: calling tac_cont_send", __FUNCTION__);

                            if (0 > tac_cont_send_seq(tac_fd, resp->resp, re.seq_no + 1)) {
                                _pam_log(LOG_ERR, "error sending continue req to TACACS+ server");
                                status = PAM_AUTH_ERR;
                                communicating = 0;
                            }
                        }
                        else {
                            _pam_log(LOG_WARNING, "%s: error sending msg=\"%s\", retval=%d (%s)",
                                     __FUNCTION__, conv_msg.msg, retval, pam_strerror(pamh, retval));
                            status = PAM_AUTH_ERR;
                            communicating = 0;
                        }
                    }
                    else { 
                        syslog(LOG_ERR, "GETDATA response with no message, returning PAM_AUTH_ERR");

                        status = PAM_AUTH_ERR;
                        communicating = 0;
                    }

                    break;

                case TAC_PLUS_AUTHEN_STATUS_GETUSER:
                    /* not implemented */
                    if (ctrl & PAM_TAC_DEBUG)
                        syslog(LOG_DEBUG, "tacacs status: TAC_PLUS_AUTHEN_STATUS_GETUSER");

                    communicating = 0;
                    break;

                case TAC_PLUS_AUTHEN_STATUS_GETPASS:
                    if (ctrl & PAM_TAC_DEBUG)
                        syslog(LOG_DEBUG, "tacacs status: TAC_PLUS_AUTHEN_STATUS_GETPASS");

                    if (ctrl & PAM_TAC_DEBUG)
                        syslog(LOG_DEBUG, "%s: tac_cont_send called", __FUNCTION__);

                    if (tac_cont_send(tac_fd, pass) < 0) {
                        _pam_log (LOG_ERR, "error sending continue req to TACACS+ server");
                        communicating = 0;
                    }
                    /* continue the while loop; go read tac response */
                    break;

                case TAC_PLUS_AUTHEN_STATUS_RESTART:
                    /* try it again */
                    if (ctrl & PAM_TAC_DEBUG)
                        syslog(LOG_DEBUG, "tacacs status: TAC_PLUS_AUTHEN_STATUS_RESTART");

                    /*
                     * not implemented
                     * WdJ: I *think* you can just do tac_authen_send(user, pass) again
                     *      but I'm not sure
                     */
                    communicating = 0;
                    break;

                case TAC_PLUS_AUTHEN_STATUS_ERROR:
                    /* server has problems */
                    if (ctrl & PAM_TAC_DEBUG)
                        syslog(LOG_DEBUG, "tacacs status: TAC_PLUS_AUTHEN_STATUS_ERROR");

                    communicating = 0;
                    break;

                case TAC_PLUS_AUTHEN_STATUS_FOLLOW:
                    /* server tells to try a different server address */
                    /* not implemented */
                    if (ctrl & PAM_TAC_DEBUG)
                        syslog(LOG_DEBUG, "tacacs status: TAC_PLUS_AUTHEN_STATUS_FOLLOW");

                    communicating = 0;
                    break;

                default:
                    if (msg < 0) {
                        /* connection error */
                        communicating = 0;
                        if (ctrl & PAM_TAC_DEBUG)
                            syslog(LOG_DEBUG, "error communicating with tacacs server");
                        break;
                    }

                    /* unknown response code */
                    communicating = 0;
                    if (ctrl & PAM_TAC_DEBUG)
                        syslog(LOG_DEBUG, "tacacs status: unknown response 0x%02x", msg);
            }

            if (NULL != resp) {
                free(resp->resp);
                free(resp);
            }
                
            free(re.msg);

        }    /* end while(communicating) */
        close(tac_fd);

        if (status == PAM_SUCCESS || status == PAM_AUTH_ERR)
            break;
    }
    if (status != PAM_SUCCESS && status != PAM_AUTH_ERR)
        _pam_log(LOG_ERR, "no more servers to connect");

    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: exit with pam status: %d", __FUNCTION__, status);

    if (NULL != pass) {
        bzero(pass, strlen (pass));
        free(pass);
        pass = NULL;
    }

    return status;
}    /* pam_sm_authenticate */