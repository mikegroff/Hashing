/*
    Michael Groff
    COP 3502 Fall 2016
    HashyHash
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "HashyHash.h"

int nextPrime(int n);

//while i was reading the pdf for this assignment i noticed that the insert delete and
//search functions were very similar since they both probed through the table looking for
//specific values. instead of coding this up three times i wrote one function that i could
//call from the other functions in different ways.

int probingthetable(int position, HashTable *h, int searchval, int probetype);

int probingthetable(int position, HashTable *h, int searchval, int probetype)
{
    int i=0, index=position;

    while(1)
    {
        if(h->probing == LINEAR)
            index = (position+i)%h->capacity; // mods the hash value with the table then probes linearly

        else if(h->probing == QUADRATIC)
            index = (position+i*i)%h->capacity;

        else
            return HASH_ERR;

        //if inserting breaks when it reaches first open position
        if(probetype == 0 && (h->array[index] == DIRTY || h->array[index] == UNUSED))
            break;
        //breaks while searching of deleting if the value cannot be found in the array
        if(probetype == 1 && h->array[index] == UNUSED)
            return -1;
        //breaks while searching of deleting if the value if found
        if(probetype == 1 && h->array[index] == searchval)
            break;
        //breaks the probing loop if we go through as many iterations of this loop as the size of the table
        if(i++ == h->capacity)
            return -1;

        h->stats.collisions++;
    }
    return index;
}
//using professors code as an auxiliary function here
int nextPrime(int n)
{
	int i, root, keepGoing = 1;

	if (n % 2 == 0)
		++n;

	while (keepGoing)
	{
		keepGoing = 0;
		root = sqrt(n);

		for (i = 3; i <= root; i++)
		{
			if (n % i == 0)
			{
				// Move on to the next candidate for primality. Since n is odd, we
				// don't want to increment it by 1. That would give us an even
				// integer greater than 2, which would necessarily be non-prime.
				n += 2;
				keepGoing = 1;

				// Leave for-loop. Move on to next iteration of while-loop.
				break;
			}
		}
	}
	return n;
}

HashTable *makeHashTable(int capacity)
{
    int cap, i;
    HashTable *NewHash = NULL; // creating a new HashTable

    //
    if(capacity <= 0) // setting capacity
        cap = DEFAULT_CAPACITY;
    else
        cap = capacity;

    NewHash = malloc(sizeof(HashTable)); // dynamically allocating room for the structure

    NewHash->array = malloc(sizeof(int)*cap); // dynamically allocating room for the array

    for(i=0; i<cap; i++)
        NewHash->array[i] = UNUSED; // filling array with unused positions

    //all initializations
    NewHash->capacity = cap;
    NewHash->probing = LINEAR;
    NewHash->size = 0;
    NewHash->stats.collisions = 0;
    NewHash->stats.opCount = 0;

    if(NewHash == NULL || NewHash->array == NULL)//checking for errors
        return NULL;

    return NewHash;
}

HashTable *destroyHashTable(HashTable *h)
{
    if(h == NULL)//checking the table to avoid freeing a null pointer
        return NULL;

    if(h->array == NULL)//checking the table to avoid freeing a null pointer
    {
        free(h);
        return NULL;
    }

    free(h->array);
    h->array = NULL;//setting the array to null
    free(h);//not setting h to null as i expect this to take place when this function is called

    return NULL;
}

int setProbingMechanism(HashTable *h, ProbingType probing)
{
    if(h == NULL || (probing != LINEAR && probing != QUADRATIC))
        return HASH_ERR;

    h->probing = probing;//setting probing, returning hash err if no probing type is input

    return HASH_OK;
}

int setHashFunction(HashTable *h, unsigned int (*hashFunction)(int))
{
    if(h == NULL)
        return HASH_ERR;

    h->hashFunction = hashFunction;//setting hash function

    return HASH_OK;
}

int isAtLeastHalfEmpty(HashTable *h)
{
    if(h == NULL || h->capacity == 0 || (h->size > (h->capacity/2)))// checking has table size vs capacity
        return 0;

    return 1;
}

int expandHashTable(HashTable *h)
{
    if(h == NULL || h->array == NULL)
        return HASH_ERR;

    int i,k, *oldarray, cap = h->capacity;

    oldarray = malloc(sizeof(int)*cap);//creating an array to hold values of old array

    for(k=0; k<h->capacity; k++)//filling said array
        oldarray[k] = h->array[k];

    free(h->array);//freeing table array to allow for expansion
    h->array = NULL;

    //expanding array accordingly
    if(h->probing == LINEAR)
    {
        h->capacity = 2*h->capacity+1;
        h->array = malloc(sizeof(int)*h->capacity);
    }
    else if(h->probing == QUADRATIC)
    {
        h->capacity = nextPrime(2*h->capacity + 1);
        h->array = malloc(sizeof(int)*h->capacity);
    }
    else
        return HASH_ERR;

    if(h->array == NULL)//checking if array was allocated correctly
        return HASH_ERR;

    for(i=0; i<h->capacity; i++)
        h->array[i] = UNUSED;//filling array with unused positions

    for(i=0; i<cap; i++)//hashing the old values into the new table
    {
        if(oldarray[i] != UNUSED && oldarray[i] != DIRTY)
        {
            insert(h, oldarray[i]);
            h->size--;//we want size to remain the same after expanding
            h->stats.opCount--;//we want the operation count to remain the same after expanding
        }
    }
    free(oldarray);//freeing the old array as it is no longer needed

    return HASH_OK;
}
int insert(HashTable *h, int key)
{
    if(h == NULL || h->hashFunction == NULL || h->array == NULL)
        return HASH_ERR;

    if(!isAtLeastHalfEmpty(h))
        expandHashTable(h);//checking the table for room

    int hh = probingthetable(h->hashFunction(key), h, UNUSED, 0);//performing probe one time

    if(hh == -1)
        return HASH_ERR;

    h->array[hh] = key;//inserting the value into the table
    h->stats.opCount++;
    h->size++;

    return HASH_OK;
}

int search(HashTable *h, int key)
{
    if(h == NULL || h->hashFunction == NULL)
        return -1;

    h->stats.opCount++;

    //returning the index found form the probe function( -1 if not found)
    return probingthetable(h->hashFunction(key), h, key, 1);
}

int delete(HashTable *h, int key)
{
    if(h == NULL || h->hashFunction == NULL)
        return -1;

    h->stats.opCount++;

    int hh = probingthetable(h->hashFunction(key), h, key, 1);

    if(hh == -1)
        return -1;

    h->array[hh] = DIRTY;//setting the deleted value to DIRTY
    h->size--;

    return hh;//returning deleted index
}

double difficultyRating(void)
{
    return 2.5;
}

double hoursSpent(void)
{
    return 12.0;
}
unsigned int hash(int key)
{
	int retval = 0;

	while (key > 0)
	{
		retval *= 37;
		retval += key % 10;
		key = key / 10;
	}

	return retval;
}

void printHash(HashTable *h)
{
    int i;

    printf("\nHASH TABLE:\n");

    for (i = 0; i < h->capacity; i++)
        printf("%d: %d\n",i ,h->array[i]);

    printf("capacity: %d, size: %d, ops: %d, collisions: %d\n\n", h->capacity, h->size, h->stats.opCount, h->stats.collisions);
}


int main(void)
{

	int i;
    int option, num, retval, keepgoing = 1;
    srand(time(NULL));

	HashTable *h = makeHashTable(5);
	setHashFunction(h, hash);
	setProbingMechanism(h, QUADRATIC);

	printf("How many values would you like to randomly insert?\n");
	scanf("%d", &option);

	for (i = 0; i < option; i++){
		num = rand() % 100 + 1;
        printf("Inserting %d\n", num);
		insert(h, num);
	}

    printHash(h);

    while(keepgoing == 1){

        printf("Input: 1 to Search; 2 to Delete; 3 to Insert; 0 to Exit\n");
        scanf("%d", &option);

        if(option == 0)
        break;

        if(option == 1)
        {
            printf("What value would you like to search for?");
            scanf("%d", &num);

            if((retval = search(h, num)) == -1)
                printf("\nValue not found.\n");
            else
                printf("\nValue was found in index %d\n\n", retval);
        }

        else if(option == 2)
        {
            printf("What value would you like to delete?");
            scanf("%d", &num);

            if((retval = delete(h, num)) == -1)
                printf("\nValue not found. Deletion failed\n\n");
            else{
                printHash(h);
                printf("Values was found and deleted.\n\n");
            }

        }

        else if(option == 3)
        {
            printf("What value would you like to insert?");
            scanf("%d", &num);

            if((retval = insert(h, num)) == HASH_ERR)
                printf("\nInsertion failed\n\n");
            else{
                printHash(h);
                printf("Values was inserted.\n\n");
            }
        }

        else
            printf("Wrong input parameter.\n");

    }

	h = destroyHashTable(h);

	return 0;
}
