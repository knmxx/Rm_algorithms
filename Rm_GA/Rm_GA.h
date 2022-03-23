#pragma once
#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>

#include "Rm_job.h"
#include "Rm_DataReader.h"
#include "Rm_ListAlgorithm.h"
#include "Rm_ListSchedule.h"

double ga_running_time = -1;

using namespace std;
const int ITERATION_MAXN = 300;
const int GA_UNCHANGED_ITERATION_MAXN = 50;
//const double LEARNINGRATE = .02, CROSSOVERRATE = .5;
const double LEARNINGRATE = .7; 
const int M = 9999999;
int POPULATION_MAXN = 50;
double CROSSOVERRATE = 0.5, MUTATIONRATE = 0.5;
int current_population_num;
int ga_inital_cost = -1;
int bestvalue = -1;
/*

	Basic data-structure for genetic algorithm

		sequence: data of problem
		cost: cached fittness value, cost = -1: needs to be updated. Needs to set to -1 if sequence is modified.

*/

struct Chromosome
{
	int sequence[MAX_M + MAX_N];
	int cost;
	double fitness;
	double rate;
	double sumFitnessrate;
	Chromosome()
	{
		cost = -1;
		double fitness = 0.0;
		double rate = 0.0;
	}
	void initialize()
	{
		for (int i = 0; i < current_config.n + current_config.m - 1; i++)
		{
			sequence[i] = i;
		}
		random_shuffle(sequence, sequence + current_config.n + current_config.m - 1);
		cost = evaluate();
		fitness = M - cost;
		rate = 0.0;
		sumFitnessrate = 0.0;

	}
	void initialize(const int* series, int result)
	{
		for (int i = 0; i < current_config.n + current_config.m - 1; i++)
		{
			sequence[i] = series[i];
		}
		cost = evaluate();
		fitness = M - cost;
		rate = 0.0;
		sumFitnessrate = 0.0;
	}
	int evaluate()
	{
		cost = 0;
		int machineindex = 0;
		int completion = 0;
		for (int i = 0; i < current_config.n + current_config.m - 1; i++)
		{
			if (sequence[i] >= current_config.n) 
			{
				machineindex++;
				completion = 0;
			}

			else
			{
				Job& job = job_set[sequence[i]];
				completion += job.proc_time[machineindex];
				cost += min(job.proc_time[machineindex], max(0, completion - job.due_date));
			}
		}
		fitness = M - cost;
		return cost;
	}
	bool operator<(Chromosome& r) { return evaluate() < r.evaluate(); }
} *population;


/*
	initialization of population:

		randomly generate solutions using random_shuffle
		if list_initialize is true, some schedule result of list algorithm will be added into the population


*/

void ga_initialize_population(bool list_initialize)
{
	population = new Chromosome[POPULATION_MAXN];
	int add_count = 0;
	current_population_num = POPULATION_MAXN;
	if (list_initialize)
	{
		//ListOptimizer listop;
		Solution(*cmp_functions[10])(void) = {
		get_edd_minc_solution ,
		get_spt_minc_solution ,
		get_lpt_minc_solution ,
		get_edd_miny_solution ,
		get_spt_miny_solution ,
		get_lpt_miny_solution ,
		get_edd_minp_solution ,
		get_spt_minp_solution ,
		get_lpt_minp_solution ,
		get_random_solution };
		int late_work = -1;
		for (int i = 0; i < 10; i++)
		{
			Solution s = cmp_functions[i]();
			population[add_count++].initialize(s.schedule, late_work);
			if (i == 7)
			{
				ga_inital_cost = evalcost(s);
			}
		}
	}
	for (int i = add_count; i < POPULATION_MAXN; i++)
		population[i].initialize();

	bestvalue = population[0].cost;
}

/*

	Crossover operator:

		1. select two parents from last population
		2. let result = parent A
		3. partial copy (not directly set current position to the value of B's, find this value in A, then swap two positions) from parent B.

	Factors: LEARNINGRATE

*/

