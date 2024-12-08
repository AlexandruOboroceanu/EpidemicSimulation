#define _NO_CRT_SECURE_WARNINGS
#include <iostream>
#include "Simulation.h"
#include "Initialization.h"
#include <cstring>

void print_menu() {
	printf("Welcome to the Epidemic Simulator\n");
	printf("Choose an option from below\n");
	printf("1.Read the input file\n");
	printf("2.Display the contents of the input file\n");
    printf("3.Run the simulation\n");
    printf("4.Run the simulation without printing every step\n");
    printf("5.Print the simulation ouput file\n");
    printf("6.Run the simulation in parralel\n");
    printf("7.Verify the serial implementation and the parallel one\n");
    printf("8.OpenMP v1\n");
    printf("9.OpenMP v2\n");
    printf("10.Exit\n");
}

char file_name[25] = ""; // global variable;

void menu_logic() {

    int choice;
    Simulation area;

    while (1) {
        print_menu();

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1: {
            printf("Enter the name of the input file:");
            scanf("%s", file_name);
            read_data(file_name, &area);
            break;
        }
        case 2:
            print_simulation(&area);
            break;
        case 3: {
            int time;  
            printf("Enter your time period: ");
            scanf("%d", &time);
            run_simulation(&area, time, 0);
            break;
        }
        case 4: {
            int time;  
            printf("Enter your time period: ");
            scanf("%d", &time);
            run_simulation(&area, time, 1);
            break;
        }
        case 5: {
            char output_file[50];
            char base_name[50];

            strncpy(base_name, file_name, sizeof(base_name) - 1);
            base_name[sizeof(base_name) - 1] = '\0';

            char* dot = strrchr(base_name, '.');
            if (dot && strcmp(dot, ".txt") == 0) {
                *dot = '\0';
            }

            snprintf(output_file, sizeof(output_file), "%s_out.txt", base_name);

            printf("Making output file...\n");
            write_data(output_file, &area);
            break;
        }
        case 6: {
            int time;  
            int num_of_threads;

            printf("Enter your time period: ");
            scanf("%d", &time);

            printf("Enter the number of threads: ");
            scanf("%d", &num_of_threads);

            run_simulation_parallel(&area, time,num_of_threads);
            break;
        }
        case 7:
            verify_simulation(&area, 10);
            break;
        case 8: {
            int time;

            printf("Enter your time period: ");
            scanf("%d", &time);

            v1(&area, time);
            break;
        }
        case 9: {
            int time;

            printf("Enter your time period: ");
            scanf("%d", &time);

            v2(&area, time);
            break;
        }
        case 10:
            printf("EXITING...\n");
            return;
        default:
            printf("Invalid choice! Please try again.\n");
            break;
        }
    }
}

