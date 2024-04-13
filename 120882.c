void lodepng_state_init(LodePNGState* state)
{
#ifdef LODEPNG_COMPILE_DECODER
  lodepng_decoder_settings_init(&state->decoder);
#endif /*LODEPNG_COMPILE_DECODER*/
#ifdef LODEPNG_COMPILE_ENCODER
  lodepng_encoder_settings_init(&state->encoder);
#endif /*LODEPNG_COMPILE_ENCODER*/
  lodepng_color_mode_init(&state->info_raw);
  lodepng_info_init(&state->info_png);
  state->error = 1;
}