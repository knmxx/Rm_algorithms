#pragma once
#include <iostream>
#include <algorithm>
#include <string.h>
#include <queue>
#include <time.h>
#include "stdafx.h"
#include "Rm_job.h"
#include "Rm_listAlgorithm.h"
#include "Rm_single_dp.h"
#include "Rm_DPm.h"
#include "Rm_ListSchedule.h"
#include "Rm_GA.h"
using namespace std;

const int BBLB_DP_RULE = 0;

long long bblb_nodes_count = 0;
Job uncertain_jobs[MAX_N];
double bb_running_time;
size_t biggest_queue_length = 0;

char bb_current_algo_name[256];
const int INFMAX = 999999;
Job bb_jobs[MAX_N];
int used_machines_num;
boolean bb_timeout = false;
//int best_value = INFMAX;
//char best_schedule[MAX_M + MAX_N];
//char current_schedule[MAX_M + MAX_N];
//int* bb_indices;
/*

	After executing any algorithms related to BB, bblb_nodes_count/bb_running_time/bb_current_algo_name
	will be updated.

*/

/*

	Partial solution for Branch & Bound

		- current_point: the length of assigned job list
		- cached_cost: the late work of partial solution
		- completion_time: the completion time of current schedule machine
		- add_confirm_id: add a new job id to the partial solution (means a new job assigned to the tail of current machine)
		- cut_by_neighbor: return true if it can be pruned by dominance rule
		- visited_id: return true if given job id is already assigned in this partial solution
		- get_upper_bound:

			if there is only one machine needs to be assigned, finish it use single DP immediately
			not - the upperbound will be equal to the minimum value of all list schedule result

		- get_lower_bound:

			if there is only one machine needs to be assigned, finish it use single DP immediately
			if not:
				get lowerbound according to the type of rule: BBLB_DP_RULE/BBLB_LPT_RULE/BBLB_ONLINE_RULE

*/

class BBNode
{
	int current_point, cached_cost, completion_time, current_machine_id;
	//	- current_point: the length of assigned job list
	//	- cached_cost : the late work of partial solution
	//	- completion_time : the completion time of current schedule machine
	
	char* confirmed_data;
	char* visited_node;

	bool exist_neighbor()
	{
		if (current_point < 2) return false;
		int last_id = confirmed_data[current_point - 1];
		int last_id_2 = confirmed_data[current_point - 2];
		return last_id < current_config.n&& last_id_2 < current_config.n; // true if last two ids are jobs
	}

	void set_completed()
	{
		current_point = current_config.n + current_config.m - 1;
	}

public:
	BBNode()
	{
		visited_node = new char[current_config.m + current_config.n];
		confirmed_data = new char[current_config.m + current_config.n];
		memset(visited_node, 0, current_config.m + current_config.n);
		memset(confirmed_data, 0, current_config.m + current_config.n);
		current_point = cached_cost = completion_time = current_machine_id = 0;
	}
	BBNode(const BBNode& prototype)
	{
		current_point = prototype.current_point;
		cached_cost = prototype.cached_cost;
		completion_time = prototype.completion_time;
		current_machine_id = prototype.current_machine_id;

		visited_node = new char[current_config.m + current_config.n];
		confirmed_data = new char[current_config.m + current_config.n];
		memcpy(visited_node, prototype.visited_node, sizeof(char) * (current_config.m + current_config.n - 1));
		memcpy(confirmed_data, prototype.confirmed_data, sizeof(char) * (current_config.m + current_config.n - 1));
	}
	~BBNode() {
		delete[] visited_node;
		delete[] confirmed_data;
	}


	int eval()
	{
		return cached_cost;
	}
	//ww
	int level()
	{
		return current_point;
	}

