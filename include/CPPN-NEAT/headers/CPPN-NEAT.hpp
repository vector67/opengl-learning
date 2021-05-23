#ifndef CPPN_NEAT_HPP
#define CPPN_NEAT_HPP

#include <cmath>
#include <unistd.h>

#include "fitness.hpp"
#include "genetic_encoding.hpp"
#include "user_definitions.hpp"

namespace ANN_USM
{
	class Niche
	{
		public:

			int years;
			int niche_champion_position;
			int amount_of_offspring;

			bool exist;

			double	total_fitness;
			
			vector<int> organism_position;
	};

	class Population
	{
		public:

			void init_population(char path[]);
			void save(char path[]);
			void print_niches();
			void epoch();
			void spatiation();

			int obtain_historical_node(int initial_in, int initial_out);
			int obtain_innovation(int in, int out);
			int obtain_row(int node, int node_initial_in, int node_initial_out);

			double compatibility(Genetic_Encoding orgm1, Genetic_Encoding orgm2); // Distance between two ANNs

			Genetic_Encoding mutation_node(Genetic_Encoding organism);
			Genetic_Encoding mutation_create_new_node(Genetic_Encoding organism);
			Genetic_Encoding mutation_connection(Genetic_Encoding organism);
			Genetic_Encoding mutation_change_weight(Genetic_Encoding organism);
			Genetic_Encoding put_randoms_weight(Genetic_Encoding organism);
			Genetic_Encoding crossover(Genetic_Encoding orgm1, Genetic_Encoding orgm2);

			int lenght;
			int total_niches; //number of niches
			int last_niche_id;
			int last_innovation;
			int last_node;
			int last_row;

			double fitness_champion;

			vector<Genetic_Encoding> organisms;
			vector<Genetic_Encoding> prev_organisms;

			vector <Niche> current_niches;
			vector <Niche> prev_niches;

			Genetic_Encoding * best_organism;
			Genetic_Encoding champion;

			vector< vector<int> > historical_nodes;
			vector< vector<int> > historical_innovation;

			vector<int> historical_row;
			vector<int> row_orderer_list;	
	};
}

ostream & operator<<(ostream & o, ANN_USM::Population & pop);

#endif