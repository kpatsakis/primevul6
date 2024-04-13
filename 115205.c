Magick_RenderingIntentString_from_PNG_RenderingIntent(const int ping_intent)
{
  switch (ping_intent)
  {
    case 0:
      return "Perceptual Intent";

    case 1:
      return "Relative Intent";

    case 2:
      return "Saturation Intent";

    case 3:
      return "Absolute Intent";

    default:
      return "Undefined Intent";
    }
}