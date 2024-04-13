do_dcs(void)
{
  // Implemented:
  // DECRQSS (Request Status String)
  // DECAUPSS (Assign User-Preferred Supplemental Set)
  // DECSIXEL
  // No DECUDK (User-Defined Keys) or xterm termcap/terminfo data.

  char *s = term.cmd_buf;
  if (!term.cmd_len)
    *s = 0;

  switch (term.dcs_cmd) {

  when CPAIR('!', 'u'):  // DECAUPSS
    if (term.state == DCS_ESCAPE) {
      ushort nrc_code = 0;
      if (term.cmd_len == 1)
        nrc_code = *s;
      else if (term.cmd_len == 2)
        nrc_code = CPAIR(s[0], s[1]);
      term_cset cs = lookup_cset(nrc_code, 7, false);
      if (cs) {
        term.curs.decsupp = cs;
        term_update_cs();
        return;
      }
    }

  when 'q': {
   sixel_state_t * st = (sixel_state_t *)term.imgs.parser_state;
   int status = -1;

   switch (term.state) {
    when DCS_PASSTHROUGH:
      if (!st)
        return;
      status = sixel_parser_parse(st, (unsigned char *)s, term.cmd_len);
      if (status < 0) {
        sixel_parser_deinit(st);
        //printf("free state 1 %p\n", term.imgs.parser_state);
        free(term.imgs.parser_state);
        term.imgs.parser_state = NULL;
        term.state = DCS_IGNORE;
        return;
      }

    when DCS_ESCAPE:
      if (!st)
        return;
      status = sixel_parser_parse(st, (unsigned char *)s, term.cmd_len);
      if (status < 0) {
        sixel_parser_deinit(st);
        //printf("free state 2 %p\n", term.imgs.parser_state);
        free(term.imgs.parser_state);
        term.imgs.parser_state = NULL;
        return;
      }

      unsigned char * pixels = sixel_parser_finalize(st);
      //printf("sixel_parser_finalize %p\n", pixels);
      sixel_parser_deinit(st);
      if (!pixels) {
        //printf("free state 3 %p\n", term.imgs.parser_state);
        free(term.imgs.parser_state);
        term.imgs.parser_state = NULL;
        return;
      }

      short left = term.curs.x;
      short top = term.virtuallines + (term.sixel_display ? 0: term.curs.y);
      int width = (st->image.width -1 ) / st->grid_width + 1;
      int height = (st->image.height -1 ) / st->grid_height + 1;
      int pixelwidth = st->image.width;
      int pixelheight = st->image.height;
      //printf("w %d/%d %d h %d/%d %d\n", pixelwidth, st->grid_width, width, pixelheight, st->grid_height, height);

      imglist * img;
      if (!winimg_new(&img, 0, pixels, 0, left, top, width, height, pixelwidth, pixelheight, false, 0, 0, 0, 0)) {
        free(pixels);
        sixel_parser_deinit(st);
        //printf("free state 4 %p\n", term.imgs.parser_state);
        free(term.imgs.parser_state);
        term.imgs.parser_state = NULL;
        return;
      }
      img->cwidth = st->max_x;
      img->cheight = st->max_y;

      fill_image_space(img);

      // add image to image list;
      // replace previous for optimisation in some cases
      if (term.imgs.first == NULL) {
        term.imgs.first = term.imgs.last = img;
      } else {
        // try some optimization: replace existing images if overwritten
#ifdef debug_sixel_list
        printf("do_dcs checking imglist\n");
#endif
#ifdef replace_images
#warning do not replace images in the list anymore
        // with new flicker-reduce strategy of rendering overlapped images,
        // new images should always be added to the end of the queue;
        // completely overlayed images should be collected for removal 
        // during the rendering loop (winimgs_paint),
        // or latest when they are scrolled out of the scrollback buffer
        for (imglist * cur = term.imgs.first; cur; cur = cur->next) {
          if (cur->pixelwidth == cur->width * st->grid_width &&
              cur->pixelheight == cur->height * st->grid_height)
          {
            // if same size, replace
            if (img->top == cur->top && img->left == cur->left &&
                img->width == cur->width &&
                img->height == cur->height)
            {
#ifdef debug_sixel_list
              printf("img replace\n");
#endif
              memcpy(cur->pixels, img->pixels, img->pixelwidth * img->pixelheight * 4);
              cur->imgi = img->imgi;
              winimg_destroy(img);
              return;
            }
            // if new image within area of previous image, ...
#ifdef handle_overlay_images
#warning this creates some crash conditions...
            if (img->top >= cur->top && img->left >= cur->left &&
                img->left + img->width <= cur->left + cur->width &&
                img->top + img->height <= cur->top + cur->height)
            {
              // inject new img into old structure;
              // copy img data in stripes, for unknown reason
              for (y = 0; y < img->pixelheight; ++y) {
                memcpy(cur->pixels +
                         ((img->top - cur->top) * st->grid_height + y) * cur->pixelwidth * 4 +
                         (img->left - cur->left) * st->grid_width * 4,
                       img->pixels + y * img->pixelwidth * 4,
                       img->pixelwidth * 4);
              }
              cur->imgi = img->imgi;
              winimg_destroy(img);
              return;
            }
#endif
          }
        }
#endif
        // append image to list
        img->prev = term.imgs.last;
        term.imgs.last->next = img;
        term.imgs.last = img;
      }

    otherwise: {
      /* parser status initialization */
      colour fg = win_get_colour(FG_COLOUR_I);
      colour bg = win_get_colour(BG_COLOUR_I);
      if (!st) {
        st = term.imgs.parser_state = calloc(1, sizeof(sixel_state_t));
        //printf("alloc state %d -> %p\n", (int)sizeof(sixel_state_t), st);
        sixel_parser_set_default_color(st);
      }
      status = sixel_parser_init(st, fg, bg, term.private_color_registers);
      if (status < 0)
        return;
    }
   }
  }

  when CPAIR('$', 'q'):
    switch (term.state) {
    when DCS_ESCAPE: {     // DECRQSS
      cattr attr = term.curs.attr;
      if (!strcmp(s, "m")) { // SGR
        char buf[90], *p = buf;
        p += sprintf(p, "\eP1$r0");

        if (attr.attr & ATTR_BOLD)
          p += sprintf(p, ";1");
        if (attr.attr & ATTR_DIM)
          p += sprintf(p, ";2");
        if (attr.attr & ATTR_SHADOW)
          p += sprintf(p, ";1:2");
        if (attr.attr & ATTR_ITALIC)
          p += sprintf(p, ";3");

        if (attr.attr & ATTR_BROKENUND)
          if (attr.attr & ATTR_DOUBLYUND)
            p += sprintf(p, ";4:5");
          else
            p += sprintf(p, ";4:4");
        else if ((attr.attr & UNDER_MASK) == ATTR_CURLYUND)
          p += sprintf(p, ";4:3");
        else if (attr.attr & ATTR_UNDER)
          p += sprintf(p, ";4");

        if (attr.attr & ATTR_BLINK)
          p += sprintf(p, ";5");
        if (attr.attr & ATTR_BLINK2)
          p += sprintf(p, ";6");
        if (attr.attr & ATTR_REVERSE)
          p += sprintf(p, ";7");
        if (attr.attr & ATTR_INVISIBLE)
          p += sprintf(p, ";8");
        if (attr.attr & ATTR_OVERSTRIKE)
          p += sprintf(p, ";8:7");
        if (attr.attr & ATTR_STRIKEOUT)
          p += sprintf(p, ";9");
        if ((attr.attr & UNDER_MASK) == ATTR_DOUBLYUND)
          p += sprintf(p, ";21");
        if (attr.attr & ATTR_FRAMED)
          p += sprintf(p, ";51;52");
        if (attr.attr & ATTR_OVERL)
          p += sprintf(p, ";53");
        if (attr.attr & ATTR_SUPERSCR)
          p += sprintf(p, ";73");
        if (attr.attr & ATTR_SUBSCR)
          p += sprintf(p, ";74");

        if (term.curs.oem_acs)
          p += sprintf(p, ";%u", 10 + term.curs.oem_acs);
        else {
          uint ff = (attr.attr & FONTFAM_MASK) >> ATTR_FONTFAM_SHIFT;
          if (ff)
            p += sprintf(p, ";%u", 10 + ff);
        }

        uint fg = (attr.attr & ATTR_FGMASK) >> ATTR_FGSHIFT;
        if (fg != FG_COLOUR_I) {
          if (fg >= TRUE_COLOUR)
            //p += sprintf(p, ";38;2;%u;%u;%u", attr.truefg & 0xFF, 
            //             (attr.truefg >> 8) & 0xFF, (attr.truefg >> 16) & 0xFF);
            p += sprintf(p, ";38:2::%u:%u:%u", attr.truefg & 0xFF, 
                         (attr.truefg >> 8) & 0xFF, (attr.truefg >> 16) & 0xFF);
          else if (fg < 16)
            p += sprintf(p, ";%u", (fg < 8 ? 30 : 90) + (fg & 7));
          else
            //p += sprintf(p, ";38;5;%u", fg);
            p += sprintf(p, ";38:5:%u", fg);
        }

        uint bg = (attr.attr & ATTR_BGMASK) >> ATTR_BGSHIFT;
        if (bg != BG_COLOUR_I) {
          if (bg >= TRUE_COLOUR)
            //p += sprintf(p, ";48;2;%u;%u;%u", attr.truebg & 0xFF, 
            //             (attr.truebg >> 8) & 0xFF, (attr.truebg >> 16) & 0xFF);
            p += sprintf(p, ";48:2::%u:%u:%u", attr.truebg & 0xFF, 
                         (attr.truebg >> 8) & 0xFF, (attr.truebg >> 16) & 0xFF);
          else if (bg < 16)
            p += sprintf(p, ";%u", (bg < 8 ? 40 : 100) + (bg & 7));
          else
            //p += sprintf(p, ";48;5;%u", bg);
            p += sprintf(p, ";48:5:%u", bg);
        }

        if (attr.attr & ATTR_ULCOLOUR) {
          p += sprintf(p, ";58:2::%u:%u:%u", attr.ulcolr & 0xFF, 
                       (attr.ulcolr >> 8) & 0xFF, (attr.ulcolr >> 16) & 0xFF);
        }

        p += sprintf(p, "m\e\\");  // m for SGR, followed by ST

        child_write(buf, p - buf);
      } else if (!strcmp(s, "r")) {  // DECSTBM (scrolling region margins)
        child_printf("\eP1$r%u;%ur\e\\", term.marg_top + 1, term.marg_bot + 1);
      } else if (!strcmp(s, "s")) {  // DECSLRM (left and right margins)
        child_printf("\eP1$r%u;%us\e\\", term.marg_left + 1, term.marg_right + 1);
      } else if (!strcmp(s, "\"p")) {  // DECSCL (conformance level)
        child_printf("\eP1$r%u;%u\"p\e\\", 65, 1);  // report as VT500 S7C1T
      } else if (!strcmp(s, "\"q")) {  // DECSCA (protection attribute)
        child_printf("\eP1$r%u\"q\e\\", (attr.attr & ATTR_PROTECTED) != 0);
      } else if (!strcmp(s, " q")) {  // DECSCUSR (cursor style)
        child_printf("\eP1$r%u q\e\\", 
                     (term.cursor_type >= 0 ? term.cursor_type * 2 : 0) + 1
                     + !(term.cursor_blinks & 1));
      } else if (!strcmp(s, "t") && term.rows >= 24) {  // DECSLPP (lines)
        child_printf("\eP1$r%ut\e\\", term.rows);
      } else if (!strcmp(s, "$|")) {  // DECSCPP (columns)
        child_printf("\eP1$r%u$|\e\\", term.cols);
      } else if (!strcmp(s, "*|")) {  // DECSNLS (lines)
        child_printf("\eP1$r%u*|\e\\", term.rows);
      } else {
        child_printf("\eP0$r%s\e\\", s);
      }
    }
    otherwise:
      return;
    }

  }
}