void evalrate()
{
	long int  sum = 0;
	for (int i = 0; i < current_population_num; i++)
	{
		sum += population[i].fitness;
	}
	for (int i = 0; i < current_population_num; i++)
	{
		population[i].rate = population[i].fitness / sum;
	}
	for (int i = 0; i < current_population_num; i++)
	{
		population[i].rate = (double)population[0].fitness / sum;
		if (i == 0)
		{
			population[0].sumFitnessrate = population[0].rate;
		}
		else
		{
			population[i].sumFitnessrate = population[i - 1].sumFitnessrate + population[i].sumFitnessrate;
		}

	}
}

void select()
{
	int count = 0;
	Chromosome* newPopulation = new Chromosome[POPULATION_MAXN];
	evalrate();
	for (int i = 0; i < POPULATION_MAXN; i++)
	{
		if (i == 0)
		{
			newPopulation[count].cost = population[i].cost;
			newPopulation[count].fitness = population[i].fitness;
			newPopulation[count].rate = population[i].rate;
			newPopulation[count].sumFitnessrate = population[i].sumFitnessrate;
			memcpy(newPopulation[count].sequence, population[i].sequence, sizeof(population[i].sequence));
			count++;
		}

		else
		{
			int rate_rand = randint(0, 1000);

			for (int j = 1; j < POPULATION_MAXN; j++)
			{
				if (population[j - 1].sumFitnessrate * 1000 < rate_rand  && rate_rand <= population[j].sumFitnessrate * 1000)
				{
					newPopulation[count].cost = population[j].cost;
					newPopulation[count].fitness = population[j].fitness;
					newPopulation[count].rate = population[j].rate;
					newPopulation[count].sumFitnessrate = population[j].sumFitnessrate;
					memcpy(newPopulation[count].sequence, population[j].sequence, sizeof(population[j].sequence));
					count++;
					break;
				}

			}
		}


	}
	current_population_num = count;
	int mark = current_population_num * 4 / 5;
	for (int i = 0; i < mark; i++)
	{
		population[i].cost = newPopulation[i].cost;
		population[i].fitness = newPopulation[i].fitness;
		population[i].rate = newPopulation[i].rate;
		population[i].sumFitnessrate = newPopulation[i].sumFitnessrate;
		memcpy(population[i].sequence, newPopulation[i].sequence, sizeof(newPopulation[i].sequence));
	}

	for (int i = mark; i < POPULATION_MAXN; i++)
	{
		population[i].cost = newPopulation[0].cost;
		population[i].fitness = newPopulation[0].fitness;
		population[i].rate = newPopulation[0].rate;
		population[i].sumFitnessrate = newPopulation[i].sumFitnessrate;
		memcpy(population[i].sequence, newPopulation[0].sequence, sizeof(newPopulation[0].sequence));
	}
	if (bestvalue > population[0].cost)
		bestvalue = population[0].cost;
	delete newPopulation;
	newPopulation = NULL;

}
/*单点交叉*/
void cross(Chromosome &c1, Chromosome &c2, int index)
{
	vector<int>tep_order1(c1.sequence, c1.sequence + current_config.n + current_config.m - 1);
	vector<int>tep_order2(c2.sequence, c2.sequence + current_config.n + current_config.m - 1);
	int work1[MAX_M + MAX_N];
	int work2[MAX_M + MAX_N];
	memset(work1, -1, sizeof(work1));
	memset(work2, -1, sizeof(work2));
	for (int i = 0; i <= index; i++)
	{
		work1[c1.sequence[i]] = 1;
		work2[c2.sequence[i]] = 1;
	}
	int index1 = index + 1;
	int index2 = index + 1;
	for (int i = 0; i < current_config.n + current_config.m - 1; i++)  
	{
		if (work1[tep_order2[i]] == -1 && index1 < current_config.n + current_config.m - 1)
		{
			c1.sequence[index1] = tep_order2[i];
			work1[tep_order2[i]] = true;
			index1++;
		}
		if (work2[tep_order1[i]] == -1 && index2 < current_config.n + current_config.m - 1)
		{
			c2.sequence[index2] = tep_order1[i];
			work2[tep_order1[i]] = true;
			index2++;
		}
	}
}
void ga_cross_over()//交叉
{
	double rate_rand(0.0);
	int num1_rand(0), num2_rand(0), position_rand(0);
	for (int i = 0; i < current_population_num; i++)
	{

		if (event_happen(CROSSOVERRATE))
		{
			num1_rand = rand() % current_population_num;
			num2_rand = rand() % current_population_num;

			position_rand = randint(0, current_config.n + current_config.m - 1);
			if (num1_rand != num2_rand)
			{
				cross(population[num1_rand], population[num2_rand], position_rand);
				population[num1_rand].cost = -1;
				population[num2_rand].cost = -1;
			}
		}

	}
}

