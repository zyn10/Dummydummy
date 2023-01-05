#include <fstream>
#include <iomanip>
#include <iostream>
#include <pthread.h>
using namespace std;

struct node {
  int data;
  node *next;
};
class Queue {
  node *front;
  node *rear;

public:
  Queue();
  bool isEmpty();
  void dequeue();
  void enqueue(int);
  void display();
  int peek();
};
int global_variable; // Values will be read into this variable. 
bool has_finished; // To check 1st thread's status.
void *read_values(void *arg);
void *store_values(void *arg);

int main() {
  Queue obj;            // To be sent as argument in Store Function
  global_variable = -1; // Initializing it...
  pthread_t reading_thread, storing_thread;
  has_finished = false;
  cout << "Initial Data: ";
  for (int i = 0; i < 100; i++)
    cout << setw(3) << i;
  cout << endl << endl;
  pthread_create(&reading_thread, NULL, read_values, NULL);
  pthread_create(&storing_thread, NULL, store_values, &obj);
  pthread_join(reading_thread, NULL);
  pthread_join(storing_thread, NULL);
  cout << endl << endl << "Data After Insertion: ";
  obj.display();
  cout << endl
       << endl
       << "As evident, almost all the values goes on missing in the queue. ";
  cout << endl;
  return 0;
}
void *read_values(void *arg) {
  fstream obj1;
  obj1.open("Hello.txt"); // A file containing numbers from 0 - 100.
  if (!obj1) {
    cout << "File doesn't exist";
    exit(1);
  }
  while (1) {
    obj1 >> global_variable;
    if (obj1.eof()) {
      break;
    }
  }
  has_finished = true;
}
void *store_values(void *arg) {
  Queue *obj = (Queue *)arg;
  while (has_finished == false) {
    if (global_variable != -1)
      obj->enqueue(global_variable);

  }
}

Queue::Queue() {
  front = NULL;
  rear = NULL;
}
bool Queue::isEmpty() {
  if (!front)
    return true;
  return false;
}
int Queue::peek() { return front->data; }
void Queue::enqueue(int d) {
  node *temp = new node;
  temp->data = d;
  temp->next = NULL;
  if (isEmpty()) {
    front = temp;
    rear = temp;
    return;
  }
  rear->next = temp;
  rear = temp;
}
void Queue::dequeue() {
  if (isEmpty()) {
    return;
  }
  node *temp = front;
  front = front->next;
  free(temp);
}
void Queue::display() {
  if (isEmpty()) {
    return;
  }
  Queue q;
  while (!(this->isEmpty())) {
    int temp = this->peek();
    q.enqueue(temp);
    this->dequeue();
    cout << temp << " ";
  }
  while (!(q.isEmpty())) {
    this->enqueue(q.peek());
    q.dequeue();
  }
}
