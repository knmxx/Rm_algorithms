#pragma once
#include "Rm_job.h"
#include "Rm_ListAlgorithm.h"
#include "Rm_solution.h"
using namespace std;

double edd_minc_time = -1, spt_minc_time = -1, lpt_minc_time = -1;
double edd_miny_time = -1, spt_miny_time = -1, lpt_miny_time = -1;
double edd_minp_time = -1, spt_minp_time = -1, lpt_minp_time = -1;
double random_running_time = -1;

//EDD_MINC
Solution get_edd_minc_solution()
{
	LARGE_INTEGER cpu_freq, t1, t2;
	QueryPerformanceFrequency(&cpu_freq);
	QueryPerformanceCounter(&t1);
	Job sort_jobArray[MAX_N];
	memcpy(sort_jobArray, job_set, sizeof(job_set));
	sort(sort_jobArray, sort_jobArray + current_config.n, cmp_edd);
	Solution solution;
	int proc_time[MAX_M] = { 0 };
	int proc_count[MAX_M] = { 0 };
	int selected_index = 0;
	initvalue();
	for (int i = 0; i < current_config.n; i++)
	{
		selected_index = argmin(proc_time, current_config.m);
		proc_time[selected_index] += sort_jobArray[i].proc_time[selected_index];
		value[selected_index][proc_count[selected_index]++] = i;
	}
	if (!check_solution(solution))
	{
		printf("Warning: Unusual Solution found\n");
		print_solution(solution);
	}
	solution = valueToSchedule();
	solution.cost = evalcost(solution);
	QueryPerformanceCounter(&t2);
	edd_minc_time = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;
	return solution;
}

//SPT_MINC
Solution get_spt_minc_solution()
{
	LARGE_INTEGER cpu_freq, t1, t2;
	QueryPerformanceFrequency(&cpu_freq);
	QueryPerformanceCounter(&t1);
	Job sort_jobArray[MAX_N];
	memcpy(sort_jobArray, job_set, sizeof(job_set));
	sort(sort_jobArray, sort_jobArray + current_config.n, cmp_spt);
	Solution solution;
	int proc_time[MAX_M] = { 0 };
	int proc_count[MAX_M] = { 0 };
	int selected_index = 0;
	initvalue();
	for (int i = 0; i < current_config.n; i++)
	{
		selected_index = argmin(proc_time, current_config.m);
		proc_time[selected_index] += sort_jobArray[i].proc_time[selected_index];
		value[selected_index][proc_count[selected_index]++] = i;
	}
	if (!check_solution(solution))
	{
		printf("Warning: Unusual Solution found\n");
		print_solution(solution);
	}
	solution = valueToSchedule();
	solution.cost = evalcost(solution);
	QueryPerformanceCounter(&t2);
	spt_minc_time = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;
	return solution;
}

//LPT_MINC
Solution get_lpt_minc_solution()
{
	LARGE_INTEGER cpu_freq, t1, t2;
	QueryPerformanceFrequency(&cpu_freq);
	QueryPerformanceCounter(&t1);
	Job sort_jobArray[MAX_N];
	memcpy(sort_jobArray, job_set, sizeof(job_set));
	sort(sort_jobArray, sort_jobArray + current_config.n, cmp_lpt);
	Solution solution;
	int proc_time[MAX_M] = { 0 };
	int proc_count[MAX_M] = { 0 };
	int selected_index = 0;
	initvalue();
	for (int i = 0; i < current_config.n; i++)
	{
		selected_index = argmin(proc_time, current_config.m);
		proc_time[selected_index] += sort_jobArray[i].proc_time[selected_index];
		value[selected_index][proc_count[selected_index]++] = i;
	}
	if (!check_solution(solution))
	{
		printf("Warning: Unusual Solution found\n");
		print_solution(solution);
	}
	solution = valueToSchedule();
	solution.cost = evalcost(solution);
	QueryPerformanceCounter(&t2);
	lpt_minc_time = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;
	return solution;
}

