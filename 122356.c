static MagickBooleanType sixel_encode_impl(unsigned char *pixels, size_t width,size_t height,
                  unsigned char *palette, size_t ncolors, int keycolor,
                  sixel_output_t *context)
{
#define RelinquishNodesAndMap \
    while ((np = context->node_free) != NULL) { \
        context->node_free = np->next; \
        np=(sixel_node_t *) RelinquishMagickMemory(np); \
    } \
    map = (unsigned char *) RelinquishMagickMemory(map)

    int x, y, i, n, c;
    int left, right;
    int pix;
    unsigned char *map;
    sixel_node_t *np, *tp, top;
    int nwrite;
    size_t len;

    context->pos = 0;

    if (ncolors < 1) {
        return (MagickFalse);
    }
    len = ncolors * width;
    context->active_palette = (-1);

    if ((map = (unsigned char *)AcquireQuantumMemory(len, sizeof(unsigned char))) == NULL) {
        return (MagickFalse);
    }
    (void) ResetMagickMemory(map, 0, len);

    if (context->has_8bit_control) {
        nwrite = sprintf((char *)context->buffer, "\x90" "0;0;0" "q");
    } else {
        nwrite = sprintf((char *)context->buffer, "\x1bP" "0;0;0" "q");
    }
    if (nwrite <= 0) {
        return (MagickFalse);
    }
    sixel_advance(context, nwrite);
    nwrite = sprintf((char *)context->buffer + context->pos, "\"1;1;%d;%d", (int) width, (int) height);
    if (nwrite <= 0) {
        RelinquishNodesAndMap;
        return (MagickFalse);
    }
    sixel_advance(context, nwrite);

    if (ncolors != 2 || keycolor == -1) {
        for (n = 0; n < (ssize_t) ncolors; n++) {
            /* DECGCI Graphics Color Introducer  # Pc ; Pu; Px; Py; Pz */
            nwrite = sprintf((char *)context->buffer + context->pos, "#%d;2;%d;%d;%d",
                             n,
                             (palette[n * 3 + 0] * 100 + 127) / 255,
                             (palette[n * 3 + 1] * 100 + 127) / 255,
                             (palette[n * 3 + 2] * 100 + 127) / 255);
            if (nwrite <= 0) {
                RelinquishNodesAndMap;
                return (MagickFalse);
            }
            sixel_advance(context, nwrite);
            if (nwrite <= 0) {
                RelinquishNodesAndMap;
                return (MagickFalse);
            }
        }
    }

    for (y = i = 0; y < (ssize_t) height; y++) {
        for (x = 0; x < (ssize_t) width; x++) {
            pix = pixels[y * width + x];
            if (pix >= 0 && pix < (ssize_t) ncolors && pix != keycolor) {
                map[pix * width + x] |= (1 << i);
            }
        }

        if (++i < 6 && (y + 1) < (ssize_t) height) {
            continue;
        }

        for (c = 0; c < (ssize_t) ncolors; c++) {
            for (left = 0; left < (ssize_t) width; left++) {
                if (*(map + c * width + left) == 0) {
                    continue;
                }

                for (right = left + 1; right < (ssize_t) width; right++) {
                    if (*(map + c * width + right) != 0) {
                        continue;
                    }

                    for (n = 1; (right + n) < (ssize_t) width; n++) {
                        if (*(map + c * width + right + n) != 0) {
                            break;
                        }
                    }

                    if (n >= 10 || right + n >= (ssize_t) width) {
                        break;
                    }
                    right = right + n - 1;
                }

                if ((np = context->node_free) != NULL) {
                    context->node_free = np->next;
                } else if ((np = (sixel_node_t *)AcquireMagickMemory(sizeof(sixel_node_t))) == NULL) {
                    RelinquishNodesAndMap;
                    return (MagickFalse);
                }

                np->color = c;
                np->left = left;
                np->right = right;
                np->map = map + c * width;

                top.next = context->node_top;
                tp = &top;

                while (tp->next != NULL) {
                    if (np->left < tp->next->left) {
                        break;
                    }
                    if (np->left == tp->next->left && np->right > tp->next->right) {
                        break;
                    }
                    tp = tp->next;
                }

                np->next = tp->next;
                tp->next = np;
                context->node_top = top.next;

                left = right - 1;
            }

        }

        for (x = 0; (np = context->node_top) != NULL;) {
            if (x > np->left) {
                /* DECGCR Graphics Carriage Return */
                context->buffer[context->pos] = '$';
                sixel_advance(context, 1);
                x = 0;
            }

            x = sixel_put_node(context, x, np, (int) ncolors, keycolor);
            sixel_node_del(context, np);
            np = context->node_top;

            while (np != NULL) {
                if (np->left < x) {
                    np = np->next;
                    continue;
                }

                x = sixel_put_node(context, x, np, (int) ncolors, keycolor);
                sixel_node_del(context, np);
                np = context->node_top;
            }
        }

        /* DECGNL Graphics Next Line */
        context->buffer[context->pos] = '-';
        sixel_advance(context, 1);
        if (nwrite <= 0) {
            RelinquishNodesAndMap;
            return (MagickFalse);
        }

        i = 0;
        (void) ResetMagickMemory(map, 0, len);
    }

    if (context->has_8bit_control) {
        context->buffer[context->pos] = 0x9c;
        sixel_advance(context, 1);
    } else {
        context->buffer[context->pos] = 0x1b;
        context->buffer[context->pos + 1] = '\\';
        sixel_advance(context, 2);
    }
    if (nwrite <= 0) {
        RelinquishNodesAndMap;
        return (MagickFalse);
    }

    /* flush buffer */
    if (context->pos > 0) {
        WriteBlob(context->image,context->pos,context->buffer);
    }

    RelinquishNodesAndMap;

    return(MagickTrue);
}