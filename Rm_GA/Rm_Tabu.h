#pragma once
#include "stdafx.h"
#include <algorithm>
#include <windows.h>
#include "Rm_job.h"
#include "Rm_solution.h"
#include "Rm_ListSchedule.h"
using namespace std;

//Parameter for tabu search
//const int TABU_LIST_LENGTH = 100;
const int TABU_ITERATION_MAXN = 80;
const int TABU_UNCHANGED_ITERATION_MAXN = 50;
const bool TABU_UNCHANGED_ITERATION_MODE = true;
int TABU_RANDOM_SELECTION_ITERATION = -1;
int TABU_SHORTLIST = -1;
int TABU_LONGLIST = -1;
const double TABU_ACCEPT_PROBABILITY = 0.02;

//const int MINC_RULE = 0, MINP_RULE = 1, MINY_RULE = 2, SPT_EDD_RULE = 3, LPT_EDD_RULE = 4, RANDOM_RULE = 5;
const int SPT_MINP_RULE = 0;

class TabuOptimizer
{
	int recur;
	int tabu_pointer_1, tabu_pointer_2;

	bool random_accept;

	// 2017-5-3
	// 增加统计算法时间
	double tick_count;
	double tabu_rspt_minp_time = -1;
	bool terminated;
	// 2017-5-1
	// 添加短期记忆、中期记忆表
	Solution* tabu_list_1;
	Solution* tabu_list_2;

	int init_cost = -1;

	bool solution_exists(const Solution& solution)
	{
		for (int i = 0; i < TABU_SHORTLIST; i++)
		{
			if (tabu_list_1[i].equals(solution))
				return true;
		}
		for (int i = 0; i < TABU_LONGLIST; i++)
		{
			if (tabu_list_2[i].equals(solution))
				return true;
		}

		return false;
	}

	void record_short_term_memory(const Solution& solution)
	{
		tabu_list_1[tabu_pointer_1++] = solution;
		if (tabu_pointer_1 >= TABU_SHORTLIST)
			tabu_pointer_1 = 0;
	}

	void record_medium_term_memory(const Solution& solution)
	{
		tabu_list_2[tabu_pointer_2++] = solution;
		if (tabu_pointer_2 >= TABU_LONGLIST)
			tabu_pointer_2 = 0;
	}
public:

	TabuOptimizer(bool random_accept, int neighbor, int shortL, int longL) : random_accept(random_accept)
	{
		//短期记忆、中期记忆表
		TABU_RANDOM_SELECTION_ITERATION = neighbor;
		TABU_SHORTLIST = shortL;
		TABU_LONGLIST = longL;
		tabu_list_1 = new Solution[TABU_SHORTLIST];
		tabu_list_2 = new Solution[TABU_LONGLIST];
	}

	~TabuOptimizer()
	{
		delete[] tabu_list_1;
		delete[] tabu_list_2;
	}

	void initialize()
	{
		tabu_pointer_1 = tabu_pointer_2 = 0;
		terminated = false;
		//tick_count = 0;
	}
	Solution find_neighbor_solution(const Solution& source)
	{
		Solution current = source;
		int swap_a = randint(0, current_config.n + current_config.m - 1);
		int swap_b = randint(0, current_config.n + current_config.m - 1);
		swap(current.schedule[swap_a], current.schedule[swap_b]);
		return current;
	}
	Solution find_next_solution_random_method(const Solution& source)//策略1:全排列搜索策略
	{
		Solution current_solution = source;
		int current_solution_value = evalcost(current_solution), temp_value;
		bool updated = false;

		for (int i = 0; i < TABU_RANDOM_SELECTION_ITERATION; i++)//TABU_RANDOM_SELECTION_ITERATION = 20
		{
			Solution next_solution = find_neighbor_solution(source);
			temp_value = evalcost(next_solution);

			// change search method
			//ww：const double TABU_ACCEPT_PROBABILITY = 0.02
			//两个禁忌表中都没有，如果有的话如果random_accept=true有还是有2%的可能  && 领邻域的目标值要小于当前的调度  || !update
			if (!solution_exists(next_solution) && ((random_accept && u(1, 100) < 100 * TABU_ACCEPT_PROBABILITY) || temp_value < current_solution_value) || !updated)
			{
				record_short_term_memory(next_solution);
				current_solution_value = temp_value;
				current_solution = next_solution;
				updated = true;
			}
		}

		return current_solution;
	}

	Solution getInitValue(int init_rule)
	{
		if (init_rule == SPT_MINP_RULE)
			return get_spt_minp_solution();

	}
	Solution minimize(int init_rule)
	{
		LARGE_INTEGER cpu_freq, t1, t2;
		QueryPerformanceFrequency(&cpu_freq);
		QueryPerformanceCounter(&t1);
		initialize();

		Solution current_solution = getInitValue(init_rule);

		Solution best_solution = current_solution;

		init_cost = evalcost(best_solution);
		int unchanged_iteration = 0, iteration = 0;
		int last_solution_value = 0, current_value = evalcost(current_solution), best_value = evalcost(best_solution);

		while (iteration < TABU_ITERATION_MAXN)
		{
			// change strategy of tabu search
			// record_solution(current_solution);
			current_solution = find_next_solution_random_method(current_solution);//策略1:全排列搜索策略
			current_value = evalcost(current_solution);

			if (best_value > current_value)
			{
				record_medium_term_memory(current_solution);
				best_value = current_value;
				best_solution = current_solution;
			}

			if (TABU_UNCHANGED_ITERATION_MODE)
			{
				if (current_value != last_solution_value)
				{
					last_solution_value = current_value;
					unchanged_iteration = 0;
				}
				else
				{
					unchanged_iteration++;
					if (unchanged_iteration > TABU_UNCHANGED_ITERATION_MAXN)
						break;
				}
			}
			iteration++;
			if (terminated) break;
		}
		QueryPerformanceCounter(&t2);
		tick_count = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;
		return best_solution;
	}
// 增加统计算法时间
	double get_ts_running_time()
	{
		return tick_count;
	}

	int get_inital_cost()
	{
		return init_cost;
	}
};

//int tabu_solve(bool is_random_accept)
//{
//	TabuOptimizer tabu(is_random_accept);
//	Solution s = tabu.minimize();
//	int result = evalcost(s);
//	return result;
//}