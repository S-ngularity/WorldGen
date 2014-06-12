#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define SIZE 10

HANDLE hConsole;

void printColor(int n);

int main()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	int m[SIZE][SIZE], i, j;


	for(i = 0; i < SIZE; i++)
		for(j = 0; j < SIZE; j++)
			m[i][j] = rand() % 7;		

	/*
for(i = 0; i < SIZE; i++)
		for(j = 0; j < SIZE; j++){
			if(i != 0 || i != SIZE - 1){
				m[i][j] = (int)((m[i+1][j] + m[i+1][j]) / 2);
			}
			else{
				if(i != SIZE -1)
				m[i][j] = (int)(m[i+1][j] /2);
				else
					m[i][j] = (int)(m[i-1][j+1] /2);
			}
		}
	*/

	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE; j++)
			printColor(m[i][j]);

		printf("\n\n");
	}

	SetConsoleTextAttribute(hConsole, 7);

	return 0;
}

void printColor(int n)
{
	switch(n)
	{
		case 0 :
			SetConsoleTextAttribute(hConsole, 1);
			printf(" %d ", n);
			break;

		case 1 :
			SetConsoleTextAttribute(hConsole, 11);
			printf(" %d ", n);
			break;

		case 2 :
			SetConsoleTextAttribute(hConsole, 15);
			printf(" %d ", n);
			break;
		
		case 3 :
			SetConsoleTextAttribute(hConsole, 2);
			printf(" %d ", n);
			break;
		
		case 4 :
			SetConsoleTextAttribute(hConsole, 8);
			printf(" %d ", n);
			break;
		
		case 5 :
			SetConsoleTextAttribute(hConsole, 7);
			printf(" %d ", n);
			break;
		
		case 6 :
			SetConsoleTextAttribute(hConsole, 12);
			printf(" %d ", n);
			break;				
	
	}
}