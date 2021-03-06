#ifndef DEFINITIONS_HEADER
#define DEFINITIONS_HEADER

//Seeds for rand() function
#include "seeds.h"


//Constants used for fitness function
#define PEOPLE_VIOLATION 1000
#define CAPACITY_VIOLATION 500

//Max tours for a bus
#define MAX_TOURS 25

//Bus Evacuation Problem instance structure, with all the data parsed from
//instance file.
typedef struct{
    int buses; 
    int stations; 
    int points; 
    int shelters; 
    int max_people;  
    int bus_capacity;
    int max_capacity;

    int* bus_per_station; 
    int* people_per_point; 
    int* capacity_per_shelter;

    int* distance_station_matrix;
    int* distance_shelter_matrix;

} BEPinstance;


//Initial tour taken by a bus, begins in a station
// and finish in a shelter.
typedef struct{
	int station;
	int point;
	int shelter;
	int evac; //people evacuated in this tour.
} initial_tour;


//Generic tour after the initial one
//begins in one shelter and ends in the same or other.
typedef struct{
	int point;
	int shelter;
	int evac;
} tour;

//Tours taken by a bus
//has one initial tour, and a list of generic tours.
typedef struct{
	initial_tour starting_tour;
	tour route[MAX_TOURS];
	int route_length;
} bus_tour;


//A solution has a list of buses
//and has a fitness value
typedef struct{
	bus_tour *bus_list;

	int* people_remaining;  
    int* capacity_remaining; 

	double fitness;
	double relative_fitness;

	int evac_time;
} solution;


#endif
