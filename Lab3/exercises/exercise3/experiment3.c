// Copyright by Adam Kinsman, Henry Ko and Nicola Nicolici
// Developed for the Embedded Systems course (COE4DS4)
// Department of Electrical and Computer Engineering
// McMaster University
// Ontario, Canada

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "sys/alt_alarm.h"
#include "alt_types.h"
#include "system.h"
#include "altera_avalon_performance_counter.h"

#define ARRAY_SIZE 500

// For performance counter
void *performance_name = PERFORMANCE_COUNTER_0_BASE;

int sum_function(int *data_array, int size){
	int i;
	int sum=0;
	for (i=0; i<size; i++){
		if (data_array[i] < 32768){
			sum += data_array[i];
		}
	}
	return sum;
}

void bubble_sort(int *data_array, int size)
{

	int i = 0, temp, sorted=0;

	while (!sorted){
		for (i = 0; i<size-1; i++){
			if(data_array[i]>data_array[i+1]){
				temp = data_array[i];
				data_array[i]=data_array[i+1];
				data_array[i+1]=temp;
			}
		}

		for	(i=0;i<size-1;i++){
			if(data_array[i]<data_array[i+1]){
				sorted=1;
			}
			else{sorted=0;}
		}
	}

}

int main()
{ 
	int data_set[ARRAY_SIZE];
	int i, j, sum;
	int results[10];
	alt_u64 average=0;


	
	for (j = 0; j < 10; j++) {
		printf("Generating random data...\n");
		for (i = 0; i < ARRAY_SIZE; i++) {
			data_set[i] = rand() % 65536;
		}


		printf("Start sorting\n");

		bubble_sort(data_set, ARRAY_SIZE);

		printf("Starting summing...\n");
		// For performance counter
		PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
		// Start the performance counter
		PERF_START_MEASURING(performance_name);

		// Start performance counter
		PERF_BEGIN(performance_name, 1);

		sum = sum_function(data_set, ARRAY_SIZE);

		// Stop performance counter
		PERF_END(performance_name, 1);

		// Stop the performance counter
		PERF_STOP_MEASURING(performance_name);

		printf("\nRun %d\nSum: %d\n",(j+1),sum);

		printf("PC: %d\n", perf_get_section_time(performance_name, 1));

		results[j]=perf_get_section_time(performance_name, 1);
	}
	
	for(i=0; i<10; i++){
		average+=results[i];
	}
	average=average/10;
	printf("\n\n---Average: %d ",average);

  /* Event loop never exits. */
  while (1);

  return 0;
}
