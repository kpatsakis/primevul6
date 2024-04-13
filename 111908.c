int pam_sm_open_session (pam_handle_t * pamh, int flags,
    int argc, const char **argv) {

    task_id=(short int) magic();
    return _pam_account(pamh, argc, argv, TAC_PLUS_ACCT_FLAG_START, NULL);
}    /* pam_sm_open_session */