//EDD_MINY
Solution get_edd_miny_solution()
{
	LARGE_INTEGER cpu_freq, t1, t2;
	QueryPerformanceFrequency(&cpu_freq);
	QueryPerformanceCounter(&t1);
	Job sort_jobArray[MAX_N];
	memcpy(sort_jobArray, job_set, sizeof(job_set));
	sort(sort_jobArray, sort_jobArray + current_config.n, cmp_edd);
	Solution solution;
	int proc_time[MAX_M] = { 0 };
	int proc_count[MAX_M] = { 0 };
	int proc_late[MAX_M] = { 0 };
	int selected_index = 0;
	initvalue();
	for (int i = 0; i < current_config.n; i++)
	{
		selected_index = argmin(proc_late, current_config.m);
		proc_time[selected_index] += sort_jobArray[i].proc_time[selected_index];
		proc_late[selected_index] += get_latework(i, selected_index, proc_time[selected_index]);
		value[selected_index][proc_count[selected_index]++] = i;
	}
	if (!check_solution(solution))
	{
		printf("Warning: Unusual Solution found\n");
		print_solution(solution);
	}
	solution = valueToSchedule();
	solution.cost = evalcost(solution);
	QueryPerformanceCounter(&t2);
	edd_miny_time = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;
	return solution;
}
//SPT_MINY
Solution get_spt_miny_solution()
{
	LARGE_INTEGER cpu_freq, t1, t2;
	QueryPerformanceFrequency(&cpu_freq);
	QueryPerformanceCounter(&t1);
	Job sort_jobArray[MAX_N];
	memcpy(sort_jobArray, job_set, sizeof(job_set));
	sort(sort_jobArray, sort_jobArray + current_config.n, cmp_spt);
	Solution solution;
	int proc_time[MAX_M] = { 0 };
	int proc_count[MAX_M] = { 0 };
	int proc_late[MAX_M] = { 0 };
	int selected_index = 0;
	initvalue();
	for (int i = 0; i < current_config.n; i++)
	{
		selected_index = argmin(proc_late, current_config.m);
		proc_time[selected_index] += sort_jobArray[i].proc_time[selected_index];
		proc_late[selected_index] += get_latework(i, selected_index, proc_time[selected_index]);
		value[selected_index][proc_count[selected_index]++] = i;
	}
	if (!check_solution(solution))
	{
		printf("Warning: Unusual Solution found\n");
		print_solution(solution);
	}
	solution = valueToSchedule();
	solution.cost = evalcost(solution);
	QueryPerformanceCounter(&t2);
	spt_miny_time = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;
	return solution;
}
//LPT_MINY
Solution get_lpt_miny_solution()
{
	LARGE_INTEGER cpu_freq, t1, t2;
	QueryPerformanceFrequency(&cpu_freq);
	QueryPerformanceCounter(&t1);
	Job sort_jobArray[MAX_N];
	memcpy(sort_jobArray, job_set, sizeof(job_set));
	sort(sort_jobArray, sort_jobArray + current_config.n, cmp_lpt);
	Solution solution;
	int proc_time[MAX_M] = { 0 };
	int proc_count[MAX_M] = { 0 };
	int proc_late[MAX_M] = { 0 };
	int selected_index = 0;
	initvalue();
	for (int i = 0; i < current_config.n; i++)
	{
		selected_index = argmin(proc_late, current_config.m);
		proc_time[selected_index] += sort_jobArray[i].proc_time[selected_index];
		proc_late[selected_index] += get_latework(i, selected_index, proc_time[selected_index]);
		value[selected_index][proc_count[selected_index]++] = i;
	}
	if (!check_solution(solution))
	{
		printf("Warning: Unusual Solution found\n");
		print_solution(solution);
	}
	solution = valueToSchedule();
	solution.cost = evalcost(solution);
	QueryPerformanceCounter(&t2);
	lpt_miny_time = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;
	return solution;
}

