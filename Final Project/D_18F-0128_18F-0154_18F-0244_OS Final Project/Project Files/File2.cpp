#include<iostream>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string>
#include<chrono>
using namespace std;
using namespace std::chrono;
char arr[3][3];
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int done = 0;
class TicTacToe {
public:
	TicTacToe();
	static void* playgame(void*);
	static void drawboard();
	static void playermove();
	static void* checkwin(void*);
	static void* checkdraw(void*);
	static void* checkresult(void*);
};
TicTacToe::TicTacToe() {
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			arr[i][j] = '-';
	}
}
void* TicTacToe::playgame(void* arg)
{
	pthread_mutex_lock(&m); // Mutex locked to block access to global variables
	system("clear");
	drawboard();
	playermove();
	done = 1;
	pthread_cond_signal(&c); // Signal sent to checkresult thread to wake up from sleep. 
	pthread_mutex_unlock(&m);
	cout << endl;
	pthread_exit(NULL);
}
void TicTacToe::drawboard()
{
	cout << "HUMAN (X) - COMPUTER (O)" << endl << endl
		<< "Terminal: COMPUTER" << endl << endl;
	for (int i = 0; i < 3; i++)
	{
		cout << " ";
		for (int j = 0; j < 3; j++)
		{
			cout << arr[i][j];
			if (j == 2)
				continue;
			cout << "  |  ";
		}
		if (i == 2)
			continue;
		cout << endl;
		cout << "____|_____|____" << endl

			<< "    |     |    " << endl;
	}
}
void TicTacToe::playermove()
{
	int row = 0, col = 0;
	bool correctmove = false;
	cout << endl << endl;
	cout << "Computer is contemplating its move..." << endl;
	sleep(2); 
	cout << endl;
	do
	{
		if ((row < 3 && row > -1) || (col > -1 && col < 3))
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					if (arr[row][col] == '-')
					{
						arr[row][col] = 'O';
						correctmove = true;
					}
				}
			}
		}
		row++;
		if (row > 2)
		{
			row = 0;
			col++;
		}
	} while (!correctmove);
}
void* TicTacToe::checkresult(void* arg)
{
	pthread_mutex_lock(&m);
	while (done == 0) // Condition Variable used. 
		pthread_cond_wait(&c, &m); // Waiting for the playgame thread to complete execution. 
	char* temp = (char*)arg;
	char c = *temp;
	pthread_t t_draw, t_win;
	void* first, * second;
	pthread_create(&t_win, NULL, TicTacToe::checkwin, &c);
	pthread_create(&t_draw, NULL, TicTacToe::checkdraw, NULL);
	pthread_join(t_win, &first);
	pthread_join(t_draw, &second);
	bool* win_check = (bool*)first;
	bool* draw_check = (bool*)second;
	int* result = new int;
	if (*win_check)
		*result = 1;
	else if (*draw_check)
		*result = 2;
	else
		*result = 0;
	pthread_mutex_unlock(&m);
	return result;
}
void* TicTacToe::checkwin(void* arg)
{
	char* c = (char*)arg;
	bool* ptr = new bool;
	*ptr = false;
	for (int i = 0; i < 3; i++)
	{
		if (arr[i][0] == (*c) && arr[i][1] == (*c) && arr[i][2] == (*c))
			*ptr = true;
		if (arr[0][i] == (*c) && arr[1][i] == (*c) && arr[2][i] == (*c))
			*ptr = true;
	}
	if (arr[0][0] == (*c) && arr[1][1] == (*c) && arr[2][2] == (*c))
		*ptr = true;
	if (arr[0][2] == (*c) && arr[1][1] == (*c) && arr[2][0] == (*c))
		*ptr = true;
	return ptr;
}
void* TicTacToe::checkdraw(void* arg)
{
	bool* ptr = new bool;
	*ptr = true;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (arr[i][j] == '-')
				*ptr = false;
		}
	}
	return ptr;
}
// ---------------- PROGRAM 2 -------------------------
bool time_condition = true;
void* time_taken(void* arg)
{
	float* turn_time = new float;
	auto start = high_resolution_clock::now();
	while (time_condition);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	*turn_time = duration.count();
	return turn_time; // Returing the Time to the main function. 
}
int main()
{
	system("clear");
	int status;
	char ch;
	const char pipe_file[] = "/tmp/program";
	mkfifo(pipe_file, 0666);
	bool check = true;
	float* turn_time;
	cout << endl << endl << "Waiting for instruction...." << endl;
	status = open(pipe_file, O_RDONLY);
	read(status, &check, sizeof(bool));
	close(status);
	pthread_t tid, time_thread, r_thread, p_thread;
	while (check)
	{
		TicTacToe obj;
		bool fh = true, first_turn = true;
		while (fh)
		{
			void* r_temp;
			system("clear");
			obj.drawboard();
			if (!first_turn)
				cout << "\n\nTime Taken for the turn: " << *turn_time << " milliseconds" << endl << endl;
			first_turn = false;
			cout << endl << endl << "Waiting for opponent's turn...." << endl;
			status = open(pipe_file, O_RDONLY);
			read(status, arr, 9 * sizeof(char));
			close(status);
			ch = 'X';
			pthread_create(&r_thread, NULL, TicTacToe::checkresult, &ch);
			done = 1;
			pthread_cond_signal(&c); // Signal sent to checkresult thread to wake up from sleep. 
			pthread_join(r_thread, &r_temp);
			int* opp_result = (int*)r_temp;
			if (*opp_result == 1)
			{
				fh = false;
				system("clear");
				obj.drawboard();
				cout << endl << endl << "Sorry, You have lost the game!" << endl;
			}
			else if (*opp_result == 2)
			{
				fh = false;
				system("clear");
				obj.drawboard();
				cout << endl << endl << "The Game has been Drawn!" << endl;
			}
			else
			{
				done = 0;
				void* temp;
				time_condition = true;
				ch = 'O';
				pthread_create(&time_thread, NULL, time_taken, NULL);
				pthread_create(&p_thread, NULL, TicTacToe::playgame, NULL);
				pthread_create(&r_thread, NULL, TicTacToe::checkresult, &ch);
				pthread_join(p_thread, NULL);
				time_condition = false;
				pthread_join(r_thread, &r_temp);
				int* result = (int*)r_temp;
				pthread_join(time_thread, &temp);
				turn_time = (float*)temp;
				if (*result == 1)
				{
					fh = false;
					system("clear");
					obj.drawboard();
					cout << endl << endl << "Congratulations! You have won the game!" << endl;
				}
				else if (*result == 2)
				{
					fh = false;
					system("clear");
					obj.drawboard();
					cout << endl << endl << "The Game has been Drawn!" << endl;
				}
				status = open(pipe_file, O_WRONLY);
				write(status, arr, 9 * sizeof(char));
				close(status);
			}
		}
		cout << endl << endl << "Waiting for next instruction...." << endl;
		status = open(pipe_file, O_RDONLY);
		read(status, &check, sizeof(bool));
		close(status);
	}
	cout << endl << "Thank you for Playing!" << endl << endl;
	exit(0);
}

