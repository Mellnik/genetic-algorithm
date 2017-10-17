#include <cstdint>
#include <iostream>
#include <string>
#include <ctime>
#include <memory>

using std::cout;
using std::endl;

#include "bitset.h"
#include "xoroshiro128plus.h"

#define CHROMO_LEN 256
#define GENE_LEN 4

#define RANDOM_NUM xoroshiro128p_double()
#define MUTATION_RATE 0.001
#define CROSSOVER_RATE 0.7

typedef unsigned long decodedGene;
typedef struct Chromo {
	unsigned char *genes;
} Chromo;

class Entity
{
public:
	Entity()
	{
		chromo = new Chromo;
		chromo->genes = (unsigned char *)malloc(CHROMO_LEN / CHAR_BIT);
		Bitset::resetall(chromo->genes, CHROMO_LEN);

		setRandomGenes();
	}

	Entity(Chromo *chromo)
	{
		this->chromo = chromo;
	}

	~Entity()
	{
		//free(chromo->genes); heap collisions lol
		delete chromo;
	}
	
private:
	float fitness = 0.0f;
	Chromo *chromo;
	std::string debugChromo;

public:
	void setRandomGenes()
	{
		for (int i = 0; i < CHROMO_LEN; i++) {
			if (xoroshiro128p_bool() == 1) {
				Bitset::setbit(chromo->genes, i);
			}
		}
	}

	void printChromo()
	{
		EvaluateChromo(true);
		cout << debugChromo << endl;
	}

	inline float getFitness()
	{
		return fitness;
	}

	inline Chromo *getChromo()
	{
		return chromo;
	}

	inline decodedGene decodeGene(int index) const
	{
		return Bitset::to_ulong(chromo->genes, index, 4, CHROMO_LEN);
	}

	inline bool testGeneConformity(const decodedGene &gene, bool bOperator)
	{
		return bOperator ? (gene >= 10 && gene <= 13) : (gene <= 9);
	}

	inline int getConformingGene(int index, decodedGene &gene, bool bOperator)
	{
		int skipGenes = 0;
		for (int i = index; i < CHROMO_LEN; i += GENE_LEN) {
			gene = decodeGene(i);

			if (testGeneConformity(gene, bOperator)) {
				return skipGenes * GENE_LEN;
			}
			skipGenes++;
		}
		return -1;
	}

	float EvaluateChromo(bool record)
	{
		decodedGene geneOperator = 0;
		decodedGene geneNumber = 0;
		float result = 0;
		int skipGenes = 0;

		for (int i = 0; i < CHROMO_LEN; i += GENE_LEN * 2) {
			if ((skipGenes = getConformingGene(i, geneOperator, true)) == -1) {
				break; // End of chromosome
			}
			i += skipGenes;

			if ((skipGenes = getConformingGene(i + GENE_LEN, geneNumber, false)) == -1) {
				break; // End of chromosome
			}
			i += skipGenes;

			// Avoid divide by zero
			if (geneOperator == 13 && geneNumber == 0) {
				geneOperator = 10;
			}

			switch (geneOperator) {
			case 10:
				if (record) debugChromo += " + ";
				result += geneNumber;
				break;
			case 11:
				if (record) debugChromo += " - ";
				result -= geneNumber;
				break;
			case 12:
				if (record) debugChromo += " * ";
				result *= geneNumber;
				break;
			case 13:
				if (record) debugChromo += " / ";
				result /= geneNumber;
				break;
			}
			if (record) debugChromo += std::to_string(geneNumber);
		}
		return result;
	}

	float CalculateFitness(int &target)
	{
		float result = EvaluateChromo(false);

		if (result == static_cast<float>(target)) {
			fitness = 999.9f;
		}
		else {
			fitness = 1.0f / fabs(target - result);
		}
		return fitness;
	}
};

class EcoSystem
{
public:
	EcoSystem(int PopulationCount, int Target)
	{
		m_PopulationCount = PopulationCount;
		m_Target = Target;

		Initiate();
	}

