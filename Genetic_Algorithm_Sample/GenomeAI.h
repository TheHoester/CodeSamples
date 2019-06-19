#pragma once

#include <assert.h>
#include <random>
#include <vector>
#include <tchar.h>

#include "Map.h"
#include "Defines.h"

struct Genome
{
	std::vector<int> bits;
	double fitness;

	Genome() : fitness(0) {}
	Genome(const int numBits) : fitness(0)
	{
		for (int i = 0; i < numBits; i++)
			bits.push_back(rand() % 2);
	}
};

class GenomeAI
{
private:
	std::vector<Genome> genomes;
	int populationSize;
	double crossoverRate;
	double mutationRate;
	int chromoLength;
	int geneLength;

	int fittestGenome;
	double bestFitnessScore;
	double totalFitnessScore;
	int generation;

	Map map;
	Map brain;

	bool busy;

	void Mutate(std::vector<int> &bits);
	void Crossover(const std::vector<int> &mum, const std::vector<int> &dad,
		std::vector<int> &baby1, std::vector<int> &baby2);

	Genome& RouletteWheelSelection(void);
	void UpdateFitnessScores(void);
	std::vector<int> Decode(const std::vector<int> &bits);
	int BinToInt(const std::vector<int> &v);
	void CreateStartPopulation(void);

	float RandomFloat(float a, float b);

public:
	GenomeAI() : crossoverRate(CROSSOVER_RATE), mutationRate(MUTATION_RATE), populationSize(POP_SIZE),
		chromoLength(CHROMO_LENGTH), geneLength(CHROMO_LENGTH / 2), bestFitnessScore(0.0),
		totalFitnessScore(0.0), fittestGenome(0), generation(0), busy(false)
	{
		CreateStartPopulation();
	}

	GenomeAI(double crossRat, double mutRat, int popSize, int numBits, int geneLen) :
		crossoverRate(crossRat), mutationRate(mutRat), populationSize(popSize),
		chromoLength(numBits), bestFitnessScore(0.0), totalFitnessScore(0.0),
		fittestGenome(0), generation(0), geneLength(geneLen), busy(false)
	{
		CreateStartPopulation();
	}

	void Run(void);
	void Render(int cxClient, int cyClient, HDC surface);

	int Generation(void) { return generation; }
	int GetFittest(void) { return fittestGenome; }
	bool Started(void) { return busy; }
	void Start(void) { busy = true; }
	void Stop(void) { busy = false; }
};
