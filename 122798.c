  switch (value) {
    case 3:
      sprintf((TNEF->priority), "high");
      break;
    case 2:
      sprintf((TNEF->priority), "normal");
      break;
    case 1:
      sprintf((TNEF->priority), "low");
      break;
    default:
      sprintf((TNEF->priority), "N/A");
      break;
  }