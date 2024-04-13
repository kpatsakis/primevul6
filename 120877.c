unsigned lodepng_encode(unsigned char** out, size_t* outsize,
                        const unsigned char* image, unsigned w, unsigned h,
                        LodePNGState* state)
{
  LodePNGInfo info;
  ucvector outv;
  unsigned char* data = 0; /*uncompressed version of the IDAT chunk data*/
  size_t datasize = 0;

  /*provide some proper output values if error will happen*/
  *out = 0;
  *outsize = 0;
  state->error = 0;

  lodepng_info_init(&info);
  lodepng_info_copy(&info, &state->info_png);

  if((info.color.colortype == LCT_PALETTE || state->encoder.force_palette)
      && (info.color.palettesize == 0 || info.color.palettesize > 256))
  {
    state->error = 68; /*invalid palette size, it is only allowed to be 1-256*/
    goto fail;
  }

  if(state->encoder.auto_convert)
  {
    state->error = lodepng_auto_choose_color(&info.color, image, w, h, &state->info_raw);
  }
  if(state->error)
    goto fail;

  if(state->encoder.zlibsettings.btype > 2)
  {
    state->error = 61; /*error: unexisting btype*/
    goto fail;
  }
  if(state->info_png.interlace_method > 1)
  {
    state->error = 71; /*error: unexisting interlace mode*/
    goto fail;
  }

  state->error = checkColorValidity(info.color.colortype, info.color.bitdepth);
  if(state->error)
    goto fail; /*error: unexisting color type given*/
  state->error = checkColorValidity(state->info_raw.colortype, state->info_raw.bitdepth);
  if(state->error)
    goto fail; /*error: unexisting color type given*/

  if(!lodepng_color_mode_equal(&state->info_raw, &info.color))
  {
    unsigned char* converted;
    size_t size = (w * h * lodepng_get_bpp(&info.color) + 7) / 8;

    converted = (unsigned char*)calloc(size, 1);
    if(!converted && size) state->error = 83; /*alloc fail*/
    if(!state->error)
    {
      state->error = lodepng_convert(converted, image, &info.color, &state->info_raw, w, h);
    }
    if(!state->error) preProcessScanlines(&data, &datasize, converted, w, h, &info, &state->encoder);
    free(converted);
  }
  else preProcessScanlines(&data, &datasize, image, w, h, &info, &state->encoder);

  ucvector_init(&outv);
  while(!state->error) /*while only executed once, to break on error*/
  {
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
    size_t i;
#endif /*LODEPNG_COMPILE_ANCILLARY_CHUNKS*/
    /*write signature and chunks*/
    writeSignature(&outv);
    /*IHDR*/
    addChunk_IHDR(&outv, w, h, info.color.colortype, info.color.bitdepth, info.interlace_method);
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
    /*unknown chunks between IHDR and PLTE*/
    if(info.unknown_chunks_data[0])
    {
      state->error = addUnknownChunks(&outv, info.unknown_chunks_data[0], info.unknown_chunks_size[0]);
      if(state->error) break;
    }
#endif /*LODEPNG_COMPILE_ANCILLARY_CHUNKS*/
    /*PLTE*/
    if(info.color.colortype == LCT_PALETTE)
    {
      addChunk_PLTE(&outv, &info.color);
    }
    if(state->encoder.force_palette && (info.color.colortype == LCT_RGB || info.color.colortype == LCT_RGBA))
    {
      addChunk_PLTE(&outv, &info.color);
    }
    /*tRNS*/
    if(info.color.colortype == LCT_PALETTE && getPaletteTranslucency(info.color.palette, info.color.palettesize) != 0)
    {
      addChunk_tRNS(&outv, &info.color);
    }
    if((info.color.colortype == LCT_GREY || info.color.colortype == LCT_RGB) && info.color.key_defined)
    {
      addChunk_tRNS(&outv, &info.color);
    }
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
    /*bKGD (must come between PLTE and the IDAt chunks*/
    if(info.background_defined) addChunk_bKGD(&outv, &info);
    /*pHYs (must come before the IDAT chunks)*/
    if(info.phys_defined) addChunk_pHYs(&outv, &info);

    /*unknown chunks between PLTE and IDAT*/
    if(info.unknown_chunks_data[1])
    {
      state->error = addUnknownChunks(&outv, info.unknown_chunks_data[1], info.unknown_chunks_size[1]);
      if(state->error) break;
    }
#endif /*LODEPNG_COMPILE_ANCILLARY_CHUNKS*/
    /*IDAT (multiple IDAT chunks must be consecutive)*/
    state->error = addChunk_IDAT(&outv, data, datasize, &state->encoder.zlibsettings);
    if(state->error) break;
#ifdef LODEPNG_COMPILE_ANCILLARY_CHUNKS
    /*tIME*/
    if(info.time_defined) addChunk_tIME(&outv, &info.time);
    /*tEXt and/or zTXt*/
    for(i = 0; i < info.text_num; i++)
    {
      if(strlen(info.text_keys[i]) > 79)
      {
        state->error = 66; /*text chunk too large*/
        break;
      }
      if(strlen(info.text_keys[i]) < 1)
      {
        state->error = 67; /*text chunk too small*/
        break;
      }
      if(state->encoder.text_compression)
      {
        addChunk_zTXt(&outv, info.text_keys[i], info.text_strings[i], &state->encoder.zlibsettings);
      }
      else
      {
        addChunk_tEXt(&outv, info.text_keys[i], info.text_strings[i]);
      }
    }
    /*LodePNG version id in text chunk*/
    if(state->encoder.add_id)
    {
      unsigned alread_added_id_text = 0;
      for(i = 0; i < info.text_num; i++)
      {
        if(!strcmp(info.text_keys[i], "LodePNG"))
        {
          alread_added_id_text = 1;
          break;
        }
      }
      if(alread_added_id_text == 0)
      {
        addChunk_tEXt(&outv, "LodePNG", VERSION_STRING); /*it's shorter as tEXt than as zTXt chunk*/
      }
    }
    /*iTXt*/
    for(i = 0; i < info.itext_num; i++)
    {
      if(strlen(info.itext_keys[i]) > 79)
      {
        state->error = 66; /*text chunk too large*/
        break;
      }
      if(strlen(info.itext_keys[i]) < 1)
      {
        state->error = 67; /*text chunk too small*/
        break;
      }
      addChunk_iTXt(&outv, state->encoder.text_compression,
                    info.itext_keys[i], info.itext_langtags[i], info.itext_transkeys[i], info.itext_strings[i],
                    &state->encoder.zlibsettings);
    }

    /*unknown chunks between IDAT and IEND*/
    if(info.unknown_chunks_data[2])
    {
      state->error = addUnknownChunks(&outv, info.unknown_chunks_data[2], info.unknown_chunks_size[2]);
      if(state->error) break;
    }
#endif /*LODEPNG_COMPILE_ANCILLARY_CHUNKS*/
    addChunk_IEND(&outv);

    break; /*this isn't really a while loop; no error happened so break out now!*/
  }

  /*instead of cleaning the vector up, give it to the output*/
  *out = outv.data;
  *outsize = outv.size;

  fail:
  lodepng_info_cleanup(&info);
  free(data);
  return state->error;
}