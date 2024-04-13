int MAPISysTimetoDTR(BYTE *data, dtr *thedate) {
  DDWORD ddword_tmp;
  int startingdate = 0;
  int tmp_date;
  int days_in_year = 365;
  unsigned int months[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  ddword_tmp = *((DDWORD *)data);
  ddword_tmp = ddword_tmp / 10; // micro-s
  ddword_tmp /= 1000; // ms
  ddword_tmp /= 1000; // s

  thedate->wSecond = (ddword_tmp % 60);

  ddword_tmp /= 60; // seconds to minutes
  thedate->wMinute = (ddword_tmp % 60);

  ddword_tmp /= 60; //minutes to hours
  thedate->wHour = (ddword_tmp % 24);

  ddword_tmp /= 24; // Hours to days

  // Now calculate the year based on # of days
  thedate->wYear = 1601;
  startingdate = 1;
  while (ddword_tmp >= days_in_year) {
    ddword_tmp -= days_in_year;
    thedate->wYear++;
    days_in_year = 365;
    startingdate++;
    if ((thedate->wYear % 4) == 0) {
      if ((thedate->wYear % 100) == 0) {
        // if the year is 1700,1800,1900, etc, then it is only
        // a leap year if exactly divisible by 400, not 4.
        if ((thedate->wYear % 400) == 0) {
          startingdate++;
          days_in_year = 366;
        }
      }  else {
        startingdate++;
        days_in_year = 366;
      }
    }
    startingdate %= 7;
  }

  // the remaining number is the day # in this year
  // So now calculate the Month, & Day of month
  if ((thedate->wYear % 4) == 0) {
    // 29 days in february in a leap year
    months[1] = 29;
  }

  tmp_date = (int)ddword_tmp;
  thedate->wDayOfWeek = (tmp_date + startingdate) % 7;
  thedate->wMonth = 0;

  while (tmp_date > months[thedate->wMonth]) {
    tmp_date -= months[thedate->wMonth];
    thedate->wMonth++;
  }
  thedate->wMonth++;
  thedate->wDay = tmp_date + 1;
  return 0;
}