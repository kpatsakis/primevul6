static void sixel_put_pixel(sixel_output_t *const context, int pix)
{
    if (pix < 0 || pix > '?') {
        pix = 0;
    }

    pix += '?';

    if (pix == context->save_pixel) {
        context->save_count++;
    } else {
        sixel_put_flash(context);
        context->save_pixel = pix;
        context->save_count = 1;
    }
}