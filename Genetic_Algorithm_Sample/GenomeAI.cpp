#include "GenomeAI.h"

//####################
// Private Functions #
//####################

/**
 * Takes a list of bits that are either 0 or 1 and loops through
 * each one, randomly deciding whether to flip the bit. If this 
 * happens, then it is considered a mutation.
 * @param bits A reference to a list of bits (integer of 0 or 1).
 */
void GenomeAI::Mutate(std::vector<int> &bits)
{
	for (int currentBit = 0; currentBit < bits.size(); ++currentBit)
		if (RandomFloat(0, 1) < mutationRate)
			bits[currentBit] = !bits[currentBit];
}

/**
 * Takes the bits from a mum and dad and combines them at a random point
 * along the list. This creates two new sets of bits for two babies.
 * If the crossover rate is exeeded or the mum and dad are the same then
 * the mum and dad bits are copied directly to the babies and a crossover
 * doesn't happen
 * @param mum Reference to a list of bits that are refered to as the mother.
 * @param dad Reference to a list of bits that are refered to as the father.
 * @param baby1 Reference to an empty list of bits that will become the first new baby.
 * @param baby2 Reference to an empty list of bits that will become the second new baby.
 */
void GenomeAI::Crossover(const std::vector<int> &mum, const std::vector<int> &dad,
	std::vector<int> &baby1, std::vector<int> &baby2)
{
	if (RandomFloat(0, 1) > crossoverRate || (mum == dad))
	{
		baby1 = mum;
		baby2 = dad;
		return;
	}

	int crossoverPoint = rand() % (chromoLength - 1);

	for (int i = 0; i < crossoverPoint; ++i)
	{
		baby1.push_back(mum[i]);
		baby2.push_back(dad[i]);
	}

	for (int i = crossoverPoint; i < mum.size(); ++i)
	{
		baby1.push_back(dad[i]);
		baby2.push_back(mum[i]);
	}
}

/**
 * Used to select a random genome from the pool.
 * Genomes with a higher fitness score will be more likely to be picked,
 * due to their "slice" being a larger part of the whole.
 * @return Reference to a random genome.
 */
Genome& GenomeAI::RouletteWheelSelection()
{
	double slice = RandomFloat(0, totalFitnessScore);
	double total = 0;
	int selectedGenome = 0;

	for (int i = 0; i < populationSize; ++i)
	{
		total += genomes[i].fitness;

		if (total > slice)
		{
			selectedGenome = i;
			break;
		}
	}
	return genomes[selectedGenome];
}

/**
 * Finds the fittest genome in the current pool, while also
 * updating the total fitness overall.
 */
void GenomeAI::UpdateFitnessScores()
{
	fittestGenome = 0;
	bestFitnessScore = 0.0;
	totalFitnessScore = 0.0;

	for (int i = 0; i < genomes.size(); ++i)
	{
		genomes[i].fitness = map.TestRoute(Decode(genomes[i].bits), map);
		totalFitnessScore += genomes[i].fitness;

		if (genomes[i].fitness > bestFitnessScore)
		{
			bestFitnessScore = genomes[i].fitness;
			fittestGenome = i;

			brain.ResetMemory();

			for (int x = 0; x < MAP_WIDTH; ++x)
				for (int y = 0; y < MAP_HEIGHT; ++y)
					brain.memory[y][x] = map.memory[y][x];
		}

		map.ResetMemory();
	}
}

/** 
 * Decodes a list of bits into a list of directions. Two bits
 * in the list define a "gene" which is converted from a binary
 * number to a decimal number. The decimal number from 0 - 3
 * represents a cardinal direction.
 * @param bits List of bits to be converted to direction.
 * @return List of directions created from the genomes bits.
 */
std::vector<int> GenomeAI::Decode(const std::vector<int> &bits)
{
	std::vector<int> directions;

	for (int i = 0; i < geneLength; ++i)
	{
		std::vector<int> genePair;
		genePair.push_back(bits[i * 2]);
		genePair.push_back(bits[(i * 2) + 1]);

		directions.push_back(BinToInt(genePair));
	}

	return directions;
}

/**
 * Converts a two bit binary number into a direction, which
 * is also defined by a number from 0 - 3.
 * @return An integer representing a cardinal direction.
 */
int GenomeAI::BinToInt(const std::vector<int> &v)
{
	switch (v[0])
	{
	case 0:
		switch (v[1])
		{
		case 0:
			return 0;
		case 1:
			return 1;
		default:
			return 0;
		}
	case 1:
		switch (v[1])
		{
		case 0:
			return 2;
		case 1:
			return 3;
		default:
			return 2;
		}
	default:
		return 0;
	}
}

/**
 * Initilizes the starting genomes.
 */
void GenomeAI::CreateStartPopulation()
{
	map = Map();
	brain = Map();

	for (int i = 0; i < populationSize; ++i)
		genomes.push_back(Genome(chromoLength));
}

/**
 * Gets a random float between the min and max values given.
 * @param min Minimum value [inclusive].
 * @param max Maximum value [inclusive].
 * @return Random float between min and max.
 */
float GenomeAI::RandomFloat(float min, float max)
{
	assert(max > min);
	float random = ((float)rand()) / (float)RAND_MAX;
	float range = max - min;
	return min + (random * range);
}

//###################
// Public Functions #
//###################

/**
 * Main function the run the logic of the genetic algorithm.
 * @param hWnd Handle to the window
 */
void GenomeAI::Run()
{
	if (bestFitnessScore == 1.0)
		return;

	UpdateFitnessScores();

	int newBabies = 0;
	std::vector<Genome> babyGenomes;

	while (newBabies < populationSize)
	{
		Genome mum = RouletteWheelSelection();
		Genome dad = RouletteWheelSelection();

		Genome baby1, baby2;
		Crossover(mum.bits, dad.bits, baby1.bits, baby2.bits);

		Mutate(baby1.bits);
		Mutate(baby2.bits);

		babyGenomes.push_back(baby1);
		babyGenomes.push_back(baby2);

		newBabies += 2;
	}

	genomes = babyGenomes;
	++generation;
}

/**
 * Calls render on the map to draw the map in the window.
 * Then draws the path of the best genome of this generation.
 * @param xClient Window width.
 * @param yClient Window height.
 * @param surface Handle to the backbuffer.
 */
void GenomeAI::Render(int xClient, int yClient, HDC surface)
{
	map.Render(xClient, yClient, surface);

	HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));

	SelectObject(surface, blueBrush);
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			switch (brain.memory[y][x])
			{
			case 1:
				Rectangle(surface, x * (xClient / MAP_WIDTH), y * (yClient / MAP_HEIGHT),
					x * (xClient / MAP_WIDTH) + (xClient / MAP_WIDTH), y * (yClient / MAP_HEIGHT) + (yClient / MAP_HEIGHT));
				break;
			}
		}
	}
	SelectObject(surface, whiteBrush);

	SetTextColor(surface, RGB(255, 255, 255));
	SetBkMode(surface, TRANSPARENT);

	TCHAR buffer[100];
	_stprintf_s(buffer, _T("%d"), generation);

	TextOut(surface, 3, 10, buffer, strlen(buffer));

	DeleteObject(whiteBrush);
	DeleteObject(blueBrush);
}