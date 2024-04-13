void TNEFPrintDate(dtr Date) {
  char days[7][15] = {"Sunday", "Monday", "Tuesday",
                      "Wednesday", "Thursday", "Friday", "Saturday"
                     };
  char months[12][15] = {"January", "February", "March", "April", "May",
                         "June", "July", "August", "September", "October", "November",
                         "December"
                        };

  if (Date.wDayOfWeek < 7)
    printf("%s ", days[Date.wDayOfWeek]);

  if ((Date.wMonth < 13) && (Date.wMonth > 0))
    printf("%s ", months[Date.wMonth - 1]);

  printf("%hu, %hu ", Date.wDay, Date.wYear);

  if (Date.wHour > 12)
    printf("%i:%02hu:%02hu pm", (Date.wHour - 12),
           Date.wMinute, Date.wSecond);
  else if (Date.wHour == 12)
    printf("%hu:%02hu:%02hu pm", (Date.wHour),
           Date.wMinute, Date.wSecond);
  else
    printf("%hu:%02hu:%02hu am", Date.wHour,
           Date.wMinute, Date.wSecond);
}