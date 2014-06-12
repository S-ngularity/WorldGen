#include <stdio.h>
#include <stdlib.h>

#define SIZE 10


int main()
{
	int m[SIZE][SIZE], i, j;


	for(i = 0; i < SIZE; i++)
		for(j = 0; j < SIZE; j++)
			m[i][j] = rand() % 9;


for(i = 0; i < SIZE; i++)
		for(j = 0; j < SIZE; j++){
			if(i != 0 || i != SIZE - 1){
				m[i][j] = (int)((m[i+1][j] + m[i+1][j]) / 2);
			}
			else{
				if(i != SIZE -1)
				m[i][j] = (int)(m[i+1][j] /2)
				else
					m[i][j] = (int)(m[i-1][j+1] /2)
			}
		}

	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE; j++)
			printf("%d  ", m[i][j]);

		printf("\n\n");
	}

}



