cmake_minimum_required(VERSION 3.7...3.20)

add_library( neat SHARED experiments.cpp experiments.h gene.cpp gene.h genome.cpp genome.h innovation.cpp innovation.h link.cpp link.h neat.cpp neat.h network.cpp network.h nnode.cpp nnode.h organism.cpp organism.h population.cpp population.h species.cpp species.h trait.cpp trait.h)

