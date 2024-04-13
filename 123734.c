ass_start_frame(ASS_Renderer *render_priv, ASS_Track *track,
                long long now)
{
    ASS_Settings *settings_priv = &render_priv->settings;

    if (!render_priv->settings.frame_width
        && !render_priv->settings.frame_height)
        return 1;               // library not initialized

    if (!render_priv->fontselect)
        return 1;

    if (render_priv->library != track->library)
        return 1;

    if (track->n_events == 0)
        return 1;               // nothing to do

    render_priv->track = track;
    render_priv->time = now;

    ass_lazy_track_init(render_priv->library, render_priv->track);

    ass_shaper_set_kerning(render_priv->shaper, track->Kerning);
    ass_shaper_set_language(render_priv->shaper, track->Language);
    ass_shaper_set_level(render_priv->shaper, render_priv->settings.shaper);

    // PAR correction
    double par = render_priv->settings.par;
    if (par == 0.) {
        if (settings_priv->frame_width && settings_priv->frame_height &&
            settings_priv->storage_width && settings_priv->storage_height) {
            double dar = ((double) settings_priv->frame_width) /
                         settings_priv->frame_height;
            double sar = ((double) settings_priv->storage_width) /
                         settings_priv->storage_height;
            par = sar / dar;
        } else
            par = 1.0;
    }
    render_priv->font_scale_x = par;

    render_priv->prev_images_root = render_priv->images_root;
    render_priv->images_root = NULL;

    check_cache_limits(render_priv, &render_priv->cache);

    return 0;
}