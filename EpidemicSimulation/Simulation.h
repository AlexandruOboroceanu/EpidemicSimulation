#define HAVE_STRUCT_TIMESPEC
#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <time.h>
#include <pthread.h>
#include <omp.h>

#define MAX_PERSONS 1000000 // the max population is 100k
#define INFECTED_DURATION 10   
#define IMMUNE_DURATION 5  

typedef struct {
    int id;
    int x_coord;
    int y_coord;
    int movement_patern;
    int infection_state;
    int amplitude;
    int infection_count;
    int infection_time;
    int imunity_time;
} Person;

typedef struct {
    int x_cord;
    int y_cord;
    int person_number;
    Person* persons;
} Simulation;

typedef struct {
    Simulation* area;  
    int start_index;   
    int end_index;     
    pthread_barrier_t* barrier;  
    int sim_time;
} ThreadData;

void movement(Person* person, int max_x, int max_y) {

    switch (person->movement_patern) {

        case 0: {// person moves up 
                    if (person->y_coord + person->amplitude < max_y) {// check if i goes outbound
                         person->y_coord = person->y_coord + person->amplitude;
                    }
                    else
                         person->movement_patern = 1; // if it goes switch from north to south 
        break;
        }
        case 1: {// person moves down
                    if (person->y_coord - person->amplitude >= 0) {// check if i goes outbound
                         person->y_coord = person->y_coord - person->amplitude;
                    }
                    else
                         person->movement_patern = 0; // if it goes switch from north to south 
        break;
        }

        case 2: {// person moves right
                    if (person->x_coord + person->amplitude < max_x) {// check if i goes outbound
                         person->x_coord = person->x_coord + person->amplitude;
                    }
                    else
                         person->movement_patern = 3; // if it goes switch from east to west 
        break;
        }

        case 3: {// person moves left
                    if (person->x_coord - person->amplitude >= 0) {// check if i goes outbound
                         person->x_coord = person->x_coord - person->amplitude;
                    }
                    else
                         person->movement_patern = 2; // if it goes switch from west to east 
        break;
        }
    }
}

void update_persons_state(Simulation* area) {

    for (int i = 0; i < area->person_number; i++) {
        Person* person = &area->persons[i];


        if (person->infection_state == 0) { // INFECTED
            person->infection_time--;
            if (person->infection_time <= 0) {
                person->infection_state = 2;
                person->imunity_time = IMMUNE_DURATION;
            }
        }
        else if (person->infection_state == 2) { // IMMUNE
            person->imunity_time--;
            if (person->imunity_time <= 0) {
                person->infection_state = 1;
            }
        }

        if (person->infection_state == 1) { // SUSCEPTIBLE
            for (int j = 0; j < area->person_number; j++) {
                if (i != j) {
                    Person* other_person = &area->persons[j];

                    if (other_person->infection_state == 0
                        && other_person->x_coord == person->x_coord
                        && other_person->y_coord == person->y_coord) {

                        person->infection_state = 0;
                        person->infection_time = INFECTED_DURATION;
                        person->infection_count++;
                        break;
                    }

                }
            }
        }

    }
}

void print_simulation(Simulation* area) {

    printf("Simulation Area: %d x %d\n", area->x_cord, area->y_cord);
    printf("Number of Persons: %d\n", area->person_number);

    for (int i = 0; i < area->person_number; i++) {
        printf("Person %d:\n", i + 1);
        printf("ID: %d\n", area->persons[i].id);
        printf("Coordinates: (%d, %d)\n", area->persons[i].x_coord, area->persons[i].y_coord);
        printf("Movement Pattern: %d\n", area->persons[i].movement_patern);
        printf("Infection State: %d\n", area->persons[i].infection_state);
        printf("Amplitutde: %d\n", area->persons[i].amplitude);
        printf("Infection Count: %d\n", area->persons[i].infection_count);
        printf("\n");
    }
}

void run_simulation(Simulation* area,int sim_time, int debug) {
    if (debug == 0) {
        for (int k = 0; k < sim_time; k++) {

            for (int i = 0; i < area->person_number; i++) {
                movement(&area->persons[i], area->x_cord, area->y_cord);
            }

            update_persons_state(area);
            printf("Time step %d:\n", k);
            print_simulation(area);
        }
    }
    else {
        clock_t start = clock();
        for (int k = 0; k < sim_time; k++) {

            for (int i = 0; i < area->person_number; i++) {
                movement(&area->persons[i], area->x_cord, area->y_cord);
            }

            update_persons_state(area);
        }
        clock_t end = clock();

        double duration = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("Total time for the simulation was: %.6f\n", duration);
    }
}

void* run_simulation_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    Simulation* area = data->area;

    Person* persons = area->persons;  
    int num_persons = area->person_number;
    int start_index = data->start_index;
    int end_index = data->end_index;
    int sim_time = data->sim_time;

    for (int k = 0; k < sim_time; k++) {
        for (int i = start_index; i < end_index; i++) {
            movement(&persons[i], area->x_cord, area->y_cord);
        }

        pthread_barrier_wait(data->barrier);

        for (int i = start_index; i < end_index; i++) {
            Person* person = &persons[i];

            if (person->infection_state == 0) {
                person->infection_time--;
                if (person->infection_time <= 0) {
                    person->infection_state = 2; // IMMUNE
                    person->imunity_time = IMMUNE_DURATION;
                }
            }
            else if (person->infection_state == 2) { // IMMUNE
                person->imunity_time--;
                if (person->imunity_time <= 0) {
                    person->infection_state = 1; // SUSCEPTIBLE
                }
            }

            if (person->infection_state == 1) { // SUSCEPTIBLE
                for (int j = 0; j < num_persons; j++) {
                    if (i != j) {
                        Person* other_person = &persons[j];

                        if (other_person->infection_state == 0 &&
                            other_person->x_coord == person->x_coord &&
                            other_person->y_coord == person->y_coord) {

                            person->infection_state = 0;
                            person->infection_time = INFECTED_DURATION;
                            person->infection_count++;
                            break;
                        }
                    }
                }
            }
        }

        pthread_barrier_wait(data->barrier);
    }

    return NULL;
}

