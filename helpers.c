#include <stdlib.h>

#include "definitions.h"
#include "helpers.h"

//Takes x,y and return the index in the 
//linealized matrix
int get_index(int x,int y,int width){
	return x+y*width;
}

//Generate random value between 0 and max_value-1
int randint(int max_value){
	return (int)((max_value)*rand_double());
}

//Generate random value between 0 and 1
double rand_double(){
	return rand()/(double)RAND_MAX;
}

//Get max people that can be evacuated from point to shelter
int get_evac(int point, int shelter,  solution sol, BEPinstance instance){
	int evac;
	
       	if(sol.people_remaining[point] > instance.bus_capacity){
		evac = instance.bus_capacity;
	}
        else{
        	evac = sol.people_remaining[point];
        }
        if(evac > sol.capacity_remaining[shelter]){
	        evac = sol.capacity_remaining[shelter];
        }
	return evac;
}


//Calculate evacuation time
int calculate_evac_time(solution sol, BEPinstance instance){
	int distances[instance.buses];
	int distance = 0;
	int people_left = 0;
	int shelter_capacity= 0;

	int max_distance = 0;
	int i,j;

	for(i=0; i < instance.buses; i++){
	//Calculating the distance to every bus
		for(j = 0; j < sol.bus_list[i].tour_length ; j++){
			if(j==0)
				distance += sol.bus_list[i].starting_tour.distance;
			else{
				distance += sol.bus_list[i].route[i-1].distance;
			}
		}
		distances[i] = distance;
		distance = 0;
	}

	//Looking for the biggest_distance
	for(i = 0; i < instance.buses; i++){
		if(distances[i] > max_distance)
			max_distance = distances[i];
	}

	//Calculating the people that still in the points
	//after the evacuation
	for(i = 0; i < instance.points; i++){
		people_left += sol.people_remaining[i];
	}
	
	//Calculating shelters with more people than its
	//capacity
	for(i = 0; i < instance.shelters;i++){
		if(sol.capacity_remaining[i] < 0){
			shelter_capacity += -1*sol.capacity_remaining[i]; 
		}
	}
	return (max_distance + PEOPLE_VIOLATION * people_left + CAPACITY_VIOLATION * shelter_capacity);

}

//Calculate the fitness value for a solution
double calculate_fitness(solution sol, BEPinstance instance){
	//Using big constants for constraint violation
	return (double) 1/calculate_evac_time(sol,instance);
}

//Create the first population where all solutions have buses with one initial tour from a
// station to a shelter. The buses are created satisfying the station bus capacity constraint.
void initialize_population(BEPinstance instance, solution** population, int pop_size){
	int station;
	int point;
	int shelter;

	int bus = 0;
	int evac;

	int distance_to_point;
	int distance_to_shelter;

	initial_tour tmp_tour;

	int i,j,k;

	double sum_fitness = 0;
	double fitness;
	
	//Iterating solutions
	for(i = 0; i < pop_size; i++){	
		(*population)[i].bus_list = (bus_tour*) malloc(sizeof(bus_tour)*instance.buses);
		(*population)[i].people_remaining = (int*) malloc(sizeof(int)*instance.points);
		(*population)[i].capacity_remaining = (int*) malloc(sizeof(int)*instance.shelters);
		(*population)[i].fitness = 0;

		for(j = 0; j < instance.points; j++){
			(*population)[i].people_remaining[j] = instance.people_per_point[j];
		}

		for(j = 0; j < instance.shelters; j++){
			(*population)[i].capacity_remaining[j] = instance.capacity_per_shelter[j];
		}

		//Iterating stations
		for(j = 0; j < instance.stations; j++){
			
			//Creating initial tour for every bus	
			for(k = 0; k < instance.bus_per_station[j]; k++){ 
				station = j;
				point = randint(instance.points);
				shelter = randint(instance.shelters);
				
				tmp_tour.station = station;
				tmp_tour.point = point;
				tmp_tour.shelter = shelter;
				
				distance_to_point = instance.distance_station_matrix[get_index(point,station,instance.points)];
				distance_to_shelter = instance.distance_shelter_matrix[get_index(shelter,point,instance.shelters)];

				tmp_tour.distance = distance_to_point + distance_to_shelter;

				(*population)[i].bus_list[bus].starting_tour = tmp_tour;
				(*population)[i].bus_list[bus].tour_length = 1;

				bus += 1;

				evac = get_evac(point,shelter, (*population)[i], instance);
				(*population)[i].bus_list[bus].starting_tour.evac = evac;				

				(*population)[i].people_remaining[point] -= evac;
				(*population)[i].capacity_remaining[shelter] -= evac;
			}
	
		}
		bus = 0;
		//Calculating solution initial fitness
		fitness = calculate_fitness((*population)[i],instance);
		sum_fitness += fitness;
		(*population)[i].fitness = fitness;	
	}

	for(i = 0; i< pop_size;i++){
		(*population)[i].relative_fitness = (double) (*population)[i].fitness / (double) sum_fitness;
	}
		

}

//Update relative fitness
void update_relative_fitness(solution** population, int pop_size){
	int i;
	double sum_fitness = 0;
	for(i = 0; i < pop_size; i++){
		sum_fitness += (*population)[i].fitness;
	}

	for(i = 0;i < pop_size; i++){
		(*population)[i].relative_fitness = (double) (*population)[i].fitness / (double) sum_fitness;
	}
}

//Update evac times
void update_evac_time(solution** population, BEPinstance instance, int pop_size){
	int i;
	int evac_time;
	for(i=0;i<pop_size;i++){
		evac_time = calculate_evac_time((*population)[i], instance);
		(*population)[i].evac_time = evac_time;
	}
}

void deep_copy_solution(solution* original, solution* copy, BEPinstance instance){
	int bus;
	int i;

	(*copy).fitness = (*original).fitness;
	(*copy).relative_fitness = (*original).relative_fitness;
	(*copy).evac_time = (*original).evac_time;

	(*copy).bus_list = (bus_tour*) malloc(sizeof(bus_tour)*instance.buses);
	(*copy).people_remaining = (int*) malloc(sizeof(int)*instance.points);
	(*copy).capacity_remaining = (int*) malloc(sizeof(int)*instance.shelters);

	for(i = 0; i < instance.points; i++){
		(*copy).people_remaining[i] = (*original).people_remaining[i];
	}
	for(i = 0; i < instance.shelters; i++){
		(*copy).capacity_remaining[i] = (*original).capacity_remaining[i];
	}
	for(bus = 0; bus < instance.buses; bus++){
		(*copy).bus_list[bus].starting_tour = (*original).bus_list[bus].starting_tour;
		(*copy).bus_list[bus].tour_length = (*original).bus_list[bus].tour_length;
		for(i = 0; i < MAX_TOURS; i++){
			(*copy).bus_list[bus].route[i] = (*original).bus_list[bus].route[i];

		}
	}


}