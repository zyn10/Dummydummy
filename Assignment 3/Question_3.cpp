#include <fstream>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <ctime>
#include <pthread.h>
using namespace std;
void Mergesort(int[], int, int, int);
void Merge(int[], int, int, int, int);
void Selection_Sort(int *, int);
void Insertion_Sort(int *, int);
void quicksort(int[], int, int);
int partition(int[], int, int);
void *MergeSort(void *arg);
void *SelectionSort(void *arg);
void *QuickSort(void *arg);
void *InsertionSort(void *arg);
const time_t ctt = time(0);
int arr_size;
int rand_number(); 
int main() {
	srand(time(0));
	cout << "Enter size of your array: ";
	cin >> arr_size;
	int *arr = new int[arr_size];
	for (int i = 0; i < arr_size; i++)
		arr[i] = rand() % 10000; // Only taking in numbers from 0-10000
	fstream obj1;
	obj1.open("Unsorted_00.txt", ios::out | ios::trunc);
	for (int i = 1; i <= arr_size / 4; i++)
		obj1 << rand_number() << " "; 
	obj1.close();
	obj1.open("Unsorted_01.txt", ios::out | ios::trunc);
	for (int i = (arr_size / 4) + 1; i <= arr_size / 2; i++)
		obj1 << rand_number() << " "; 
	obj1.close();
	obj1.open("Unsorted_02.txt", ios::out | ios::trunc);
	for (int i = (arr_size / 2) + 1; i <= (arr_size / 4) + (arr_size / 2); i++)
		obj1 << rand_number() << " "; 
	obj1.close();
	obj1.open("Unsorted_03.txt", ios::out | ios::trunc);
	for (int i = (arr_size / 4) + (arr_size / 2) + 1; i <= arr_size; i++)
		obj1 << rand_number() << " "; 
	obj1.close();
	pthread_t merge, selection, quick, insertion;
	pthread_create(&merge, NULL, MergeSort, NULL);
	pthread_create(&selection, NULL, SelectionSort, NULL);
	pthread_create(&insertion, NULL, InsertionSort, NULL);
	pthread_create(&quick, NULL, QuickSort, NULL);
	pthread_join(merge, NULL);
	pthread_join(selection, NULL);
	pthread_join(insertion, NULL);
	pthread_join(quick, NULL);
	fstream in, out;
	int temp;
	out.open("Sorted.txt", ios::out | ios::trunc);
	in.open("merge.txt");
	if (!in)
	{
		cout << "File doesn't exist";
		getchar();
		exit(0);
	}
	while (1)
	{
		in >> temp;
		out << temp << " ";
		if (in.eof())
			break;
	}
	in.close();
	in.open("selection.txt");
	if (!in)
	{
		cout << "File doesn't exist";
		getchar();
		exit(0);
	}
	while (1)
	{
		in >> temp;
		out << temp << " ";
		if (in.eof())
			break;

	}
	in.close();
	in.open("insertion.txt");
	if (!in)
	{
		cout << "File doesn't exist";
		getchar();
		exit(0);
	}
	while (1)
	{
		in >> temp;
		out << temp << " ";
		if (in.eof())
			break;

	}
	in.close();
	in.open("quick.txt");
	if (!in)
	{
		cout << "File doesn't exist";
		getchar();
		exit(0);
	}
	while (1)
	{
		in >> temp;
		out << temp << " ";
		if (in.eof())
			break;

	}
	in.close();
	out.close();
	system("pause");
	return 0;
}
int rand_number()
{
	return rand() % 100000; // Numbers are kept within the 1000000 range. 
}
void *MergeSort(void *arg)
{
	clock_t start, end;
	double cpu_time_used;
	start = clock();
	cout << "Thread ID for Merge Sort: " << pthread_self() << endl
		<< "Algorithm Name: Merge Sort" << endl
		<< "Start Time for Merge Sort: " << asctime(localtime(&ctt));
	fstream in;
	int index = 0;
	int *arr = new int[arr_size / 4];
	in.open("Unsorted_00.txt");
	if (!in)
	{
		cout << "File doesn't exist";
		getchar();
		exit(0);
	}
	while (1)
	{
		in >> arr[index++];
		if (in.eof())
			break;
	}
	in.close();
	Mergesort(arr, 0, (arr_size / 4) - 1, arr_size / 4);
	index = 0; 
	in.open("merge.txt", ios::out | ios::trunc);
	for (int i = 0; i < arr_size / 4; i++)
		in << arr[index++] << " ";
	in.close();
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	cout << "End Time for Merge Sort: " << asctime(localtime(&ctt))
		<< "Time consumed for Merge Sort: " << cpu_time_used << " seconds." << endl << endl;
}
void *QuickSort(void *arg)
{
	clock_t start, end;
	double cpu_time_used;
	sleep(1);
	start = clock();
	cout << "Thread ID for Quick Sort: " << pthread_self() << endl
		<< "Algorithm Name: Quick Sort" << endl
		<< "Start Time for Quick Sort: " << asctime(localtime(&ctt));
	fstream in;
	int index = 0;
	int *arr = new int[arr_size / 4];
	in.open("Unsorted_03.txt");
	if (!in)
	{
		cout << "File doesn't exist";
		getchar();
		exit(0);
	}
	while (1)
	{
		in >> arr[index++];
		if (in.eof())
			break;
	}
	in.close();
	quicksort(arr, 0, (arr_size / 4) - 1);
	index = 0; 
	in.open("quick.txt", ios::out | ios::trunc);
	for (int i = 0; i < arr_size / 4; i++)
		in << arr[index++] << " ";
	in.close();
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	cout << "End Time for Quick Sort: " << asctime(localtime(&ctt))
		<< "Time consumed for Quick Sort: " << cpu_time_used << " seconds." << endl << endl;
}
void *SelectionSort(void *arg)
{
	clock_t start, end;
	double cpu_time_used;
	sleep(2);
	start = clock();
	cout << "Thread ID for Selection Sort: " << pthread_self() << endl
		<< "Algorithm Name: Selection Sort" << endl
		<< "Start Time for Selection Sort: " << asctime(localtime(&ctt));
	fstream in;
	int index = 0;
	int *arr = new int[arr_size / 4];
	in.open("Unsorted_01.txt");
	if (!in)
	{
		cout << "File doesn't exist";
		getchar();
		exit(0);
	}
	while (1)
	{
		in >> arr[index++];
		if (in.eof())
			break;
	}
	in.close();
	Selection_Sort(arr, arr_size / 4);
	index = 0; 
	in.open("selection.txt", ios::out | ios::trunc);
	for (int i = 0; i < arr_size / 4; i++)
		in << arr[index++] << " ";
	in.close();
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	cout << "End Time for Selection Sort: " << asctime(localtime(&ctt))
		<< "Time consumed for Selection Sort: " << cpu_time_used << " seconds." << endl << endl;
}
void *InsertionSort(void *arg)
{
	clock_t start, end;
	double cpu_time_used;
	sleep(3);
	start = clock();
	cout << "Thread ID for Insertion Sort: " << pthread_self() << endl
		<< "Algorithm Name: Insertion Sort" << endl
		<< "Start Time for Insertion Sort: " << asctime(localtime(&ctt));
	fstream in;
	int index = 0;
	int *arr = new int[arr_size / 4];
	in.open("Unsorted_02.txt");
	if (!in)
	{
		cout << "File doesn't exist";
		getchar();
		exit(0);
	}
	while (1)
	{
		in >> arr[index++];
		if (in.eof())
			break;
	}
	in.close();
	Insertion_Sort(arr, arr_size / 4);
	index = 0; 
	in.open("insertion.txt", ios::out | ios::trunc);
	for (int i = 0; i < arr_size / 4; i++)
		in << arr[index++] << " ";
	in.close();
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	cout << "End Time for Insertion Sort: " << asctime(localtime(&ctt))
		<< "Time consumed for Insertion Sort: " << cpu_time_used << " seconds." << endl << endl;
}
void Merge(int arr[], int low, int high, int mid, int s)
{
	int i, j, k, *b;
	b = new int[s];
	i = low;
	k = low;
	j = mid + 1;
	while (i <= mid && j <= high)
	{
		if (arr[i] < arr[j])
		{
			b[k] = arr[i];
			k++;
			i++;
		}
		else
		{
			b[k] = arr[j];
			k++;
			j++;
		}
	}
	while (i <= mid)
	{
		b[k] = arr[i];
		k++;
		i++;
	}
	while (j <= high)
	{
		b[k] = arr[j];
		k++;
		j++;
	}
	for (i = low; i < k; i++)
	{
		arr[i] = b[i];
	}
}
void Mergesort(int arr[], int low, int high, int size)
{
	if (low < high) {
		int mid = (low + high) / 2;
		Mergesort(arr, low, mid, size);
		Mergesort(arr, mid + 1, high, size);
		Merge(arr, low, high, mid, size);
	}
}
void Selection_Sort(int *arr, int n)
{
	int min, temp;
	for (int i = 0; i < n - 1; i++)
	{
		min = i;
		for (int j = i + 1; j < n; j++)
			if (arr[j] < arr[min])
				min = j;
		temp = arr[i];
		arr[i] = arr[min];
		arr[min] = temp;
	}
}
void Insertion_Sort(int *arr, int n)
{
	int j, temp;
	for (int i = 1; i < n; i++)
	{
		temp = arr[i];
		j = i - 1;
		while (j >= 0 && arr[j] > temp)
		{
			arr[j + 1] = arr[j];
			j = j - 1;
		}
		arr[j + 1] = temp;
	}
}

int partition(int arr[], int l, int u)
{
	int v, i, j;
	v = arr[l];
	i = l;
	j = u + 1;
	do {
		do {
			i++;
		} while (arr[i] < v && i <= u);
		do {
			j--;
		} while (v < arr[j]);
		if (i < j) {
			int temp;
			temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
		}
	} while (i < j);
	arr[l] = arr[j];
	arr[j] = v;
	return j;
}
void quicksort(int arr[], int l, int u)
{
	int j = 0;
	if (l < u) {
		j = partition(arr, l, u);
		quicksort(arr, l, j - 1);
		quicksort(arr, j + 1, u);
	}
}

