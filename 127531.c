static int init_tile(Jpeg2000DecoderContext *s, int tileno)
{
    int compno;
    int tilex = tileno % s->numXtiles;
    int tiley = tileno / s->numXtiles;
    Jpeg2000Tile *tile = s->tile + tileno;

    if (!tile->comp)
        return AVERROR(ENOMEM);

    for (compno = 0; compno < s->ncomponents; compno++) {
        Jpeg2000Component *comp = tile->comp + compno;
        Jpeg2000CodingStyle *codsty = tile->codsty + compno;
        Jpeg2000QuantStyle  *qntsty = tile->qntsty + compno;
        int ret; // global bandno

        comp->coord_o[0][0] = FFMAX(tilex       * s->tile_width  + s->tile_offset_x, s->image_offset_x);
        comp->coord_o[0][1] = FFMIN((tilex + 1) * s->tile_width  + s->tile_offset_x, s->width);
        comp->coord_o[1][0] = FFMAX(tiley       * s->tile_height + s->tile_offset_y, s->image_offset_y);
        comp->coord_o[1][1] = FFMIN((tiley + 1) * s->tile_height + s->tile_offset_y, s->height);

        comp->coord[0][0] = ff_jpeg2000_ceildivpow2(comp->coord_o[0][0], s->reduction_factor);
        comp->coord[0][1] = ff_jpeg2000_ceildivpow2(comp->coord_o[0][1], s->reduction_factor);
        comp->coord[1][0] = ff_jpeg2000_ceildivpow2(comp->coord_o[1][0], s->reduction_factor);
        comp->coord[1][1] = ff_jpeg2000_ceildivpow2(comp->coord_o[1][1], s->reduction_factor);

        if (ret = ff_jpeg2000_init_component(comp, codsty, qntsty,
                                             s->cbps[compno], s->cdx[compno],
                                             s->cdy[compno], s->avctx))
            return ret;
    }
    return 0;
}