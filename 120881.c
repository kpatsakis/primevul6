void lodepng_state_cleanup(LodePNGState* state)
{
  lodepng_color_mode_cleanup(&state->info_raw);
  lodepng_info_cleanup(&state->info_png);
}