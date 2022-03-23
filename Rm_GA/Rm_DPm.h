#pragma once

#include "Rm_job.h"
#include "Rm_ListAlgorithm.h"
#include <string.h>
#include <algorithm>
#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
using namespace std;

const int dpm_inf = 10000007;

/*

	In large-scale N tests, static dynamic programming array should be disabled, that's why there is a min(40, N) in array allocation.

*/
//²âÊÔ£º
double dpm_running_time;
int dpm_result = dpm_inf;
Job pdm_jobs[MAX_N];
int dp_p2_dp_table[(MAX_N > 40 ? 40 : MAX_N) + 1][(MAX_N > 40 ? 40 : MAX_N) * JOB_PROCTIME_MAX][(MAX_N > 40 ? 40 : MAX_N) * JOB_PROCTIME_MAX] = {0};

int dp_pn_dp_table_last_n = -1, dp_pn_dp_table_last_m = -1;
void** dp_pn_dp_table = NULL;

int dp_p2_dp_lowerbound(Job* job_ids, int job_count)
{
	int max_due_date = job_ids[0].due_date, proc_sum[MAX_M] = { 0 };
	int left, right, temp_index;
	for (int i = 0; i < job_count; i++) {
		max_due_date = max(max_due_date, job_ids[i].due_date);
		for (int j = 0; j < current_config.m; j++)
		{
			proc_sum[j] += job_ids[i].proc_time[j];
		}
		
	}

	if (proc_sum[0] <= max_due_date && proc_sum[1] <= max_due_date)
		return 0;

	for (int m1 = 0; m1 <= max_due_date; m1++)
	{
		for (int m2 = 0; m2 <= max_due_date; m2++)
		{
			left = max(0, job_ids[0].proc_time[0] - m1);
			right = max(0, job_ids[0].proc_time[1] - m2);
			dp_p2_dp_table[0][m1][m2] = min(left, right);
		}
	}

	for (int i = 1; i < job_count; i++)
	{
		Job& current_job = job_ids[i];
		for (int m1 = 0; m1 <= max_due_date; m1++)
		{
			for (int m2 = 0; m2 <= max_due_date; m2++)
			{
				temp_index = max(m1 - current_job.proc_time[0], 0);
				left = max(current_job.proc_time[0] - m1, 0) + dp_p2_dp_table[i - 1][temp_index][m2];

				temp_index = max(m2 - current_job.proc_time[1], 0);
				right = max(current_job.proc_time[1] - m2, 0) + dp_p2_dp_table[i - 1][m1][temp_index];

				dp_p2_dp_table[i][m1][m2] = min(left, right);
			}
		}
	}
	return dp_p2_dp_table[job_count - 1][max_due_date][max_due_date];
}

int dp_p2_dp_lowerbound_dynamic(Job* job_ids, int job_count,int current_machine_id, int completion_time_in_m1)
{
	int max_due_date = 0, total_proc = 0;
	register int left, right, temp_index;
	for (int i = 0; i < job_count; i++)
	{
		max_due_date = max(max_due_date, job_ids[i].due_date);
		total_proc += min(job_ids[i].proc_time[current_machine_id], job_ids[i].proc_time[1 + current_machine_id]);
		
	}

	if (max_due_date <= completion_time_in_m1)
		return total_proc - max_due_date;

	int m1_time = max(0, max_due_date - completion_time_in_m1);
	completion_time_in_m1 = min(completion_time_in_m1, max_due_date);
	for (int i = 1; i <= job_count; i++)
	{
		Job& current_job = job_ids[i - 1];
		for (int m1 = completion_time_in_m1; m1 <= max_due_date; m1++)
		{
			for (int m2 = 0; m2 <= max_due_date; m2++)
			{
				temp_index = max(m1 - current_job.proc_time[current_machine_id], completion_time_in_m1);
				left = max(current_job.proc_time[current_machine_id] - m1, 0) + dp_p2_dp_table[i - 1][temp_index][m2];

				temp_index = max(m2 - current_job.proc_time[1 + current_machine_id], 0);
				right = max(current_job.proc_time[1 + current_machine_id] - m2, 0) + dp_p2_dp_table[i - 1][m1][temp_index];

				dp_p2_dp_table[i][m1][m2] = min(left, right);

			}
		}
	}
	int result = dp_p2_dp_table[job_count][max_due_date][max_due_date];
	return result;
}

void* allocate_multidimension_array(int dim_size, int dim_num)
{
	if (dim_num == 1)
	{
		void* pointer = new int[dim_size];
		memset(pointer, 0, sizeof(int) * dim_size);
		return pointer;
	}
	void** pointer = new void* [dim_size];
	for (int i = 0; i < dim_size; i++)
	{
		pointer[i] = allocate_multidimension_array(dim_size, dim_num - 1);
	}
	return pointer;
}

