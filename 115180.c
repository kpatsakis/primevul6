Magick_RenderingIntent_from_PNG_RenderingIntent(const int ping_intent)
{
  switch (ping_intent)
  {
    case 0:
      return PerceptualIntent;

    case 1:
      return RelativeIntent;

    case 2:
      return SaturationIntent;

    case 3:
      return AbsoluteIntent;

    default:
      return UndefinedIntent;
    }
}