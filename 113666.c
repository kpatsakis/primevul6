int LightProcess::GetId() {
  return (long)pthread_self() % g_procsCount;
}