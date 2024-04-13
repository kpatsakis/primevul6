int ff_htmlmarkup_to_ass(void *log_ctx, AVBPrint *dst, const char *in)
{
    char *param, buffer[128], tmp[128];
    int len, tag_close, sptr = 1, line_start = 1, an = 0, end = 0;
    SrtStack stack[16];
    int closing_brace_missing = 0;

    stack[0].tag[0] = 0;
    strcpy(stack[0].param[PARAM_SIZE],  "{\\fs}");
    strcpy(stack[0].param[PARAM_COLOR], "{\\c}");
    strcpy(stack[0].param[PARAM_FACE],  "{\\fn}");

    for (; !end && *in; in++) {
        switch (*in) {
        case '\r':
            break;
        case '\n':
            if (line_start) {
                end = 1;
                break;
            }
            rstrip_spaces_buf(dst);
            av_bprintf(dst, "\\N");
            line_start = 1;
            break;
        case ' ':
            if (!line_start)
                av_bprint_chars(dst, *in, 1);
            break;
        case '{':    /* skip all {\xxx} substrings except for {\an%d}
                        and all microdvd like styles such as {Y:xxx} */
            an += scanbraces(in);
            if (!closing_brace_missing) {
                if (   (an != 1 && in[1] == '\\')
                    || (in[1] && strchr("CcFfoPSsYy", in[1]) && in[2] == ':')) {
                    char *bracep = strchr(in+2, '}');
                    if (bracep) {
                        in = bracep;
                        break;
                    } else
                        closing_brace_missing = 1;
                }
            }

            av_bprint_chars(dst, *in, 1);
            break;
        case '<':
            tag_close = in[1] == '/';
            len = 0;
            if (scantag(in+tag_close+1, buffer, &len) && len > 0) {
                const char *tagname = buffer;
                while (*tagname == ' ')
                    tagname++;
                if ((param = strchr(tagname, ' ')))
                    *param++ = 0;
                if ((!tag_close && sptr < FF_ARRAY_ELEMS(stack) && *tagname) ||
                    ( tag_close && sptr > 0 && !strcmp(stack[sptr-1].tag, tagname))) {
                    int i, j, unknown = 0;
                    in += len + tag_close;
                    if (!tag_close)
                        memset(stack+sptr, 0, sizeof(*stack));
                    if (!strcmp(tagname, "font")) {
                        if (tag_close) {
                            for (i=PARAM_NUMBER-1; i>=0; i--)
                                if (stack[sptr-1].param[i][0])
                                    for (j=sptr-2; j>=0; j--)
                                        if (stack[j].param[i][0]) {
                                            av_bprintf(dst, "%s", stack[j].param[i]);
                                            break;
                                        }
                        } else {
                            while (param) {
                                if (!strncmp(param, "size=", 5)) {
                                    unsigned font_size;
                                    param += 5 + (param[5] == '"');
                                    if (sscanf(param, "%u", &font_size) == 1) {
                                        snprintf(stack[sptr].param[PARAM_SIZE],
                                             sizeof(stack[0].param[PARAM_SIZE]),
                                             "{\\fs%u}", font_size);
                                    }
                                } else if (!strncmp(param, "color=", 6)) {
                                    param += 6 + (param[6] == '"');
                                    snprintf(stack[sptr].param[PARAM_COLOR],
                                         sizeof(stack[0].param[PARAM_COLOR]),
                                         "{\\c&H%X&}",
                                         html_color_parse(log_ctx, param));
                                } else if (!strncmp(param, "face=", 5)) {
                                    param += 5 + (param[5] == '"');
                                    len = strcspn(param,
                                                  param[-1] == '"' ? "\"" :" ");
                                    av_strlcpy(tmp, param,
                                               FFMIN(sizeof(tmp), len+1));
                                    param += len;
                                    snprintf(stack[sptr].param[PARAM_FACE],
                                             sizeof(stack[0].param[PARAM_FACE]),
                                             "{\\fn%s}", tmp);
                                }
                                if ((param = strchr(param, ' ')))
                                    param++;
                            }
                            for (i=0; i<PARAM_NUMBER; i++)
                                if (stack[sptr].param[i][0])
                                    av_bprintf(dst, "%s", stack[sptr].param[i]);
                        }
                    } else if (tagname[0] && !tagname[1] && strspn(tagname, "bisu") == 1) {
                        av_bprintf(dst, "{\\%c%d}", tagname[0], !tag_close);
                    } else {
                        unknown = 1;
                        snprintf(tmp, sizeof(tmp), "</%s>", tagname);
                    }
                    if (tag_close) {
                        sptr--;
                    } else if (unknown && !strstr(in, tmp)) {
                        in -= len + tag_close;
                        av_bprint_chars(dst, *in, 1);
                    } else
                        av_strlcpy(stack[sptr++].tag, tagname,
                                   sizeof(stack[0].tag));
                    break;
                }
            }
        default:
            av_bprint_chars(dst, *in, 1);
            break;
        }
        if (*in != ' ' && *in != '\r' && *in != '\n')
            line_start = 0;
    }

    if (!av_bprint_is_complete(dst))
        return AVERROR(ENOMEM);

    while (dst->len >= 2 && !strncmp(&dst->str[dst->len - 2], "\\N", 2))
        dst->len -= 2;
    dst->str[dst->len] = 0;
    rstrip_spaces_buf(dst);

    return 0;
}