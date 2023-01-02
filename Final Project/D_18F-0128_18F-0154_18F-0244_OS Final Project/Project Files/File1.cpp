#include<chrono>
#include<iostream>
#include<stdio.h>
#include <iomanip>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include <string>
#include <fstream>
using namespace std;
using namespace std::chrono;
char arr[3][3]; // For Board printing
int record[3]; // To store win/loss/draw record of players 
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int done = 0;
class TicTacToe { // TicTacToe class implementation 
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
		<< "Terminal: HUMAN" << endl << endl;
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
	int row, col;
	bool correctmove = false;
	cout << endl << endl;
	cout << "Make your Move!" << endl;
	while (!correctmove)
	{
		cout << "Enter Row(0-2): ";
		cin >> row;
		cout << "Enter Col(0-2): ";
		cin >> col;
		if ((row < 3 && row > -1) || (col > -1 && col < 3))
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					if (arr[row][col] == '-')
					{
						arr[row][col] = 'X';
						correctmove = true;
					}
				}
			}
		}
		if (!correctmove)
			cout << "Wrong Inputs, please enter again" << endl;
	}
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
void* save_score(void* arg)
{
	fstream in;
	in.open("Score.txt", ios::out | ios::trunc);
	for (int i = 0; i < 3; i++)
		in << record[i] << endl;
}
bool time_condition = true;
float turn_time;
void* time_taken(void*)
{
	auto start = high_resolution_clock::now();
	while (time_condition);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	turn_time = duration.count();

}
const char pipe_file[] = "/tmp/program";
void main_program()
{
	int status;
	char ch;
	void* r_temp;
	bool fh = true;
	pid_t pid;
	pthread_t tid, time_thread, r_thread, p_thread;
	TicTacToe obj;
	mkfifo(pipe_file, 0666);
	while (fh)
	{
		done = 0; // Condition variable for Synchronization. 
		ch = 'X';
		time_condition = true;
		pthread_create(&time_thread, NULL, time_taken, NULL);
		pthread_create(&p_thread, NULL, TicTacToe::playgame, NULL);
		//obj.playgame();
		pthread_create(&r_thread, NULL, TicTacToe::checkresult, &ch);
		pthread_join(p_thread, NULL);
		time_condition = false;
		pthread_join(r_thread, &r_temp);
		int* result = (int*)r_temp;
		if (*result != 0)
		{
			fh = false;
			pid = fork(); // Creating another process to calculate the score 
			if (pid == 0)
			{
				if (*result == 1)
					record[0]++;
				if (*result == 2)
					record[2]++;
				pthread_create(&tid, NULL, save_score, NULL); // Updated scores have been stored. 
				pthread_join(tid, NULL);
				exit(0); // Child process terminated. 
			}
			wait(NULL); // Parent Process will wait for child to execute. 
			if (*result == 1)
			{
				system("clear");
				obj.drawboard();
				cout << endl << endl << "Congratulations! You have won the game!" << endl;
			}
			else if (*result == 2)
			{
				system("clear");
				obj.drawboard();
				cout << endl << endl << "The Game has been Drawn!" << endl;
			}
		}
		status = open(pipe_file, O_WRONLY);
		write(status, arr, 9 * sizeof(char));
		close(status);
		if (fh)
		{
			system("clear");
			obj.drawboard();
			cout << "\n\nTime Taken for turn: " << turn_time << " milliseconds" << endl;
			cout << endl << endl << "Waiting for opponent's turn...." << endl;
			status = open(pipe_file, O_RDONLY);
			read(status, arr, 9 * sizeof(char));
			close(status);
			ch = 'O';
			pthread_create(&r_thread, NULL, TicTacToe::checkresult, &ch);
			done = 1;
			pthread_cond_signal(&c); // Signal sent to checkresult thread to wake up from sleep. 
			pthread_join(r_thread, &r_temp);
			int* opp_result = new int;
			opp_result = (int*)r_temp;
			if (*opp_result != 0)
			{
				fh = false;
				pid = fork(); // Creating another process to calculate the score 
				if (pid == 0)
				{
					if (*opp_result == 1)
						record[1]++;
					if (*opp_result == 2)
						record[2]++;
					pthread_create(&tid, NULL, save_score, NULL); // Updated scores have been stored. 
					pthread_join(tid, NULL);
					exit(0); // Child process terminated. 
				}
				wait(NULL);
				if (*opp_result == 1)
				{
					system("clear");
					obj.drawboard();
					cout << endl << endl << "Sorry, You have lost the game!" << endl;
				}
				else if (*opp_result == 2)
				{
					system("clear");
					obj.drawboard();
					cout << endl << endl << "The Game has been Drawn!" << endl;
				}
			}
		}
	}
}
void load_file() // This will load all the data from High Score file into the array. 
{
	fstream in;
	in.open("Score.txt", ios::in);
	for (int i = 0; i < 3; i++)
		in >> record[i];
	in.close();
}
int main()
{
	// FILE 1
	fstream in;
	int status;
	bool fh = true;
	//bool check = true;
	while (fh)
	{
		load_file();
		system("clear");
		cout << endl << "\t\t\tWELCOME TO TIC-TAC-TOE!" << endl << endl
			<< "Please select from below" << endl
			<< "1) Start Game (Human vs Computer)" << endl
			<< "2) Display High Scores" << endl
			<< "3) Exit Game" << endl
			<< "Your Option: ";
		int choice;
		cin >> choice;
		switch (choice)
		{
		case 1:
			status = open(pipe_file, O_WRONLY);
			write(status, &fh, sizeof(bool));
			close(status);
			main_program();
			break;
		case 2:
			cout << setw(15) << "Human wins: " << setw(5) << " " << record[0] << endl;
			cout << setw(15) << "Computer wins: " << setw(5) << " " << record[1] << endl;
			cout << setw(15) << "Draws played: " << setw(5) << " " << record[2] << endl << endl;
			break;
		case 3:
			fh = false;
			status = open(pipe_file, O_WRONLY);
			write(status, &fh, sizeof(bool));
			close(status);
			cout << "Thank you for playing!!" << endl << endl;
			exit(0);
			break;
		default:
			cout << "Invalid choice entered. Please try again... " << endl << endl;
		}
		cin.ignore();
		string temp;
		cout << "Press Enter to continue...." << endl;
		getline(cin, temp);
	}
	return 0;
}



