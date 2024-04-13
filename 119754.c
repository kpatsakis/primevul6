inline uint32_t mult_alpha(uint32_t a, uint32_t b)
{
    return 0xFF - (0xFF - a) * (0xFF - b) / 0xFF;
}