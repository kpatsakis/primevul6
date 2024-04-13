Magick_RenderingIntent_to_PNG_RenderingIntent(const RenderingIntent intent)
{
  switch (intent)
  {
    case PerceptualIntent:
       return 0;

    case RelativeIntent:
       return 1;

    case SaturationIntent:
       return 2;

    case AbsoluteIntent:
       return 3;

    default:
       return -1;
  }
}