//EDD_MINP
Solution get_edd_minp_solution()
{
	LARGE_INTEGER cpu_freq, t1, t2;
	QueryPerformanceFrequency(&cpu_freq);
	QueryPerformanceCounter(&t1);
	Job sort_jobArray[MAX_N];
	memcpy(sort_jobArray, job_set, sizeof(job_set));
	sort(sort_jobArray, sort_jobArray + current_config.n, cmp_edd);
	Solution solution;
	int proc_count[MAX_M] = { 0 };
	int selected_index = 0;
	initvalue();
	for (int i = 0; i < current_config.n; i++)
	{
		selected_index = argmin(sort_jobArray[i].proc_time, current_config.m);
		value[selected_index][proc_count[selected_index]++] = i;
	}
	solution = valueToSchedule();
	solution.cost = evalcost(solution);
	QueryPerformanceCounter(&t2);
	edd_minp_time = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;
	return solution;
}
//SPT_MINP
Solution get_spt_minp_solution()
{
	LARGE_INTEGER cpu_freq, t1, t2;
	QueryPerformanceFrequency(&cpu_freq);
	QueryPerformanceCounter(&t1);
	Job sort_jobArray[MAX_N];
	memcpy(sort_jobArray, job_set, sizeof(job_set));
	sort(sort_jobArray, sort_jobArray + current_config.n, cmp_spt);
	Solution solution;
	int proc_count[MAX_M] = { 0 };
	int selected_index = 0;
	initvalue();
	for (int i = 0; i < current_config.n; i++)
	{
		selected_index = argmin(sort_jobArray[i].proc_time, current_config.m);
		value[selected_index][proc_count[selected_index]++] = i;
	}
	solution = valueToSchedule();
	solution.cost = evalcost(solution);
	QueryPerformanceCounter(&t2);
	spt_minp_time = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;
	return solution;
}

//LPT_MINP
Solution get_lpt_minp_solution()
{
	LARGE_INTEGER cpu_freq, t1, t2;
	QueryPerformanceFrequency(&cpu_freq);
	QueryPerformanceCounter(&t1);
	Job sort_jobArray[MAX_N];
	memcpy(sort_jobArray, job_set, sizeof(job_set));
	sort(sort_jobArray, sort_jobArray + current_config.n, cmp_lpt);
	Solution solution;
	int proc_count[MAX_M] = { 0 };
	int selected_index = 0;
	initvalue();
	for (int i = 0; i < current_config.n; i++)
	{
		selected_index = argmin(sort_jobArray[i].proc_time, current_config.m);
		value[selected_index][proc_count[selected_index]++] = i;
	}
	solution = valueToSchedule();
	solution.cost = evalcost(solution);
	QueryPerformanceCounter(&t2);
	lpt_minp_time = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;
	return solution;
}
Solution get_random_solution()
{
	LARGE_INTEGER cpu_freq, t1, t2;
	QueryPerformanceFrequency(&cpu_freq);
	QueryPerformanceCounter(&t1);
	Solution solution;
	int m, s;
	initvalue();
	for (int i = 0; i < current_config.n; i++)
	{
		m = u(0, current_config.m);
		s = u(0, current_config.n);
		if (value[m][s] == VALUE_DEFAULT)
			value[m][s] = i;
		else
			i--;
	}
	if (!check_solution(solution))
	{
		printf("Warning: Unusual Solution found\n");
		print_solution(solution);
	}
	solution = valueToSchedule();
	solution.cost = evalcost(solution);
	QueryPerformanceCounter(&t2);
	random_running_time = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;
	return solution;
}




int get_edd_minc_latework(Job* job_ids, int job_count, int machine_id, int last_completion_time);
int get_spt_minc_latework(Job* job_ids, int job_count, int machine_id, int last_completion_time);
int get_lpt_minc_latework(Job* job_ids, int job_count, int machine_id, int last_completion_time);
int get_edd_miny_latework(Job* job_ids, int job_count, int machine_id, int last_completion_time);
int get_spt_miny_latework(Job* job_ids, int job_count, int machine_id, int last_completion_time);
int get_lpt_miny_latework(Job* job_ids, int job_count, int machine_id, int last_completion_time);





