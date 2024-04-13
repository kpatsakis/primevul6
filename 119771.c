void process_karaoke_effects(ASS_Renderer *render_priv)
{
    GlyphInfo *cur, *cur2;
    GlyphInfo *s1, *e1;      // start and end of the current word
    GlyphInfo *s2;           // start of the next word
    int i;
    int timing;                 // current timing
    int tm_start, tm_end;       // timings at start and end of the current word
    int tm_current;
    double dt;
    int x;
    int x_start, x_end;

    tm_current = render_priv->time - render_priv->state.event->Start;
    timing = 0;
    s1 = s2 = 0;
    for (i = 0; i <= render_priv->text_info.length; ++i) {
        cur = render_priv->text_info.glyphs + i;
        if ((i == render_priv->text_info.length)
            || (cur->effect_type != EF_NONE)) {
            s1 = s2;
            s2 = cur;
            if (s1) {
                e1 = s2 - 1;
                tm_start = timing + s1->effect_skip_timing;
                tm_end = tm_start + s1->effect_timing;
                timing = tm_end;
                x_start = 1000000;
                x_end = -1000000;
                for (cur2 = s1; cur2 <= e1; ++cur2) {
                    x_start = FFMIN(x_start, d6_to_int(cur2->bbox.x_min + cur2->pos.x));
                    x_end = FFMAX(x_end, d6_to_int(cur2->bbox.x_max + cur2->pos.x));
                }

                dt = (tm_current - tm_start);
                if ((s1->effect_type == EF_KARAOKE)
                    || (s1->effect_type == EF_KARAOKE_KO)) {
                    if (dt >= 0)
                        x = x_end + 1;
                    else
                        x = x_start;
                } else if (s1->effect_type == EF_KARAOKE_KF) {
                    dt /= (tm_end - tm_start);
                    x = x_start + (x_end - x_start) * dt;
                } else {
                    ass_msg(render_priv->library, MSGL_ERR,
                            "Unknown effect type");
                    continue;
                }

                for (cur2 = s1; cur2 <= e1; ++cur2) {
                    cur2->effect_type = s1->effect_type;
                    cur2->effect_timing = x - d6_to_int(cur2->pos.x);
                }
                s1->effect = 1;
            }
        }
    }
}