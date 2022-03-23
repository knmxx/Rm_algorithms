#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
using namespace std;


int indexOf(int* array, int size, int value)
{
	for (int i = 0; i < size; i++)
	{
		if (array[i] == value)
			return i;
	}
	return -1;
}

int randint(int _min, int _max) { return _min == _max ? _min : _min + rand() % (_max - _min); }


bool event_happen(double probability) { return randint(0, 1000) < 1000.0 * probability; }

int argmin(int* array, int size)
{
	int min_index = 0;
	for (int i = 1; i < size; i++)
	{
		if (array[min_index] > array[i])
			min_index = i;
	}
	return min_index;
}
int argmin(int* array, int size,int first_id)
{
	int min_index = first_id;
	for (int i = first_id; i < size; i++)
	{
		if (array[min_index] > array[i])
			min_index = i;
	}
	return min_index;
}
int argmax(int* array, int size)
{
	int max_index = 0;
	for (int i = 1; i < size; i++)
	{
		if (array[i] > array[max_index])
			max_index = i;
	}
	return max_index;
}
void draw_line(int line_width)
{
	while (line_width--)
		printf("-");
	printf("\n");
}

int u(int min, int max)
{
	return min == max ? min : min + (rand() % (max - min));
}

int sum(int* array, int size)
{
	int result = 0;
	for (int i = 0; i < size; i++)
	{
		result += array[i];
	}
	return result;
}