	~EcoSystem()
	{
		for (int i = 0; i < m_PopulationCount; i++) {
			delete m_Population[i];
		}
		delete[] m_Population;

		delete[] m_Descendants;
	}
private:
	void Initiate()
	{
		m_Population = new Entity*[m_PopulationCount];

		for (int i = 0; i < m_PopulationCount; i++) {
			m_Population[i] = new Entity();
		}

		m_Descendants = new Entity*[m_PopulationCount];
	}

public:
	void run()
	{
		bool bFound = false;
		while (!bFound) {
			
			if (m_EvolutionCount % 500 == 0 && m_EvolutionCount > 499) {
				cout << "EvolutionCount: " << m_EvolutionCount << " TotalFitness: " << m_TotalFitness << endl;
			}

			m_TotalFitness = 0.0f;

			for (int i = 0; i < m_PopulationCount; i++) {
				m_TotalFitness += m_Population[i]->CalculateFitness(m_Target);
				if (m_TotalFitness == 999.9f) {
					cout << "DONE! Evolution count: " << m_EvolutionCount << endl;
					m_Population[i]->printChromo();
					bFound = true;
				}
			}

			int DescendantsCount = 0;
			while (DescendantsCount < m_PopulationCount) {
				Entity *entity1 = Roulette();
				Entity *entity2 = Roulette();

				Chromo *offspring1 = new Chromo();
				offspring1->genes = (unsigned char *)malloc(CHROMO_LEN / CHAR_BIT);
				Chromo *offspring2 = new Chromo();
				offspring2->genes = (unsigned char *)malloc(CHROMO_LEN / CHAR_BIT);

				Bitset::bitcpy(offspring1->genes, entity1->getChromo()->genes, CHROMO_LEN);
				Bitset::bitcpy(offspring2->genes, entity2->getChromo()->genes, CHROMO_LEN);

				if (RANDOM_NUM < CROSSOVER_RATE) {
					int point = (int)(RANDOM_NUM * CHROMO_LEN);

					Bitset::crossover_singlepoint(offspring1->genes, offspring2->genes, point, CHROMO_LEN);
				}

				for (int i = 0; i < CHROMO_LEN; i++) {
					if (RANDOM_NUM < MUTATION_RATE) {
						Bitset::flipbit(offspring1->genes, i);
					}

					if (RANDOM_NUM < MUTATION_RATE) {
						Bitset::flipbit(offspring2->genes, i);
					}
				}

				m_Descendants[DescendantsCount++] = new Entity(offspring1);
				m_Descendants[DescendantsCount++] = new Entity(offspring2);
			}
			for (int i = 0; i < m_PopulationCount; i++) {
				delete m_Population[i];
				m_Population[i] = m_Descendants[i];
			}

			m_EvolutionCount++;
		}
	}

	Entity *Roulette()
	{
		float Slice = (float)RANDOM_NUM * m_TotalFitness;

		float FitnessSoFar = 0.0f;
		for (int i = 0; i < m_PopulationCount; i++) {
			FitnessSoFar += m_Population[i]->getFitness();

			if (FitnessSoFar >= Slice) {
				return m_Population[i];
			}
		}
		return nullptr;
	}

	int GetPopulationCount() const
	{
		return m_PopulationCount;
	}

private:
	int m_Target = 0;
	int m_PopulationCount = 0;
	Entity **m_Population = nullptr;
	Entity **m_Descendants = nullptr;
	float m_TotalFitness = 0.0f;
	int m_EvolutionCount = 0;
};

int main(int argc, char *argv[])
{
	if (argc < 3) {
		cout << "Usage: " << argv[0] << " <target> <population count>";
		return 1;
	}
	
	xoroshiro128p_seed((uint64_t)time(NULL));

	std::unique_ptr<EcoSystem> world(new EcoSystem(atoi(argv[2]), atoi(argv[1])));
	
	cout << "[world] Initial population of " << world->GetPopulationCount() << " entities generated." << endl;

	world->run();

	return 0;
}