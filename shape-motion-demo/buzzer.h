#ifndef buzzer_included
#define buzzer_included

void buzzer_init();
void buzzer_set_period(short cycles);
void winningScreen();
void welcomeScreen();
void lostScreen();
void sounds(int state);

#endif // included
