#define _CRT_SECURE_NO_WARNINGS
#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <stdio.h>
#include <stdlib.h>
#include "Simulation.h"

int read_data(const char* file_name, Simulation* area) {

	FILE* file = fopen(file_name, "r");
	if (!file) {
		perror("Error opening the initialization file");
		return -1;
	}

	if (fscanf(file,"%d %d\n", &area->x_cord, &area->y_cord) != 2) {
		perror("Error reading area size");
		return -1;
	}

	if (fscanf(file, "%d\n", &area->person_number) != 1) {
		perror("Error reading persons");
		return -1;
	}

	if (area->person_number > MAX_PERSONS) {
		perror("Too many persons");
		return -1;
	}

	area->persons = (Person*)malloc(sizeof(Person) * area->person_number);
	if (!area->persons) {
		perror("Memory allocation for persons failed");
		fclose(file);
		return -1;
	}

	for (int i = 0; i < area->person_number; i++) {
		if (fscanf(file, "%d %d %d %d %d %d\n",
			&area->persons[i].id,
			&area->persons[i].x_coord,
			&area->persons[i].y_coord,
			&area->persons[i].movement_patern,
			&area->persons[i].infection_state,
			&area->persons[i].amplitude) != 6) {

			perror("Error reading persons data");
			free(area->persons);
			fclose(file);
			return -1;
		}
		area->persons[i].infection_count = 0;
		area->persons[i].infection_time = 0;
		area->persons[i].imunity_time = 0;
	}

	fclose(file);
	return 0;
}

int write_data(const char* file_name, Simulation* area) {

	FILE* file = fopen(file_name, "w");

	if (!file) {
		perror("Error opening the initialization file");
		return -1;
	}

	fprintf(file, "%d %d\n", area->x_cord, area->y_cord);

	fprintf(file, "%d\n", area->person_number);

	for (int i = 0; i < area->person_number; i++) {
		fprintf(file,"%d %d %d %d\n",
			area->persons[i].x_coord,
			area->persons[i].y_coord,
			area->persons[i].infection_state,
			area->persons[i].infection_count);
	}

	fclose(file);
	return 1;

}
#endif  // INITIALIZATION_H