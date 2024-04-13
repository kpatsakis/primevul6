void apply_transition_effects(ASS_Renderer *render_priv, ASS_Event *event)
{
    int v[4];
    int cnt;
    char *p = event->Effect;

    if (!p || !*p)
        return;

    cnt = 0;
    while (cnt < 4 && (p = strchr(p, ';'))) {
        v[cnt++] = atoi(++p);
    }

    if (strncmp(event->Effect, "Banner;", 7) == 0) {
        int delay;
        if (cnt < 1) {
            ass_msg(render_priv->library, MSGL_V,
                    "Error parsing effect: '%s'", event->Effect);
            return;
        }
        if (cnt >= 2 && v[1] == 0)      // right-to-left
            render_priv->state.scroll_direction = SCROLL_RL;
        else                    // left-to-right
            render_priv->state.scroll_direction = SCROLL_LR;

        delay = v[0];
        if (delay == 0)
            delay = 1;          // ?
        render_priv->state.scroll_shift =
            (render_priv->time - render_priv->state.event->Start) / delay;
        render_priv->state.evt_type = EVENT_HSCROLL;
        return;
    }

    if (strncmp(event->Effect, "Scroll up;", 10) == 0) {
        render_priv->state.scroll_direction = SCROLL_BT;
    } else if (strncmp(event->Effect, "Scroll down;", 12) == 0) {
        render_priv->state.scroll_direction = SCROLL_TB;
    } else {
        ass_msg(render_priv->library, MSGL_DBG2,
                "Unknown transition effect: '%s'", event->Effect);
        return;
    }
    // parse scroll up/down parameters
    {
        int delay;
        int y0, y1;
        if (cnt < 3) {
            ass_msg(render_priv->library, MSGL_V,
                    "Error parsing effect: '%s'", event->Effect);
            return;
        }
        delay = v[2];
        if (delay == 0)
            delay = 1;          // ?
        render_priv->state.scroll_shift =
            (render_priv->time - render_priv->state.event->Start) / delay;
        if (v[0] < v[1]) {
            y0 = v[0];
            y1 = v[1];
        } else {
            y0 = v[1];
            y1 = v[0];
        }
        if (y1 == 0)
            y1 = render_priv->track->PlayResY;  // y0=y1=0 means fullscreen scrolling
        render_priv->state.clip_y0 = y0;
        render_priv->state.clip_y1 = y1;
        render_priv->state.evt_type = EVENT_VSCROLL;
        render_priv->state.detect_collisions = 0;
    }

}