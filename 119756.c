static int parse_vector_clip(ASS_Renderer *render_priv,
                             struct arg *args, int nargs)
{
    int scale = 1;
    ASS_Drawing *drawing = render_priv->state.clip_drawing;

    if (nargs != 1 && nargs != 2)
        return 0;
    if (nargs == 2)
        scale = argtoi(args[0]);
    struct arg text = args[nargs - 1];

    ass_drawing_free(drawing);
    render_priv->state.clip_drawing = ass_drawing_new(render_priv->library);
    drawing = render_priv->state.clip_drawing;
    if (drawing) {
        drawing->scale = scale;
        drawing->scale_x = render_priv->font_scale_x * render_priv->font_scale;
        drawing->scale_y = render_priv->font_scale;
        ass_drawing_set_text(drawing, text.start, text.end - text.start);
    }

    return 1;
}