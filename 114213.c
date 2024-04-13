mit_update_delay(uint32_t *curr, uint32_t value)
{
    if (value && (*curr == 0 || value < *curr)) {
        *curr = value;
    }
}