do_cmd(void)
{
  char *s = term.cmd_buf;
  s[term.cmd_len] = 0;
  //printf("OSC %d <%s> %s\n", term.cmd_num, s, term.state == CMD_ESCAPE ? "ST" : "BEL");
  char * osc_fini = term.state == CMD_ESCAPE ? "\e\\" : "\a";

  if (*cfg.suppress_osc && contains(cfg.suppress_osc, term.cmd_num))
    // skip suppressed OSC command
    return;

  switch (term.cmd_num) {
    when 0 or 2: win_set_title(s);  // ignore icon title
    when 4:   do_colour_osc(true, 4, false);
    when 5:   do_colour_osc(true, 5, false);
    when 6 or 106: {
      int col, on;
      if (sscanf(term.cmd_buf, "%u;%u", &col, &on) == 2)
        if (col == 0)
          term.enable_bold_colour = on;
    }
    when 104: do_colour_osc(true, 4, true);
    when 105: do_colour_osc(true, 5, true);
    when 10:  do_colour_osc(false, FG_COLOUR_I, false);
    when 11:  if (strchr("*_%=", *term.cmd_buf)) {
                wchar * bn = cs__mbstowcs(term.cmd_buf);
                wstrset(&cfg.background, bn);
                free(bn);
                if (*term.cmd_buf == '%')
                  scale_to_image_ratio();
                win_invalidate_all(true);
              }
              else
                do_colour_osc(false, BG_COLOUR_I, false);
    when 12:  do_colour_osc(false, CURSOR_COLOUR_I, false);
    when 17:  do_colour_osc(false, SEL_COLOUR_I, false);
    when 19:  do_colour_osc(false, SEL_TEXT_COLOUR_I, false);
    when 15:  do_colour_osc(false, TEK_FG_COLOUR_I, false);
    when 16:  do_colour_osc(false, TEK_BG_COLOUR_I, false);
    when 18:  do_colour_osc(false, TEK_CURSOR_COLOUR_I, false);
    when 110: do_colour_osc(false, FG_COLOUR_I, true);
    when 111: do_colour_osc(false, BG_COLOUR_I, true);
    when 112: do_colour_osc(false, CURSOR_COLOUR_I, true);
    when 117: do_colour_osc(false, SEL_COLOUR_I, true);
    when 119: do_colour_osc(false, SEL_TEXT_COLOUR_I, true);
    when 115: do_colour_osc(false, TEK_FG_COLOUR_I, true);
    when 116: do_colour_osc(false, TEK_BG_COLOUR_I, true);
    when 118: do_colour_osc(false, TEK_CURSOR_COLOUR_I, true);
    when 7:  // Set working directory (from Mac Terminal) for Alt+F2
      // extract dirname from file://host/path scheme
      if (!strncmp(s, "file:", 5))
        s += 5;
      if (!strncmp(s, "//localhost/", 12))
        s += 11;
      else if (!strncmp(s, "///", 3))
        s += 2;
      if (!*s || *s == '/')
        child_set_fork_dir(s);
    when 701:  // Set/get locale (from urxvt).
      if (!strcmp(s, "?"))
        child_printf("\e]701;%s%s", cs_get_locale(), osc_fini);
      else
        cs_set_locale(s);
    when 7721:  // Copy window title to clipboard.
      win_copy_title();
    when 7773: {  // Change icon.
      uint icon_index = 0;
      char *comma = strrchr(s, ',');
      if (comma) {
        char *start = comma + 1, *end;
        icon_index = strtoul(start, &end, 0);
        if (start != end && !*end)
          *comma = 0;
        else
          icon_index = 0;
      }
      win_set_icon(s, icon_index);
    }
    when 7770:  // Change font size.
      if (!strcmp(s, "?"))
        child_printf("\e]7770;%u%s", win_get_font_size(), osc_fini);
      else {
        char *end;
        int i = strtol(s, &end, 10);
        if (*end)
          ; // Ignore if parameter contains unexpected characters
        else if (*s == '+' || *s == '-')
          win_zoom_font(i, false);
        else
          win_set_font_size(i, false);
      }
    when 7777:  // Change font and window size.
      if (!strcmp(s, "?"))
        child_printf("\e]7777;%u%s", win_get_font_size(), osc_fini);
      else {
        char *end;
        int i = strtol(s, &end, 10);
        if (*end)
          ; // Ignore if parameter contains unexpected characters
        else if (*s == '+' || *s == '-')
          win_zoom_font(i, true);
        else
          win_set_font_size(i, true);
      }
    when 7771: {  // Enquire about font support for a list of characters
      if (*s++ != '?')
        return;
      wchar wcs[term.cmd_len];
      uint n = 0;
      while (*s) {
        if (*s++ != ';')
          return;
        wcs[n++] = strtoul(s, &s, 10);
      }
      win_check_glyphs(wcs, n, term.curs.attr.attr);
      s = term.cmd_buf;
      for (size_t i = 0; i < n; i++) {
        *s++ = ';';
        if (wcs[i])
          s += sprintf(s, "%u", wcs[i]);
      }
      *s = 0;
      child_printf("\e]7771;!%s%s", term.cmd_buf, osc_fini);
    }
    when 77119: {  // Indic and Extra characters wide handling
      int what = atoi(s);
      term.wide_indic = false;
      term.wide_extra = false;
      if (what & 1)
        term.wide_indic = true;
      if (what & 2)
        term.wide_extra = true;
    }
    when 52: do_clipboard();
    when 50:
      if (tek_mode) {
        tek_set_font(cs__mbstowcs(s));
        tek_init(false, cfg.tek_glow);
      }
      else {
        uint ff = (term.curs.attr.attr & FONTFAM_MASK) >> ATTR_FONTFAM_SHIFT;
        if (!strcmp(s, "?")) {
          char * fn = cs__wcstombs(win_get_font(ff) ?: W(""));
          child_printf("\e]50;%s%s", fn, osc_fini);
          free(fn);
        }
        else {
          if (ff < lengthof(cfg.fontfams) - 1) {
            wstring wfont = cs__mbstowcs(s);  // let this leak...
            win_change_font(ff, wfont);
          }
        }
      }
    when 8: {  // hyperlink attribute
      char * link = s;
      char * url = strchr(s, ';');
      if (url++ && *url) {
        term.curs.attr.link = putlink(link);
      }
      else
        term.curs.attr.link = -1;
    }
    when 1337: {  // iTerm2 image protocol
                  // https://www.iterm2.com/documentation-images.html
      char * payload = strchr(s, ':');
      if (payload) {
        *payload = 0;
        payload++;
      }

      // verify protocol
      if (0 == strncmp("File=", s, 5))
        s += 5;
      else
        return;

      char * name = 0;
      int width = 0;
      int height = 0;
      int pixelwidth = 0;
      int pixelheight = 0;
      bool pAR = true;
      int crop_x = 0;
      int crop_y = 0;
      int crop_width = 0;
      int crop_height = 0;

      // process parameters
      while (s && *s) {
        char * nxt = strchr(s, ';');
        if (nxt) {
          *nxt = 0;
          nxt++;
        }
        char * sval = strchr(s, '=');
        if (sval) {
          *sval = 0;
          sval++;
        }
        else
          sval = "";
        int val = atoi(sval);
        char * suf = sval;
        while (isdigit((uchar)*suf))
          suf++;
        bool pix = 0 == strcmp("px", suf);
        bool per = 0 == strcmp("%", suf);
        //printf("<%s>=<%s>%d<%s>\n", s, sval, val, suf);

        if (0 == strcmp("name", s))
          name = s;  // can serve as cache id
        else if (0 == strcmp("width", s)) {
          if (pix) {
            pixelwidth = val;
            width = (val - 1) / cell_width + 1;
          }
          else if (per) {
            width = term.cols * val / 100;
            pixelwidth = width * cell_width;
          }
          else {
            width = val;
            pixelwidth = val * cell_width;
          }
        }
        else if (0 == strcmp("height", s)) {
          if (pix) {
            pixelheight = val;
            height = (val - 1) / cell_height + 1;
          }
          else if (per) {
            height = term.rows * val / 100;
            pixelheight = height * cell_height;
          }
          else {
            height = val;
            pixelheight = val * cell_height;
          }
        }
        else if (0 == strcmp("preserveAspectRatio", s)) {
          pAR = val;
        }
        else if (0 == strcmp("cropX", s) || 0 == strcmp("cropLeft", s)) {
          if (pix) {
            crop_x = val;
          }
        }
        else if (0 == strcmp("cropY", s) || 0 == strcmp("cropTop", s)) {
          if (pix) {
            crop_y = val;
          }
        }
        else if (0 == strcmp("cropWidth", s)) {
          if (pix) {
            crop_width = val;
          }
        }
        else if (0 == strcmp("cropHeight", s)) {
          if (pix) {
            crop_height = val;
          }
        }
        else if (0 == strcmp("cropRight", s)) {
          if (pix) {
            crop_width = - val;
          }
        }
        else if (0 == strcmp("cropBottom", s)) {
          if (pix) {
            crop_height = - val;
          }
        }

        s = nxt;
      }

      if (payload) {
#ifdef strip_newlines
#warning not applicable as preprocessing OSC would not pass it here
        char * from = strpbrk(payload, "\r\n");
        if (from) {  // strip new lines
          char * to = from;
          while (*from) {
            if (*from >= ' ')
              *to++ = *from;
            from++;
          }
          *to = 0;
        }
#endif
        int len = strlen(payload);
        int datalen = len - (len / 4);
        void * data = malloc(datalen);
        if (!data)
          return;
        datalen = base64_decode_clip(payload, len, data, datalen);
        if (datalen > 0) {
          // OK
          imglist * img;
          short left = term.curs.x;
          short top = term.virtuallines + term.curs.y;
          if (winimg_new(&img, name, data, datalen, left, top, width, height, pixelwidth, pixelheight, pAR, crop_x, crop_y, crop_width, crop_height)) {
            fill_image_space(img);

            if (term.imgs.first == NULL) {
              term.imgs.first = term.imgs.last = img;
            } else {
              // append image to list
              img->prev = term.imgs.last;
              term.imgs.last->next = img;
              term.imgs.last = img;
            }
          }
          else
            free(data);
        }
        else
          free(data);
      }
    }
    when 9: {
typedef struct {
  char * p;
  int v;
} paramap;
      int scanenum(char * s, int * _i, paramap * p, bool donum) {
        char * sep = strchr(s, ';');
        int len = sep ? (uint)(sep - s) : strlen(s);
        while (p->p) {
          if (0 == strncasecmp(s, p->p, len)) {
            *_i = p->v;
            return len;
          }
          p++;
        }
        if (donum) {
          // fallback scan for number
          int numlen = sscanf(s, "%d", _i);
          if (numlen && numlen == len)
            return numlen;
        }
        // not found
        return 0;
      }

      int cmd;
      int len = scanenum(s, &cmd,
                         (paramap[]){{"4", 4}, {"progress", 4}, {0, 0}},
                         false);
      if (!len || cmd != 4)
        return;
      s += len;

      if (!*s)
        return;
      s++;
      int state;
      len = scanenum(s, &state,
                     (paramap[]){
                                 {"off", 0},
                                 {"green", 1},
                                 {"yellow", 2},
                                 {"red", 3},
                                 {"busy", 8},
                                 {"0", 0},
                                 {"1", 1},
                                 {"4", 2},
                                 {"2", 3},
                                 {"3", 8},
                                 {0, 0}},
                     false);
      if (!len)
        return;
      s += len;

      int percent = -1;
      if (*s) {
        s++;
        sscanf(s, "%d", &percent);
      }

      set_taskbar_progress(state, percent);
    }
  }
}