void free_multidimension_array(int dim_size, int dim_num, void** pointer)
{
	if (dim_num > 1)
	{
		for (int i = 0; i < dim_size; i++)
		{
			free_multidimension_array(dim_size, dim_num - 1, (void**)pointer[i]);
		}
	}
	if (dim_num == 1)
		delete[]((int*)pointer);
	else
		delete[] pointer;

}

int& multidim_array_index(int* index, int index_count, void** table)
{
	for (int i = 0; i < index_count - 1; i++)
	{
		table = (void**)table[index[i]];
	}
	return ((int*)table)[index[index_count - 1]];
}

int dp_pn_dp_lowerbound_index_array[10];

int dp_pn_dp_lowerbound_machine_value(int current_job_id, Job& current_job, int available_machines, int machine_id, void** table)
{
	int current_early_work = dp_pn_dp_lowerbound_index_array[machine_id];
	int temp_index = max(0, current_early_work - current_job.proc_time[machine_id]);
	int backup = dp_pn_dp_lowerbound_index_array[machine_id];
	dp_pn_dp_lowerbound_index_array[machine_id] = temp_index;

	int result = max(current_job.proc_time[machine_id] - current_early_work, 0);
	if (current_job_id > 0)
		result += multidim_array_index(dp_pn_dp_lowerbound_index_array, available_machines, (void**)table[current_job_id - 1]);
	dp_pn_dp_lowerbound_index_array[machine_id] = backup;

	return result;
}

void dp_pn_fill_table_for(int current_deep, int due_date, int available_machines, int job_id, Job& current_job, void** table)
{
	int min_temp = 0;
	if (current_deep == available_machines)
	{
		min_temp = 100000007;
		for (int i = 0; i < available_machines; i++)
		{
			min_temp = min(min_temp, dp_pn_dp_lowerbound_machine_value(job_id, current_job, available_machines, i, table));
		}
		multidim_array_index(dp_pn_dp_lowerbound_index_array, available_machines, (void**)table[job_id]) = min_temp;
	}
	else
	{
		for (int i = 0; i <= due_date; i++)
		{
			dp_pn_dp_lowerbound_index_array[current_deep] = i;
			dp_pn_fill_table_for(current_deep + 1, due_date, available_machines, job_id, current_job, table);
		}
	}

}

void free_dp_pn_table()
{
	if (!dp_pn_dp_table) return;
	for (int i = 0; i < dp_pn_dp_table_last_n; i++)
	{
		free_multidimension_array(MAX_N * JOB_PROCTIME_MAX, dp_pn_dp_table_last_m, (void**)dp_pn_dp_table[i]);
	}
	delete[] dp_pn_dp_table;
}

void initialize_dp_pn_table(int job_count, int available_machines)
{
	if (dp_pn_dp_table_last_m != available_machines || job_count > dp_pn_dp_table_last_n)
	{
		free_dp_pn_table();
		dp_pn_dp_table_last_m = available_machines;
		dp_pn_dp_table_last_n = job_count;
		dp_pn_dp_table = new void* [job_count];
		for (int i = 0; i < job_count; i++)
		{
			dp_pn_dp_table[i] = allocate_multidimension_array(MAX_N * JOB_PROCTIME_MAX, available_machines);
		}
	}
}

int dp_p3_dp_lowerbound_dynamic(Job* job_ids, int job_count,int current_machine_id, int last_completion_time_in_m1)
{

	int*** table[2];
	int total_proc_time[MAX_M] = { 0 }, due_date = 0;

	for (int i = 0; i < job_count; i++)
	{
		
		due_date = max(due_date, job_ids[i].due_date);
		for (int j = 0; j < current_config.m - current_machine_id; j++)//4-1=3, 0 1 2, cmid=1, 1,2,3
		{
			total_proc_time[j] += job_ids[i].proc_time[j + current_machine_id];
		}
	}

	table[0] = (int***)allocate_multidimension_array(due_date + 1, 3);
	table[1] = (int***)allocate_multidimension_array(due_date + 1, 3);

	int m1_finished_time = max(due_date - last_completion_time_in_m1, 0);

	int array_index_m[3] = { 0 }, resi_sum_m[3] = { 0 }, m[3] = { 0 };

	for (int i = 0; i < job_count; i++)
	{
		Job& current_job = job_ids[i];
		for (m[0] = 0; m[0] <= m1_finished_time; m[0]++)
		{
			for (m[1] = 0; m[1] <= due_date; m[1]++)
			{
				for (m[2] = 0; m[2] <= due_date; m[2]++)
				{
					for (int mi = current_machine_id; mi < 3; mi++)
					{
						resi_sum_m[mi] = current_job.proc_time[mi] - m[mi];
						array_index_m[mi] = 0;
						if (resi_sum_m[mi] < 0)
						{
							array_index_m[mi] = -resi_sum_m[mi];
							resi_sum_m[mi] = 0;
						}
					}

					resi_sum_m[0] += table[(i + 1) % 2][array_index_m[0]][m[1]][m[2]];
					resi_sum_m[1] += table[(i + 1) % 2][m[0]][array_index_m[1]][m[2]];
					resi_sum_m[2] += table[(i + 1) % 2][m[0]][m[1]][array_index_m[2]];

					table[i % 2][m[0]][m[1]][m[2]] = resi_sum_m[argmin(resi_sum_m, 3)];
				}
			}
		}
	}

	int ans = table[(job_count + 1) % 2][m1_finished_time][due_date][due_date];

	free_multidimension_array(due_date + 1, 3, (void**)table[0]);
	free_multidimension_array(due_date + 1, 3, (void**)table[1]);

	return ans;
}

