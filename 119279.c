static bool isFlowCorrect(float u) { return !cvIsNaN(u) && (fabs(u) < 1e9); }