int get_list_schedule_upper_bound(Job* job_ids, int job_count, int machine_id, int last_completion_time)
{
	int edd_minc_result = 0, rspt_minc_result = 0, rlpt_minc_result = 0, edd_miny_result = 0, rspt_miny_result = 0, rlpt_miny_result = 0;
	int edd_minp_result = 0, rspt_minp_result = 0, rlpt_minp_result = 0, random_result = 0;

	edd_minc_result = get_edd_minc_latework(job_ids,job_count, machine_id, last_completion_time);
	rspt_minc_result = get_spt_minc_latework(job_ids, job_count, machine_id, last_completion_time);
	rlpt_minc_result = get_lpt_minc_latework(job_ids, job_count, machine_id, last_completion_time);

	edd_miny_result = get_edd_miny_latework(job_ids, job_count, machine_id, last_completion_time);
	rspt_miny_result = get_spt_miny_latework(job_ids, job_count, machine_id, last_completion_time);
	rlpt_miny_result = get_lpt_miny_latework(job_ids, job_count, machine_id, last_completion_time);

	double record[6] = {
		edd_minc_result,
		rspt_minc_result,
		rlpt_minc_result,
		edd_miny_result,
		rspt_miny_result,
		rlpt_miny_result,
	};
	//edd_minc_result,
	//	rspt_minc_result,
	//	rlpt_minc_result,
	//	edd_miny_result,
	//	rspt_miny_result,
	//	rlpt_miny_result,
	double minresult = 99999999;
	int minindex = -1;
	for (int i = 0; i < 6; ++i)
	{
		if (minresult > record[i])
		{
			minresult = record[i];
			minindex = i;
		}

	}
	//cout << "minindex:" << minindex << ",minresult:" << minresult << endl;
	//if (minindex == -1) return 0;
	return minresult;
	//return 99999999;
}

//NEW_EDD_MINC_latework
int get_edd_minc_latework(Job* job_ids, int job_count, int machine_id, int last_completion_time)
{
	Job machine_sequence[MAX_M][MAX_N];
	int machine_sequence_length[MAX_M] = { 0 };
	int machine_completion_time[MAX_M] = { 0 };
	machine_completion_time[machine_id] = last_completion_time;
	int expected_late_work = 0, assigned_machine_id = -1, current_pointer = 0, splitter = job_count;
	sort(job_ids, job_ids + job_count, cmp_edd);
	for (int i = 0; i < job_count; i++)
	{
		Job& current_job = job_ids[i];
		assigned_machine_id = argmin(machine_completion_time, current_config.m, machine_id);

		machine_completion_time[assigned_machine_id] += current_job.proc_time[assigned_machine_id];
		expected_late_work += job_late_work(current_job, assigned_machine_id, machine_completion_time[assigned_machine_id]);

		machine_sequence[assigned_machine_id][machine_sequence_length[assigned_machine_id] ++] = job_ids[i];
	}
	return expected_late_work;
}
//SPT_MINC_latework
int get_spt_minc_latework(Job* job_ids, int job_count, int machine_id, int last_completion_time)
{


	Job machine_sequence[MAX_M][MAX_N];
	int machine_sequence_length[MAX_M] = { 0 };
	int machine_completion_time[MAX_M] = { 0 };
	machine_completion_time[machine_id] = last_completion_time;
	int expected_late_work = 0, assigned_machine_id = -1, current_pointer = 0, splitter = job_count;
	sort(job_ids, job_ids + job_count, cmp_spt);
	for (int i = 0; i < job_count; i++)
	{
		Job& current_job = job_ids[i];
		assigned_machine_id = argmin(machine_completion_time, current_config.m, machine_id);

		machine_completion_time[assigned_machine_id] += current_job.proc_time[assigned_machine_id];
		expected_late_work += job_late_work(current_job, assigned_machine_id, machine_completion_time[assigned_machine_id]);

		machine_sequence[assigned_machine_id][machine_sequence_length[assigned_machine_id] ++] = job_ids[i];
	}
	return expected_late_work;
}