int dp_p4_dp_lowerbound_dynamic(Job* job_ids, int job_count,int current_machine_id, int last_completion_time_in_m1)
{
	int**** table[2];
	int total_proc_time[MAX_M] = { 0 }, due_date = 0;

	for (int i = 0; i < job_count; i++)
	{
		due_date = max(due_date, job_ids[i].due_date);
		for (int j = 0; j < current_config.m - current_machine_id; j++)
		{
			total_proc_time[j] += job_ids[i].proc_time[j + current_machine_id];
		}
	}

	table[0] = (int****)allocate_multidimension_array(due_date + 1, 4);
	table[1] = (int****)allocate_multidimension_array(due_date + 1, 4);

	int m1_finished_time = max(0, due_date - last_completion_time_in_m1);

	int array_index_m[4] = { 0 }, resi_sum_m[4] = { 0 }, m[4] = { 0 };

	for (int i = 0; i < job_count; i++)
	{
		Job& current_job = job_ids[i];
		for (m[0] = 0; m[0] <= m1_finished_time; m[0]++)
		{
			for (m[1] = 0; m[1] <= due_date; m[1]++)
			{
				for (m[2] = 0; m[2] <= due_date; m[2]++)
				{
					for (m[3] = 0; m[3] <= due_date; m[3] ++)
					{
						for (int mi = current_machine_id; mi < 4; mi++)
						{
							resi_sum_m[mi] = current_job.proc_time[mi] - m[mi];
							array_index_m[mi] = 0;
							if (resi_sum_m[mi] < 0)
							{
								array_index_m[mi] = -resi_sum_m[mi];
								resi_sum_m[mi] = 0;
							}
						}

						resi_sum_m[0] += table[(i + 1) % 2][array_index_m[0]][m[1]][m[2]][m[3]];
						resi_sum_m[1] += table[(i + 1) % 2][m[0]][array_index_m[1]][m[2]][m[3]];
						resi_sum_m[2] += table[(i + 1) % 2][m[0]][m[1]][array_index_m[2]][m[3]];
						resi_sum_m[3] += table[(i + 1) % 2][m[0]][m[1]][m[2]][array_index_m[3]];

						table[i % 2][m[0]][m[1]][m[2]][m[3]] = resi_sum_m[argmin(resi_sum_m, 4)];
					}
				}
			}
		}
	}

	int ans = table[(job_count + 1) % 2][m1_finished_time][due_date][due_date][due_date];

	free_multidimension_array(due_date + 1, 4, (void**)table[0]);
	free_multidimension_array(due_date + 1, 4, (void**)table[1]);

	return ans;
}

