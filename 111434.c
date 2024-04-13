static void strip_text(char * text) {
    char * ch_loc = NULL;

    ch_loc = strrchr(text, '\n');
    while (ch_loc != NULL) {
        *ch_loc = ' ';
        ch_loc = strrchr(text, '\n');
    }
    ch_loc = strrchr(text, '\r');
    while (ch_loc != NULL) {
        *ch_loc = ' ';
        ch_loc = strrchr(text, '\r');
    }
}