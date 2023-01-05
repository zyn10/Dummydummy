#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
using namespace std;

pthread_cond_t c = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int done;
// THREAD Function
void *Thread_Fun(void *arg) {
  while (done == 0) // To avoid supurious wake up, done is used. 
    pthread_cond_wait(&c, &m); // Thread will wait for signal until time of sleep is completed or the alarm is over. 
  cout << "Thread has finished its execution" << endl;
  pthread_exit(NULL);
}
class Alarm_Clock { // alarm Class

  int time, hours, mins, secs, alarm;
  friend void *Thread_Fun(void *); 
public:
  Alarm_Clock(int);
  void GoToSleep(int);
  void PrintTime();
  void Timer(); 
};
Alarm_Clock::Alarm_Clock(int t) { // Constructor
  done = 0;
  alarm = t;
  secs = t;
  mins = t / 60;
  secs = t % 60;
  hours = mins / 60;
  mins = mins % 60;

}
void Alarm_Clock::GoToSleep(int ForHowLong) {
  time = ForHowLong; // Go To Sleep Function...
}
// Function to Print alarm clock...
void Alarm_Clock::Timer() {
  while (alarm > 0) {
   
    system("clear");
 if (time <= 0 && done == 0) { // If Thread sleep time is completed.
      pthread_cond_signal(&c);
      done = 1;
      cout << "Thread has been waken up" << endl;
    } else if (done == 0)
      cout << "Thread is Sleeping" << endl;
else 
cout << "Thread has waken up" << endl;
    PrintTime();
    alarm--;
    secs--;
    time--;
    if (secs < 0) {
      secs = 60;
      mins--;
    }
    if (mins < 0) {
      mins = 60;
      hours--;
    }
    sleep(1);
if (alarm <= 0) // If Alarm is over, Snooze option will appear.
{
system ("clear"); 
cout << endl;
PrintTime(); 
char y;
cout << "Snooze Alarm for 10 seconds (Y/N): "; 
cin >> y;   
switch(y)
{
case 'y':
case 'Y':
alarm = alarm+10; 
secs = secs + 10; 
break; // In case any other key is pressed, program will come out
}
}
  }
done = 1; 
  pthread_cond_broadcast(&c); // Wake up all Sleeping Threads
cout << "All Threads have waken up" << endl;
}
void Alarm_Clock::PrintTime() { // Function to Alarm Clock
  cout << "==================================\n"
       << "Alarm Time Left:  " << hours << ":" << mins << ":" << secs << endl
       << "==================================\n";
}
int main() {
cout << "Enter Time of Alarm (in secs): "; 
int x, y; 
cin >> x; 
Alarm_Clock obj(x); 
cout << "Enter Time of Sleep for Thread (in secs): ";
cin >> y; 
  obj.GoToSleep(y);
pthread_t t1; 
pthread_create(&t1, NULL, Thread_Fun, NULL);
obj.Timer();
pthread_join(t1, NULL);
  return 0;
}