int dp_p5_dp_lowerbound_dynamic(Job* job_ids, int job_count, int available_machines,int last_completion_time_in_m1)
{
	int***** table[2];
	int total_proc_time[MAX_M] = { 0 }, due_date = 0;

	for (int i = 0; i < job_count; i++)
	{
		due_date = max(due_date, job_ids[i].due_date);
		for (int j = 0; j < available_machines; j++)
		{
			total_proc_time[j] += job_ids[i].proc_time[j];
		}
	}

	table[0] = (int*****)allocate_multidimension_array(due_date + 1, 5);
	table[1] = (int*****)allocate_multidimension_array(due_date + 1, 5);

	int m1_finished_time = max(0, due_date - last_completion_time_in_m1);

	int array_index_m[5], resi_sum_m[5], m[5];

	for (int i = 0; i < job_count; i++)
	{
		Job& current_job = job_ids[i];
		for (m[0] = 0; m[0] <= m1_finished_time; m[0]++)
		{
			for (m[1] = 0; m[1] <= due_date; m[1]++)
			{
				for (m[2] = 0; m[2] <= due_date; m[2]++)
				{
					for (m[3] = 0; m[3] <= due_date; m[3] ++)
					{
						for (m[4] = 0; m[4] <= due_date; m[4] ++)
						{
							for (int mi = 0; mi < 5; mi++)
							{
								resi_sum_m[mi] = current_job.proc_time[mi] - m[mi];
								array_index_m[mi] = 0;
								if (resi_sum_m[mi] < 0)
								{
									array_index_m[mi] = -resi_sum_m[mi];
									resi_sum_m[mi] = 0;
								}
							}

							resi_sum_m[0] += table[(i + 1) % 2][array_index_m[0]][m[1]][m[2]][m[3]][m[4]];
							resi_sum_m[1] += table[(i + 1) % 2][m[0]][array_index_m[1]][m[2]][m[3]][m[4]];
							resi_sum_m[2] += table[(i + 1) % 2][m[0]][m[1]][array_index_m[2]][m[3]][m[4]];
							resi_sum_m[3] += table[(i + 1) % 2][m[0]][m[1]][m[2]][array_index_m[3]][m[4]];
							resi_sum_m[4] += table[(i + 1) % 2][m[0]][m[1]][m[2]][m[3]][array_index_m[4]];

							table[i % 2][m[0]][m[1]][m[2]][m[3]][m[4]] = resi_sum_m[argmin(resi_sum_m, 5)];
						}
					}
				}
			}
		}
	}

	int ans = table[(job_count + 1) % 2][m1_finished_time][due_date][due_date][due_date][due_date];

	free_multidimension_array(due_date + 1, 5, (void**)table[0]);
	free_multidimension_array(due_date + 1, 5, (void**)table[1]);

	return ans;
}

int dp_pn_dp_lowerbound_dynamic(Job* job_ids, int job_count,int current_machine_id, int completion_time_in_m1)
{
	if (current_config.m - current_machine_id < 2)
	{
		perror("Critical error: available machines not enough\n");
		exit(0);
	}
	if (current_config.m - current_machine_id > 5)
	{
		perror("Error: Memory limited.\n");
		exit(0);
	}
	if (current_config.m - current_machine_id == 2)
		return dp_p2_dp_lowerbound_dynamic(job_ids, job_count, current_machine_id, completion_time_in_m1);
	if (current_config.m - current_machine_id == 3)
		return dp_p3_dp_lowerbound_dynamic(job_ids, job_count, current_machine_id, completion_time_in_m1);
	if (current_config.m - current_machine_id == 4)
		return dp_p4_dp_lowerbound_dynamic(job_ids, job_count, current_machine_id, completion_time_in_m1);
	if (current_config.m - current_machine_id == 5)
		return dp_p5_dp_lowerbound_dynamic(job_ids, job_count, current_machine_id, completion_time_in_m1);



	int due_date = 0, total_proc[MAX_M] = { 0 };
	initialize_dp_pn_table(job_count, current_config.m - current_machine_id);
	for (int i = 0; i < job_count; i++)
	{
		due_date = max(due_date, job_ids[i].due_date);
		for (int j = 0; j < current_config.m - current_machine_id; j++)
		{
			total_proc[j] += job_ids[i].proc_time[j + current_machine_id];
		}
	}

	for (int i = 0; i < job_count; i++)
	{
		Job& current_job = job_ids[i];
		dp_pn_fill_table_for(0, due_date, current_config.m - current_machine_id, i, current_job, (void**)dp_pn_dp_table);
	}

	for (int i = 0; i < current_config.m - current_machine_id; i++)
		dp_pn_dp_lowerbound_index_array[i] = due_date;
	dp_pn_dp_lowerbound_index_array[0] = max(0, due_date - completion_time_in_m1);
	int result = multidim_array_index(dp_pn_dp_lowerbound_index_array, current_config.m - current_machine_id, (void**)dp_pn_dp_table[job_count - 1]);
	return result;
}


//²âÊÔ£º

int dpm_solve()
{
	LARGE_INTEGER cpu_freq, t1, t2;

	QueryPerformanceFrequency(&cpu_freq);
	QueryPerformanceCounter(&t1);


	memcpy(pdm_jobs, job_set, sizeof(job_set));

	
	dpm_result = dp_pn_dp_lowerbound_dynamic(pdm_jobs, current_config.n, 0, 0);
	QueryPerformanceCounter(&t2);

	dpm_running_time = 1.0 * (t2.QuadPart - t1.QuadPart) / cpu_freq.QuadPart;

	return dpm_result;
}