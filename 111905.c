int pam_sm_close_session (pam_handle_t * pamh, int flags,
    int argc, const char **argv) {

    return _pam_account(pamh, argc, argv, TAC_PLUS_ACCT_FLAG_STOP, NULL);
}    /* pam_sm_close_session */