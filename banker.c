#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define ARRAY_SIZE 20

int main( int argc, char *argv[] )
{
	int  retVal, i, j, k, l, pNum, rNum, reqProcess, runNum, safe, completeNum, stacklen, rejects, lastReject = -1, test = 0;
	int	 need[ARRAY_SIZE][ARRAY_SIZE];
	int	 needD[ARRAY_SIZE][ARRAY_SIZE][ARRAY_SIZE];	
	int  alloc[ARRAY_SIZE][ARRAY_SIZE];
	int  allocD[ARRAY_SIZE][ARRAY_SIZE][ARRAY_SIZE];
	int	 max[ARRAY_SIZE][ARRAY_SIZE];
	int  avail[ARRAY_SIZE];
	int  availD[ARRAY_SIZE][ARRAY_SIZE];
	int  req[ARRAY_SIZE];
	int  stack[ARRAY_SIZE];							
	FILE *inFile;
	char line[ARRAY_SIZE][ARRAY_SIZE];
	char word[ARRAY_SIZE][ARRAY_SIZE];
	
	if(argv[1] == NULL)
	{
		printf( "\nIncomplete input!!\n\n" );
		return( retVal );
	}
	
	inFile = fopen(argv[1], "rt");		
	if(!inFile)
	{
		printf( "\nInput file cannot be loaded!!\n\n" );
		return( retVal );
	}
	i = 0;
	while(fgets(line[i], sizeof(line[i]), inFile)!=NULL)		
		i++;
	
	pNum = atoi(line[0]);										
	rNum = atoi(line[2]);										
	printf("\nThere are %d processes in the system.\n\nThere are %d resource types.\n\nThe Allocation Matrix is...\n   A B C D", pNum, rNum);
	
	for(i=4; i<(9+(2*pNum)); i++)
	{
		k = 0;
		l = 0;
		for(j=0; line[i][j]!='\0'; j++)							
		{
			if(line[i][j] != ' ' && line[i][j] != ':')			
			{
				word[k][l] = line[i][j];
				l++;
			}
			else
			{
				k++;
				l = 0;
			}
		}
		if(i>=4 && i<(4+pNum))
			for(j=0; j<=k; j++)
			{
				alloc[i-4][j]  = atoi(word[j]);					
				allocD[0][i-4][j] = alloc[i-4][j];				
			}
		else if(i>=(5+pNum) && i<(5+(2*pNum)))
			for(j=0; j<=k; j++)
				max[i-(5+pNum)][j] = atoi(word[j]);			
		else if(i==(6+(2*pNum)))
			for(j=0; j<=k; j++)
			{
				avail[j]  = atoi(word[j]);						
				availD[0][j] = avail[j];
			}
		else if(i==(8+(2*pNum)))
		{
			reqProcess = atoi(word[0]);							
			for(j=1; j<=k; j++)
				req[j-1] = atoi(word[j]);					
		}
		
		for(j=0; j<ARRAY_SIZE; j++)
			for(k=0; k<ARRAY_SIZE; k++)
				word[j][k] = NULL;								
	}
	
	for(i=0; i<pNum; i++)
	{
		printf("\n%d:", i);
		for(j=0; j<rNum; j++)
			printf(" %d", alloc[i][j]);
	}
	
	printf("\n\nThe Max Matrix is...\n   A B C D");
	for(i=0; i<pNum; i++)
	{
		printf("\n%d:", i);
		for(j=0; j<rNum; j++)
			printf(" %d", max[i][j]);
	}

	printf("\n\nThe Need Matrix is...\n   A B C D");
	for(i=0; i<pNum; i++)
	{
		printf("\n%d:", i);
		for(j=0; j<rNum; j++)
		{
			need[i][j]  = max[i][j] - alloc[i][j];
			needD[0][i][j] = need[i][j];
			printf(" %d", need[i][j]);
		}
	}
	
	printf("\n\nThe Available Vector is...\nA B C D\n");
	for(i=0; i<rNum; i++)
		printf("%d ", avail[i]);							
	C:runNum = 0;
	for(i=0; i<pNum; i++)
	{
		for(j=0; j<rNum; j++)
			if(availD[0][j] - needD[0][i][j] < 0)
			{
				runNum++;
				j = rNum;
			}
	}
	
	if(runNum == pNum)
	{
		if(test == 0)						
			printf("\n\nTHE SYSTEM IS IN UNSAFE STATE!\n\n");
		else
			printf("\n\nThis request cannot be granted!\n\n");
		return( retVal );
	}
	else
	{
		stacklen = 0;						
		for(k=0; k<pNum; k++)				
		{
			rejects = 0;					
			for(i=0; i<pNum; i++)
			{
				if(i == lastReject)			
				{
					rejects++;					
					goto B;
				}
				j = 0;
				while(j<stacklen && stacklen > 0)						
				{
					if(i == stack[j])
					{
						rejects++;
						goto B;										
					}
					j++;
				}
				for(j=0; j<rNum; j++)									
				{
					if(availD[stacklen][j] - needD[stacklen][i][j] < 0)
					{
						rejects++;
						goto B;
					}
				}
				stack[stacklen] = i;									
				stacklen++;												

				if(stacklen == pNum)									
				{
					printf("\n\nThese processes can be executed in the order: ");
					for(l=0; l<stacklen; l++)
						printf("%d ", stack[l]);						
					goto A;												
				}
				for(j=0; j<rNum; j++)
				{
					availD[stacklen][j] = availD[stacklen-1][j] + allocD[stacklen-1][i][j];
					for(l=0; l<pNum; l++)
					{
						allocD[stacklen][l][j] = allocD[stacklen-1][l][j];
						needD[stacklen][l][j] = max[l][j] - allocD[stacklen][l][j];
					}
				}
				i = pNum;
				
				B:if(rejects >= pNum)
				{
					
					stacklen--;											
					lastReject = stack[stacklen];						

					i = pNum;
				}
			}
		}
		if(test == 0)
			printf("\n\nTHE SYSTEM IS IN UNSAFE STATE!\n\n");
		else
			printf("\n\nThis request cannot be granted!\n\n");
		return( retVal );
	}
	
	A:if(test == 0)														
	{
		printf("\n\nTHE SYSTEM IS IN A SAFE STATE!\n\nThe Request Vector is...\n  A B C D\n%d:", reqProcess);
		for(i=0; i<rNum; i++)
			printf("%d ", req[i]);
		for(j=0; j<rNum; j++)
		{
			if(avail[j] - req[j] < 0)								
			{
				printf("\n\nRequest cannot be granted as it exceeds Available Vector!\n\n");
				return( retVal );
			}
			else
			{														
				availD[0][j] = avail[j] - req[j];
				allocD[0][reqProcess][j] = alloc[reqProcess][j] + req[j];
			}
		}
		test = 1;													
		goto C;															
	}
	else
	{
		printf("\n\nThis request can be granted!\n\n");
	}
	
	return( retVal);
}