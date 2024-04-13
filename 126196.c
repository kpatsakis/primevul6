gif_out_code(
    gif_t           /* in */ *g,
    unsigned short  /* in */ code
)
{
    /* recurse to decode the prefixes, since the linked-list is backwards,
       and working backwards through an interleaved image would be nasty */
    if (g->codes[code].prefix >= 0) {
        gif_out_code(g, (unsigned short)g->codes[code].prefix);
    }

    if (g->cur_y >= g->max_y) {
        return;
    }

    g->out[g->cur_x + g->cur_y] = g->codes[code].suffix;
    g->cur_x++;

    if (g->cur_x >= g->max_x) {
        g->cur_x = g->start_x;
        g->cur_y += g->step;

        while (g->cur_y >= g->max_y && g->parse > 0) {
            g->step = (1 << g->parse) * g->line_size;
            g->cur_y = g->start_y + (g->step >> 1);
            --g->parse;
        }
    }
}