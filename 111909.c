int _pam_send_account(int tac_fd, int type, const char *user, char *tty,
    char *r_addr, char *cmd) {

    char buf[64];
    struct tac_attrib *attr;
    int retval;

    attr=(struct tac_attrib *)xcalloc(1, sizeof(struct tac_attrib));

    sprintf(buf, "%lu", (unsigned long)time(NULL));

    if (type == TAC_PLUS_ACCT_FLAG_START) {
        tac_add_attrib(&attr, "start_time", buf);
    } else if (type == TAC_PLUS_ACCT_FLAG_STOP) {
        tac_add_attrib(&attr, "stop_time", buf);
    }
    sprintf(buf, "%hu", task_id);
    tac_add_attrib(&attr, "task_id", buf);
    tac_add_attrib(&attr, "service", tac_service);
    if(tac_protocol[0] != '\0')
      tac_add_attrib(&attr, "protocol", tac_protocol);
    if (cmd != NULL) {
        tac_add_attrib(&attr, "cmd", cmd);
    }

    retval = tac_acct_send(tac_fd, type, user, tty, r_addr, attr);

    /* this is no longer needed */
    tac_free_attrib(&attr);

    if(retval < 0) {
        _pam_log (LOG_WARNING, "%s: send %s accounting failed (task %hu)",
            __FUNCTION__,
            tac_acct_flag2str(type),
            task_id);
        close(tac_fd);
        return -1;
    }

    struct areply re;
    if( tac_acct_read(tac_fd, &re) != TAC_PLUS_ACCT_STATUS_SUCCESS ) {
        _pam_log (LOG_WARNING, "%s: accounting %s failed (task %hu)",
            __FUNCTION__,
            tac_acct_flag2str(type),
            task_id);

        if(re.msg != NULL)
            free(re.msg);

        close(tac_fd);
        return -1;
    }

    if(re.msg != NULL)
        free(re.msg);

    close(tac_fd);
    return 0;
}