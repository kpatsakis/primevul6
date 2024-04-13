static void process_lru_command(conn *c, token_t *tokens, const size_t ntokens) {
    uint32_t pct_hot;
    uint32_t pct_warm;
    uint32_t hot_age;
    double factor;

    set_noreply_maybe(c, tokens, ntokens);

    if (strcmp(tokens[1].value, "tune") == 0 && ntokens >= 7) {
        if (!safe_strtoul(tokens[2].value, &pct_hot) ||
            !safe_strtoul(tokens[3].value, &pct_warm) ||
            !safe_strtoul(tokens[4].value, &hot_age) ||
            !safe_strtod(tokens[5].value, &factor)) {
            out_string(c, "ERROR");
        } else {
            if (pct_hot + pct_warm > 80) {
                out_string(c, "ERROR hot and warm pcts must not exceed 80");
            } else if (factor <= 0) {
                out_string(c, "ERROR cold age factor must be greater than 0");
            } else {
                settings.hot_lru_pct = pct_hot;
                settings.warm_lru_pct = pct_warm;
                settings.hot_max_age = hot_age;
                settings.warm_max_factor = factor;
                out_string(c, "OK");
            }
        }
    } else {
        out_string(c, "ERROR");
    }
}