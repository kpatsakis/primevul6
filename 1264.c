static CompositeOperator PSDBlendModeToCompositeOperator(const char *mode)
{
  if (mode == (const char *) NULL)
    return(OverCompositeOp);
  if (LocaleNCompare(mode,"norm",4) == 0)
    return(OverCompositeOp);
  if (LocaleNCompare(mode,"mul ",4) == 0)
    return(MultiplyCompositeOp);
  if (LocaleNCompare(mode,"diss",4) == 0)
    return(DissolveCompositeOp);
  if (LocaleNCompare(mode,"diff",4) == 0)
    return(DifferenceCompositeOp);
  if (LocaleNCompare(mode,"dark",4) == 0)
    return(DarkenCompositeOp);
  if (LocaleNCompare(mode,"lite",4) == 0)
    return(LightenCompositeOp);
  if (LocaleNCompare(mode,"hue ",4) == 0)
    return(HueCompositeOp);
  if (LocaleNCompare(mode,"sat ",4) == 0)
    return(SaturateCompositeOp);
  if (LocaleNCompare(mode,"colr",4) == 0)
    return(ColorizeCompositeOp);
  if (LocaleNCompare(mode,"lum ",4) == 0)
    return(LuminizeCompositeOp);
  if (LocaleNCompare(mode,"scrn",4) == 0)
    return(ScreenCompositeOp);
  if (LocaleNCompare(mode,"over",4) == 0)
    return(OverlayCompositeOp);
  if (LocaleNCompare(mode,"hLit",4) == 0)
    return(HardLightCompositeOp);
  if (LocaleNCompare(mode,"sLit",4) == 0)
    return(SoftLightCompositeOp);
  if (LocaleNCompare(mode,"smud",4) == 0)
    return(ExclusionCompositeOp);
  if (LocaleNCompare(mode,"div ",4) == 0)
    return(ColorDodgeCompositeOp);
  if (LocaleNCompare(mode,"idiv",4) == 0)
    return(ColorBurnCompositeOp);
  if (LocaleNCompare(mode,"lbrn",4) == 0)
    return(LinearBurnCompositeOp);
  if (LocaleNCompare(mode,"lddg",4) == 0)
    return(LinearDodgeCompositeOp);
  if (LocaleNCompare(mode,"lLit",4) == 0)
    return(LinearLightCompositeOp);
  if (LocaleNCompare(mode,"vLit",4) == 0)
    return(VividLightCompositeOp);
  if (LocaleNCompare(mode,"pLit",4) == 0)
    return(PinLightCompositeOp);
  if (LocaleNCompare(mode,"hMix",4) == 0)
    return(HardMixCompositeOp);
  return(OverCompositeOp);
}