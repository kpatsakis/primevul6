inline void change_alpha(uint32_t *var, int32_t new, double pwr)
{
    *var = (*var & 0xFFFFFF00) | (uint8_t) (_a(*var) * (1 - pwr) + new * pwr);
}