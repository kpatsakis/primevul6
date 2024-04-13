static int hue_to_rgb(int n1, int n2, int hue)
{
    const int HLSMAX = 100;

    if (hue < 0) {
        hue += HLSMAX;
    }

    if (hue > HLSMAX) {
        hue -= HLSMAX;
    }

    if (hue < (HLSMAX / 6)) {
        return (n1 + (((n2 - n1) * hue + (HLSMAX / 12)) / (HLSMAX / 6)));
    }
    if (hue < (HLSMAX / 2)) {
        return (n2);
    }
    if (hue < ((HLSMAX * 2) / 3)) {
        return (n1 + (((n2 - n1) * (((HLSMAX * 2) / 3) - hue) + (HLSMAX / 12))/(HLSMAX / 6)));
    }
    return (n1);
}