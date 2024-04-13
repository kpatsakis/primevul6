float _WM_GetSamplesPerTick(uint32_t divisions, uint32_t tempo) {
    float microseconds_per_tick;
    float secs_per_tick;
    float samples_per_tick;

    /* Slow but needed for accuracy */
    microseconds_per_tick = (float) tempo / (float) divisions;
    secs_per_tick = microseconds_per_tick / 1000000.0f;
    samples_per_tick = _WM_SampleRate * secs_per_tick;

    return (samples_per_tick);
}