	bool is_complete() { return current_point == current_config.n + current_config.m - 1; }
	void add_confirm_id(int job_id)
	{
		confirmed_data[current_point++] = job_id;
		visited_node[job_id] = 1;
		if (job_id >= current_config.n)   // new data is a machine splitter
		{
			completion_time = 0;
			current_machine_id++;
		}

		else// new data is a job
		{
			completion_time += bb_jobs[job_id].proc_time[current_machine_id];
			cached_cost += min(bb_jobs[job_id].proc_time[current_machine_id], max(0, completion_time - bb_jobs[job_id].due_date));
		}
	
	}
	bool cut_by_neighbor()
	{
		if (!exist_neighbor() || is_complete())
			return false;

		/*
		CURRENT SOLUTION, LAST TWO JOBS IS AB
		CALCULATE IF LAST TWO JOBS IS BA, COMPARE TWO LATE WORK
		*/

		Job& job_a = bb_jobs[confirmed_data[current_point - 2]];
		Job& job_b = bb_jobs[confirmed_data[current_point - 1]];
		int cutted_completion_time = completion_time - job_a.proc_time[current_machine_id] - job_b.proc_time[current_machine_id];
		int cost_ab = 0, cost_ba = 0;
		cost_ab = job_late_work(job_a, current_machine_id, cutted_completion_time + job_a.proc_time[current_machine_id]) + job_late_work(job_b, current_machine_id, completion_time);
		cost_ba = job_late_work(job_b, current_machine_id, cutted_completion_time + job_b.proc_time[current_machine_id]) + job_late_work(job_a, current_machine_id, completion_time);
		return cost_ba < cost_ab || (cost_ba == cost_ab && confirmed_data[current_point - 2] < confirmed_data[current_point - 1]);
	}
	bool visited_id(int node)
	{
		return visited_node[node] != 0;
	}
	int get_upper_bound()
	{
		// NEED DUE DATE ORDER
		// EDD-MIN-COMLETIONTIME
		if (is_complete())
			return eval();
		int used_machines = 0;
		int uncertain_jobs_count = 0;
		for (int i = 0; i < current_config.m + current_config.n - 1; i++)
		{
			if (!visited_node[i])
			{
				if (i >= current_config.n)
					used_machines++;
				else
					uncertain_jobs[uncertain_jobs_count++] = bb_jobs[i];
			}
		}
		if (current_config.m - current_machine_id == 1)
		{
			cached_cost = eval() + dp_p1_single_dp(current_machine_id, uncertain_jobs, uncertain_jobs_count);
			set_completed();
		}
		return cached_cost + get_list_schedule_upper_bound(uncertain_jobs, uncertain_jobs_count, current_machine_id, completion_time);
		//return INFMAX;
	}
	int get_lower_bound()
	{
		if (is_complete())
			return eval();
		int used_machines = 0;
		int uncertain_jobs_count = 0;
		for (int i = 0; i < current_config.m + current_config.n - 1; i++)
		{
			if (!visited_node[i])
			{
				if (i >= current_config.n)
					used_machines ++;
				else
					uncertain_jobs[uncertain_jobs_count++] = bb_jobs[i];
			}
		}
		if (current_config.m - current_machine_id == 1)
		{
			cached_cost = eval() + dp_p1_single_dp(current_machine_id, uncertain_jobs, uncertain_jobs_count);
			set_completed();

			return eval();
		}
		if (current_config.m - current_machine_id == 2)
		{
			return eval() + dp_p2_dp_lowerbound_dynamic(uncertain_jobs, uncertain_jobs_count, current_machine_id, completion_time);
		}
		return eval() + dp_pn_dp_lowerbound_dynamic(uncertain_jobs, uncertain_jobs_count, current_machine_id, completion_time);
	}
	void print_current_node()
	{
		cout << "current_machine_id:" << current_machine_id <<",completion_time:" << completion_time << endl;
		for (int i = 0; i < current_config.m + current_config.n - 1; i++)
		{
			//if (!visited_node[bb_indices[i]])
			{
				cout << (int)confirmed_data[i] << " ";
			}
		}
		cout << endl;
		for (int i = 0; i < current_config.m + current_config.n - 1; i++)
		{
			//if (!visited_node[bb_indices[i]])
			{
				cout << (int)visited_node[i] << " ";
			}
		}
		cout << endl;
	}
};

// a helper function to generate the algorithm's name given the type of lowerbound
//void bblb_reg_name(bool with_lower_bound_rule)
//{
//	if (with_lower_bound_rule)
//	{
//		char lb_rule[100];
//		sprintf(bb_current_algo_name, "Branch&Bound(%DP)");
//	}
//	else
//		sprintf(bb_current_algo_name, "Branch&Bound");
//}

/*

	Main function of Brunch & Bound algorithm

	Pseudo code:

		global_upper = result of GA
		work_queue = [a blank paratial solution]

		while work_queue is not empty:

			cur_node = work_queue.pop_front()

			for all possible next solution next_node:

				if next_node.lowerbound() < global_upper and not next_node.cut_by_neighbor():

					if next_node.remain_machines <= 1:
						global_upper = min(global_upper, next_node.cost)
					else:
						work_queue.push_back(next_node)

				global_upper = min(global_upper, next_node.lowerbound())


		return global_upper

	global variables:	bb_running_time (in seconds)
						bblb_nodes_count (long long type, the number of nodes visited)

*/


int bblb_search(bool with_lower_bound_rule)
{
	LARGE_INTEGER cpu_freq, t1, t2, time_bb;
	BBNode root;
	QueryPerformanceFrequency(&cpu_freq);
	QueryPerformanceCounter(&t1);
	memcpy(bb_jobs, job_set, sizeof(job_set));
	TabuOptimizer ts(false, 50, 100, 50);
	int global_upper_bound = evalcost(ts.minimize(SPT_MINP_RULE));
	int current_upper_bound = INFMAX, current_lower_bound = INFMAX;
	queue<BBNode> work_queue;
	work_queue.push(root);
	bblb_nodes_count = 0;
	biggest_queue_length = 0;
	random_shuffle(bb_jobs, bb_jobs + current_config.n);
	while (!work_queue.empty())
	{
		QueryPerformanceCounter(&time_bb);
		if ((time_bb.QuadPart - t1.QuadPart) / cpu_freq.QuadPart > 300)
		{
			bb_timeout = true;
			return RUN_TIME_OUT;
		}
		if (biggest_queue_length < work_queue.size())
			biggest_queue_length = work_queue.size();

		BBNode current_node = work_queue.front();
		work_queue.pop();

		if (current_node.is_complete())
		{
			current_upper_bound = current_node.eval();
			global_upper_bound = min(global_upper_bound, current_upper_bound);
			continue;
		}
		if (current_node.eval() >= global_upper_bound) continue;

		for (int node_id = 0; node_id < current_config.m + current_config.n - 1; node_id++)
		{
			if (!current_node.visited_id(node_id))
			{
				BBNode next_node(current_node);
				next_node.add_confirm_id(node_id);
				if (next_node.cut_by_neighbor())
				{
					continue;
				}
				current_lower_bound = 0;

				if (with_lower_bound_rule)
					current_lower_bound = next_node.get_lower_bound();

				if (current_lower_bound >= global_upper_bound)
				{
					continue;
				}


				current_upper_bound = next_node.get_upper_bound();
				global_upper_bound = min(global_upper_bound, current_upper_bound);
				bblb_nodes_count += 1;
				if (!next_node.is_complete())
					work_queue.push(next_node);

			}
		}
	}
	QueryPerformanceCounter(&t2);

	bb_running_time = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;
	return global_upper_bound;
}