//LPT_MINC_latework
int get_lpt_minc_latework(Job* job_ids, int job_count, int machine_id, int last_completion_time)
{


	Job machine_sequence[MAX_M][MAX_N];
	int machine_sequence_length[MAX_M] = { 0 };
	int machine_completion_time[MAX_M] = { 0 };
	machine_completion_time[machine_id] = last_completion_time;
	int expected_late_work = 0, assigned_machine_id = -1, current_pointer = 0, splitter = job_count;
	sort(job_ids, job_ids + job_count, cmp_lpt);
	for (int i = 0; i < job_count; i++)
	{
		Job& current_job = job_ids[i];
		assigned_machine_id = argmin(machine_completion_time, current_config.m, machine_id);

		machine_completion_time[assigned_machine_id] += current_job.proc_time[assigned_machine_id];
		expected_late_work += job_late_work(current_job, assigned_machine_id, machine_completion_time[assigned_machine_id]);

		machine_sequence[assigned_machine_id][machine_sequence_length[assigned_machine_id] ++] = job_ids[i];
	}
	return expected_late_work;
}
//EDD_MINY_latework
int get_edd_miny_latework(Job* job_ids, int job_count, int machine_id, int last_completion_time)
{
	Job machine_sequence[MAX_M][MAX_N];
	int machine_sequence_length[MAX_M] = { 0 };
	int machine_completion_time[MAX_M] = { 0 };
	int machine_late_work[MAX_M] = { 0 };
	int expected_late_work = 0, assigned_machine_id = -1, current_pointer = 0, splitter = job_count, current_late_work;
	machine_completion_time[machine_id] = last_completion_time;
	sort(job_ids, job_ids + job_count, cmp_edd);
	for (int i = 0; i < job_count; i++)
	{
		Job& current_job = job_ids[i];
		assigned_machine_id = argmin(machine_late_work, current_config.m, machine_id);

		machine_completion_time[assigned_machine_id] += current_job.proc_time[assigned_machine_id];

		current_late_work = job_late_work(current_job, assigned_machine_id, machine_completion_time[assigned_machine_id]);
		expected_late_work += current_late_work;
		machine_late_work[assigned_machine_id] += current_late_work;

		machine_sequence[assigned_machine_id][machine_sequence_length[assigned_machine_id] ++] = job_ids[i];
	}
	return expected_late_work;
}
//SPT_MINY_latework
int get_spt_miny_latework(Job* job_ids, int job_count, int machine_id, int last_completion_time)
{
	Job machine_sequence[MAX_M][MAX_N];
	int machine_sequence_length[MAX_M] = { 0 };
	int machine_completion_time[MAX_M] = { 0 };
	int machine_late_work[MAX_M] = { 0 };
	int expected_late_work = 0, assigned_machine_id = -1, current_pointer = 0, splitter = job_count, current_late_work;
	machine_completion_time[machine_id] = last_completion_time;
	sort(job_ids, job_ids + job_count, cmp_spt);
	for (int i = 0; i < job_count; i++)
	{
		Job& current_job = job_ids[i];
		assigned_machine_id = argmin(machine_late_work, current_config.m, machine_id);

		machine_completion_time[assigned_machine_id] += current_job.proc_time[assigned_machine_id];

		current_late_work = job_late_work(current_job, assigned_machine_id, machine_completion_time[assigned_machine_id]);
		expected_late_work += current_late_work;
		machine_late_work[assigned_machine_id] += current_late_work;

		machine_sequence[assigned_machine_id][machine_sequence_length[assigned_machine_id] ++] = job_ids[i];
	}
	return expected_late_work;
}
//LPT_MINY_latework
int get_lpt_miny_latework(Job* job_ids, int job_count, int machine_id, int last_completion_time)
{
	Job machine_sequence[MAX_M][MAX_N];
	int machine_sequence_length[MAX_M] = { 0 };
	int machine_completion_time[MAX_M] = { 0 };
	int machine_late_work[MAX_M] = { 0 };
	int expected_late_work = 0, assigned_machine_id = -1, current_pointer = 0, splitter = job_count, current_late_work;
	machine_completion_time[machine_id] = last_completion_time;
	sort(job_ids, job_ids + job_count, cmp_lpt);
	for (int i = 0; i < job_count; i++)
	{
		Job& current_job = job_ids[i];
		assigned_machine_id = argmin(machine_late_work, current_config.m, machine_id);

		machine_completion_time[assigned_machine_id] += current_job.proc_time[assigned_machine_id];

		current_late_work = job_late_work(current_job, assigned_machine_id, machine_completion_time[assigned_machine_id]);
		expected_late_work += current_late_work;
		machine_late_work[assigned_machine_id] += current_late_work;

		machine_sequence[assigned_machine_id][machine_sequence_length[assigned_machine_id] ++] = job_ids[i];
	}
	return expected_late_work;
}