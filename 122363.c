static int sixel_put_node(sixel_output_t *const context, int x,
               sixel_node_t *np, int ncolors, int keycolor)
{
    int nwrite;

    if (ncolors != 2 || keycolor == -1) {
        /* designate palette index */
        if (context->active_palette != np->color) {
            nwrite = sprintf((char *)context->buffer + context->pos,
                             "#%d", np->color);
            sixel_advance(context, nwrite);
            context->active_palette = np->color;
        }
    }

    for (; x < np->left; x++) {
        sixel_put_pixel(context, 0);
    }

    for (; x < np->right; x++) {
        sixel_put_pixel(context, np->map[x]);
    }

    sixel_put_flash(context);

    return x;
}