void* run_simulation_parallel(Simulation* area, int sim_time, int num_of_threads) {

    pthread_t* threads = (pthread_t*)malloc(num_of_threads * sizeof(pthread_t));
    ThreadData* thread_data = (ThreadData*)malloc(num_of_threads * sizeof(ThreadData));
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, num_of_threads);  

    clock_t start_time = clock();

    int persons_per_thread = area->person_number / num_of_threads;

    for (int i = 0; i < num_of_threads; i++) {
        thread_data[i].area = area;
        thread_data[i].start_index = i * persons_per_thread;
        thread_data[i].end_index = (i == num_of_threads - 1) ? area->person_number : (i + 1) * persons_per_thread;
        thread_data[i].sim_time = sim_time;
        thread_data[i].barrier = &barrier;

        pthread_create(&threads[i], NULL, run_simulation_thread, (void*)&thread_data[i]);
    }

    for (int i = 0; i < num_of_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);

    clock_t end_time = clock();
    double duration = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Total time for parallel simulation: %.6f seconds\n", duration);

    free(threads);
    free(thread_data);

    return NULL;
}

bool compare_persons(Person* p1, Person* p2) {
    return p1->id == p2->id &&
        p1->x_coord == p2->x_coord &&
        p1->y_coord == p2->y_coord &&
        p1->infection_state == p2->infection_state &&
        p1->infection_count == p2->infection_count &&
        p1->infection_time == p2->infection_time &&
        p1->imunity_time == p2->imunity_time &&
        p1->movement_patern == p2->movement_patern &&
        p1->amplitude == p2->amplitude;
}

bool compare_simulations(Simulation* serial, Simulation* parallel) {
    if (serial->person_number != parallel->person_number) {
        printf("Person numbers do not match.\n");
        return false;
    }

    for (int i = 0; i < serial->person_number; i++) {
        if (!compare_persons(&serial->persons[i], &parallel->persons[i])) {
            printf("Mismatch found in person with ID: %d\n", serial->persons[i].id);
            return false;
        }
    }

    return true;
}

void verify_simulation(Simulation* area, int sim_time) {

    Simulation serial_area;
    Simulation parallel_area;

    memcpy(&serial_area, area, sizeof(Simulation));   
    memcpy(&parallel_area, area, sizeof(Simulation)); 

    run_simulation(&serial_area, sim_time,1);

    run_simulation_parallel(&parallel_area, sim_time, 8);

    if (compare_simulations(&serial_area, &parallel_area)) {
        printf("Simulation passed verification: serial and parallel results match.\n");
    }
    else {
        printf("Simulation verification failed: serial and parallel results do not match.\n");
    }
}

void v1(Simulation* area, int sim_time) {
    double start_time = omp_get_wtime();  

    for (int k = 0; k < sim_time; k++) {
#pragma omp parallel for schedule(static, 10)
        for (int i = 0; i < area->person_number; i++) {
            movement(&area->persons[i], area->x_cord, area->y_cord);
        }

        update_persons_state(area);
    }

    double end_time = omp_get_wtime();  
    double duration = end_time - start_time; 
    printf("Total time for parallel simulation V1: %.6f seconds\n", duration);
}

void v2(Simulation* area, int sim_time) {
    double start_time = omp_get_wtime();
#pragma omp parallel
    {
#pragma omp single
        {
            for (int k = 0; k < sim_time; k++) {
#pragma omp task
                {
                    for (int i = 0; i < area->person_number; i++) {
                        movement(&area->persons[i], area->x_cord, area->y_cord);
                    }
                }
#pragma omp task
                {
#pragma omp parallel for
                    for (int i = 0; i < area->person_number; i++) {
                        Person* person = &area->persons[i];

                        if (person->infection_state == 0) { // INFECTED
                            person->infection_time--;
                            if (person->infection_time <= 0) {
                                person->infection_state = 2; // IMMUNE
                                person->imunity_time = IMMUNE_DURATION;
                            }
                        }
                        else if (person->infection_state == 2) { // IMMUNE
                            person->imunity_time--;
                            if (person->imunity_time <= 0) {
                                person->infection_state = 1; // SUSCEPTIBLE
                            }
                        }

                        if (person->infection_state == 1) { // SUSCEPTIBLE
                            for (int j = 0; j < area->person_number; j++) {
                                if (i != j) {
                                    Person* other_person = &area->persons[j];

                                    if (other_person->infection_state == 0 &&
                                        other_person->x_coord == person->x_coord &&
                                        other_person->y_coord == person->y_coord) {

#pragma omp atomic
                                        person->infection_state = 0; // INFECTED
#pragma omp atomic
                                        person->infection_time = INFECTED_DURATION;
#pragma omp atomic
                                        person->infection_count++; 

                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

#pragma omp taskwait
            }
        }
    }
    double end_time = omp_get_wtime();  
    double duration = end_time - start_time;
    printf("Total time for parallel simulation V2: %.6f seconds\n", duration);
}
#endif