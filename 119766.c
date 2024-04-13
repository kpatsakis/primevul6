static void change_color(uint32_t *var, uint32_t new, double pwr)
{
    (*var) = ((uint32_t) (_r(*var) * (1 - pwr) + _r(new) * pwr) << 24) |
        ((uint32_t) (_g(*var) * (1 - pwr) + _g(new) * pwr) << 16) |
        ((uint32_t) (_b(*var) * (1 - pwr) + _b(new) * pwr) << 8) | _a(*var);
}