/*

	Mutation operator:

		randomly swap two positions of one parent in last population

*/

void ga_mutation()
{
	for (int i = 0; i < current_population_num; i++)
	{
		if (event_happen(MUTATIONRATE))
		{
			int swap_a = randint(0, current_config.n + current_config.m - 1);
			int swap_b = randint(0, current_config.n + current_config.m - 1);
			swap(population[i].sequence[swap_a], population[i].sequence[swap_b]);
			population[i].cost = -1;
		}

	}

}

void computePrority()
{
	for (int i = 0; i < current_population_num; i++)
	{
		if (population[i].cost = -1)
		{
			population[i].evaluate();
		}
	}
}
/*最佳个体保存策略：用上一代最优个体替换当代群体中的最差个体，起优化作用*/
//void Elitist()
//{
//	int best_individual(0), worst_individual(0);
//	int best_fitness = PopulationList[0].Fitness, worst_fitness = PopulationList[0].Fitness;
//
//	for (int i = 1; i < GROUP_SCALE; i++)
//	{
//		if (PopulationList[i].Fitness > best_fitness)
//		{
//			best_fitness = PopulationList[i].Fitness;
//			best_individual = i;
//		}
//		if (PopulationList[i].Fitness < worst_fitness)
//		{
//			worst_fitness = PopulationList[i].Fitness;
//			worst_individual = i;
//		}
//	}
//
//	if (PopulationList[GROUP_SCALE].Fitness < best_fitness)
//	{
//		PopulationList[GROUP_SCALE] = PopulationList[best_individual];
//	}
//	else
//	{
//		PopulationList[worst_individual] = PopulationList[GROUP_SCALE];
//	}
//
//
//}
/*

	GA solver, entry function

	the cpu running time will be recorded in global varable: ga_running_time (in seconds)

*/

int ga_solve(bool list_initialize, int population_size, double pc, double pm)
{
	POPULATION_MAXN = population_size;
	CROSSOVERRATE = pc;
	MUTATIONRATE = pm;
	int last_solution_value = 0;
	int unchanged_iteration = 0;
	LARGE_INTEGER cpu_freq, t1, t2;

	QueryPerformanceFrequency(&cpu_freq);
	QueryPerformanceCounter(&t1);
	ga_initialize_population(list_initialize);
	evalrate();
	sort(population, population + POPULATION_MAXN);
	for (int iteration = 0; iteration < ITERATION_MAXN; iteration++)
	{
		select();//轮盘赌选择
		ga_cross_over();//交叉
		ga_mutation();//变异
		computePrority();//计算交叉和变异后的适应度值
		sort(population, population + current_population_num);

		if (population[0].cost != last_solution_value)
		{
			last_solution_value = population[0].cost;
			unchanged_iteration = 0;
		}
		else
		{
			unchanged_iteration++;
			if (unchanged_iteration > GA_UNCHANGED_ITERATION_MAXN)
				break;
		}
	}
	QueryPerformanceCounter(&t2);
	ga_running_time = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;//ms
	int finalresult = population[0].evaluate();
	delete population;
	population = NULL;
	return finalresult;
}

int get_ga_inital_cost()
{
	return ga_inital_cost;
}