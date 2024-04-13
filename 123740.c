ass_render_event(ASS_Renderer *render_priv, ASS_Event *event,
                 EventImages *event_images)
{
    DBBox bbox;
    int MarginL, MarginR, MarginV;
    int valign;
    double device_x = 0;
    double device_y = 0;
    TextInfo *text_info = &render_priv->text_info;

    if (event->Style >= render_priv->track->n_styles) {
        ass_msg(render_priv->library, MSGL_WARN, "No style found");
        return 1;
    }
    if (!event->Text) {
        ass_msg(render_priv->library, MSGL_WARN, "Empty event");
        return 1;
    }

    free_render_context(render_priv);
    init_render_context(render_priv, event);

    if (parse_events(render_priv, event))
        return 1;

    if (text_info->length == 0) {
        // no valid symbols in the event; this can be smth like {comment}
        free_render_context(render_priv);
        return 1;
    }

    // Find shape runs and shape text
    ass_shaper_set_base_direction(render_priv->shaper,
            resolve_base_direction(render_priv->state.font_encoding));
    ass_shaper_find_runs(render_priv->shaper, render_priv, text_info->glyphs,
            text_info->length);
    if (ass_shaper_shape(render_priv->shaper, text_info) < 0) {
        ass_msg(render_priv->library, MSGL_ERR, "Failed to shape text");
        free_render_context(render_priv);
        return 1;
    }

    retrieve_glyphs(render_priv);

    preliminary_layout(render_priv);

    // depends on glyph x coordinates being monotonous, so it should be done before line wrap
    process_karaoke_effects(render_priv);

    valign = render_priv->state.alignment & 12;

    MarginL =
        (event->MarginL) ? event->MarginL : render_priv->state.style->MarginL;
    MarginR =
        (event->MarginR) ? event->MarginR : render_priv->state.style->MarginR;
    MarginV =
        (event->MarginV) ? event->MarginV : render_priv->state.style->MarginV;

    // calculate max length of a line
    double max_text_width =
        x2scr(render_priv, render_priv->track->PlayResX - MarginR) -
        x2scr(render_priv, MarginL);

    // wrap lines
    if (render_priv->state.evt_type != EVENT_HSCROLL) {
        // rearrange text in several lines
        wrap_lines_smart(render_priv, max_text_width);
    } else {
        // no breaking or wrapping, everything in a single line
        text_info->lines[0].offset = 0;
        text_info->lines[0].len = text_info->length;
        text_info->n_lines = 1;
        measure_text(render_priv);
    }

    reorder_text(render_priv);

    align_lines(render_priv, max_text_width);

    // determing text bounding box
    compute_string_bbox(text_info, &bbox);

    // determine device coordinates for text

    // x coordinate for everything except positioned events
    if (render_priv->state.evt_type == EVENT_NORMAL ||
        render_priv->state.evt_type == EVENT_VSCROLL) {
        device_x = x2scr(render_priv, MarginL);
    } else if (render_priv->state.evt_type == EVENT_HSCROLL) {
        if (render_priv->state.scroll_direction == SCROLL_RL)
            device_x =
                x2scr(render_priv,
                      render_priv->track->PlayResX -
                      render_priv->state.scroll_shift);
        else if (render_priv->state.scroll_direction == SCROLL_LR)
            device_x =
                x2scr(render_priv,
                      render_priv->state.scroll_shift) - (bbox.xMax -
                                                          bbox.xMin);
    }

    // y coordinate for everything except positioned events
    if (render_priv->state.evt_type == EVENT_NORMAL ||
        render_priv->state.evt_type == EVENT_HSCROLL) {
        if (valign == VALIGN_TOP) {     // toptitle
            device_y =
                y2scr_top(render_priv,
                          MarginV) + text_info->lines[0].asc;
        } else if (valign == VALIGN_CENTER) {   // midtitle
            double scr_y =
                y2scr(render_priv, render_priv->track->PlayResY / 2.0);
            device_y = scr_y - (bbox.yMax + bbox.yMin) / 2.0;
        } else {                // subtitle
            double line_pos = render_priv->state.explicit ?
                0 : render_priv->settings.line_position;
            double scr_top, scr_bottom, scr_y0;
            if (valign != VALIGN_SUB)
                ass_msg(render_priv->library, MSGL_V,
                       "Invalid valign, assuming 0 (subtitle)");
            scr_bottom =
                y2scr_sub(render_priv,
                          render_priv->track->PlayResY - MarginV);
            scr_top = y2scr_top(render_priv, 0); //xxx not always 0?
            device_y = scr_bottom + (scr_top - scr_bottom) * line_pos / 100.0;
            device_y -= text_info->height;
            device_y += text_info->lines[0].asc;
            // clip to top to avoid confusion if line_position is very high,
            // turning the subtitle into a toptitle
            // also, don't change behavior if line_position is not used
            scr_y0 = scr_top + text_info->lines[0].asc;
            if (device_y < scr_y0 && line_pos > 0) {
                device_y = scr_y0;
            }
        }
    } else if (render_priv->state.evt_type == EVENT_VSCROLL) {
        if (render_priv->state.scroll_direction == SCROLL_TB)
            device_y =
                y2scr(render_priv,
                      render_priv->state.clip_y0 +
                      render_priv->state.scroll_shift) - (bbox.yMax -
                                                          bbox.yMin);
        else if (render_priv->state.scroll_direction == SCROLL_BT)
            device_y =
                y2scr(render_priv,
                      render_priv->state.clip_y1 -
                      render_priv->state.scroll_shift);
    }

    // positioned events are totally different
    if (render_priv->state.evt_type == EVENT_POSITIONED) {
        double base_x = 0;
        double base_y = 0;
        get_base_point(&bbox, render_priv->state.alignment, &base_x, &base_y);
        device_x =
            x2scr_pos(render_priv, render_priv->state.pos_x) - base_x;
        device_y =
            y2scr_pos(render_priv, render_priv->state.pos_y) - base_y;
    }

    // fix clip coordinates (they depend on alignment)
    if (render_priv->state.evt_type == EVENT_NORMAL ||
        render_priv->state.evt_type == EVENT_HSCROLL ||
        render_priv->state.evt_type == EVENT_VSCROLL) {
        render_priv->state.clip_x0 =
            x2scr_scaled(render_priv, render_priv->state.clip_x0);
        render_priv->state.clip_x1 =
            x2scr_scaled(render_priv, render_priv->state.clip_x1);
        if (valign == VALIGN_TOP) {
            render_priv->state.clip_y0 =
                y2scr_top(render_priv, render_priv->state.clip_y0);
            render_priv->state.clip_y1 =
                y2scr_top(render_priv, render_priv->state.clip_y1);
        } else if (valign == VALIGN_CENTER) {
            render_priv->state.clip_y0 =
                y2scr(render_priv, render_priv->state.clip_y0);
            render_priv->state.clip_y1 =
                y2scr(render_priv, render_priv->state.clip_y1);
        } else if (valign == VALIGN_SUB) {
            render_priv->state.clip_y0 =
                y2scr_sub(render_priv, render_priv->state.clip_y0);
            render_priv->state.clip_y1 =
                y2scr_sub(render_priv, render_priv->state.clip_y1);
        }
    } else if (render_priv->state.evt_type == EVENT_POSITIONED) {
        render_priv->state.clip_x0 =
            x2scr_pos_scaled(render_priv, render_priv->state.clip_x0);
        render_priv->state.clip_x1 =
            x2scr_pos_scaled(render_priv, render_priv->state.clip_x1);
        render_priv->state.clip_y0 =
            y2scr_pos(render_priv, render_priv->state.clip_y0);
        render_priv->state.clip_y1 =
            y2scr_pos(render_priv, render_priv->state.clip_y1);
    }

    if (render_priv->state.explicit) {
        // we still need to clip against screen boundaries
        double zx = x2scr_pos_scaled(render_priv, 0);
        double zy = y2scr_pos(render_priv, 0);
        double sx = x2scr_pos_scaled(render_priv, render_priv->track->PlayResX);
        double sy = y2scr_pos(render_priv, render_priv->track->PlayResY);

        render_priv->state.clip_x0 = render_priv->state.clip_x0 < zx ? zx : render_priv->state.clip_x0;
        render_priv->state.clip_y0 = render_priv->state.clip_y0 < zy ? zy : render_priv->state.clip_y0;
        render_priv->state.clip_x1 = render_priv->state.clip_x1 > sx ? sx : render_priv->state.clip_x1;
        render_priv->state.clip_y1 = render_priv->state.clip_y1 > sy ? sy : render_priv->state.clip_y1;
    }

    calculate_rotation_params(render_priv, &bbox, device_x, device_y);

    render_and_combine_glyphs(render_priv, device_x, device_y);

    memset(event_images, 0, sizeof(*event_images));
    event_images->top = device_y - text_info->lines[0].asc;
    event_images->height = text_info->height;
    event_images->left =
        (device_x + bbox.xMin * render_priv->font_scale_x) + 0.5;
    event_images->width =
        (bbox.xMax - bbox.xMin) * render_priv->font_scale_x + 0.5;
    event_images->detect_collisions = render_priv->state.detect_collisions;
    event_images->shift_direction = (valign == VALIGN_TOP) ? 1 : -1;
    event_images->event = event;
    event_images->imgs = render_text(render_priv);

    if (render_priv->state.border_style == 4)
        add_background(render_priv, event_images);

    ass_shaper_cleanup(render_priv->shaper, text_info);
    free_render_context(render_priv);

    return 0;
}