int64_t mg_millis(void) {
#if MG_ARCH == MG_ARCH_WIN32
  return GetTickCount();
#elif MG_ARCH == MG_ARCH_ESP32
  return esp_timer_get_time() / 1000;
#elif MG_ARCH == MG_ARCH_ESP8266
  return xTaskGetTickCount() * portTICK_PERIOD_MS;
#elif MG_ARCH == MG_ARCH_FREERTOS_TCP || MG_ARCH == MG_ARCH_FREERTOS_LWIP
  return xTaskGetTickCount() * portTICK_PERIOD_MS;
#elif MG_ARCH == MG_ARCH_AZURERTOS
  return tx_time_get() * (1000 /* MS per SEC */ / TX_TIMER_TICKS_PER_SECOND);
#elif MG_ARCH == MG_ARCH_UNIX && defined(__APPLE__)
  uint64_t ticks = mach_absolute_time();
  static mach_timebase_info_data_t timebase;
  mach_timebase_info(&timebase);
  double ticks_to_nanos = (double) timebase.numer / timebase.denom;
  uint64_t uptime_nanos = (uint64_t) (ticks_to_nanos * ticks);
  return (int64_t) (uptime_nanos / 1000000);
#elif MG_ARCH == MG_ARCH_UNIX
  struct timespec ts;
#ifdef _POSIX_MONOTONIC_CLOCK
#ifdef CLOCK_MONOTONIC_RAW
  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
#else
  clock_gettime(CLOCK_MONOTONIC, &ts);
#endif
#else
  clock_gettime(CLOCK_REALTIME, &ts);
#endif
  return ((int64_t) ts.tv_sec * 1000 + (int64_t) ts.tv_nsec / 1000000);
#else
  return time(NULL) * 1000;
#endif
}