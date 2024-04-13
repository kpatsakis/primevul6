static int hls_to_rgb(int hue, int lum, int sat)
{
    int R, G, B;
    int Magic1, Magic2;
    const int RGBMAX = 255;
    const int HLSMAX = 100;

    if (sat == 0) {
        R = G = B = (lum * RGBMAX) / HLSMAX;
    } else {
        if (lum <= (HLSMAX / 2)) {
            Magic2 = (lum * (HLSMAX + sat) + (HLSMAX / 2)) / HLSMAX;
        } else {
            Magic2 = lum + sat - ((lum * sat) + (HLSMAX / 2)) / HLSMAX;
        }
        Magic1 = 2 * lum - Magic2;

        R = (hue_to_rgb(Magic1, Magic2, hue + (HLSMAX / 3)) * RGBMAX + (HLSMAX / 2)) / HLSMAX;
        G = (hue_to_rgb(Magic1, Magic2, hue) * RGBMAX + (HLSMAX / 2)) / HLSMAX;
        B = (hue_to_rgb(Magic1, Magic2, hue - (HLSMAX / 3)) * RGBMAX + (HLSMAX/2)) / HLSMAX;
    }
    return SIXEL_RGB(R, G, B);
}