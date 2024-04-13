int pam_sm_setcred (pam_handle_t * pamh, int flags,
    int argc, const char **argv) {

    int ctrl = _pam_parse (argc, argv);

    if (ctrl & PAM_TAC_DEBUG)
        syslog (LOG_DEBUG, "%s: called (pam_tacplus v%u.%u.%u)"
            , __FUNCTION__, PAM_TAC_VMAJ, PAM_TAC_VMIN, PAM_TAC_VPAT);

    return PAM_SUCCESS;
}    /* pam_sm_setcred */