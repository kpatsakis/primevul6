static void log_func(enum fuse_log_level level, const char *fmt, va_list ap)
{
    g_autofree char *localfmt = NULL;

    if (current_log_level < level) {
        return;
    }

    if (current_log_level == FUSE_LOG_DEBUG) {
        if (!use_syslog) {
            localfmt = g_strdup_printf("[%" PRId64 "] [ID: %08ld] %s",
                                       get_clock(), syscall(__NR_gettid), fmt);
        } else {
            localfmt = g_strdup_printf("[ID: %08ld] %s", syscall(__NR_gettid),
                                       fmt);
        }
        fmt = localfmt;
    }

    if (use_syslog) {
        int priority = LOG_ERR;
        switch (level) {
        case FUSE_LOG_EMERG:
            priority = LOG_EMERG;
            break;
        case FUSE_LOG_ALERT:
            priority = LOG_ALERT;
            break;
        case FUSE_LOG_CRIT:
            priority = LOG_CRIT;
            break;
        case FUSE_LOG_ERR:
            priority = LOG_ERR;
            break;
        case FUSE_LOG_WARNING:
            priority = LOG_WARNING;
            break;
        case FUSE_LOG_NOTICE:
            priority = LOG_NOTICE;
            break;
        case FUSE_LOG_INFO:
            priority = LOG_INFO;
            break;
        case FUSE_LOG_DEBUG:
            priority = LOG_DEBUG;
            break;
        }
        vsyslog(priority, fmt, ap);
    } else {
        vfprintf(stderr, fmt, ap);
    }
}