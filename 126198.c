gif_get8(gif_context_t *s)
{
    if (s->img_buffer < s->img_buffer_end) {
        return *s->img_buffer++;
    }
    return 0;
}