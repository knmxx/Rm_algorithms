#pragma once
#include <algorithm>
#include "Rm_job.h"
#include "Rm_ListAlgorithm.h"

using namespace std;


const int dp_INF = 0x7fffff;

void Init(int& B, int& piRange);
int CalLateWorkByDP(int machine_id, Job* jobList, int job_count);

int CalCompleteTimeBound(int machine_id,Job* jobList, int job_count)
{
	int sumP = 0;
	int maxDP = 0;
	for (int i = 0; i < job_count; i++)
	{
		sumP += jobList[i].proc_time[machine_id];
		int tep = jobList[i].proc_time[machine_id] + jobList[i].due_date - 1;
		if (tep > maxDP)
		{
			maxDP = tep;
		}

	}
	return min(sumP, maxDP);
}

int dp_p1_single_dp(int machine_id, Job* job_ids, int job_count)
{
	
	sort(job_ids, job_ids + job_count, cmp_edd);
	vector<vector<int>>run_arry(job_count + 1);
	run_arry.resize(job_count + 1);
	int T = CalCompleteTimeBound(machine_id, job_ids, job_count);
	for (int i = 0; i <= job_count; i++)
	{
		run_arry[i].resize(T + 1);
		for (int j = 0; j <= T; j++)
		{
			if (j == 0 && i == 0)
			{
				run_arry[i][j] = 0;
			}
			else
			{
				run_arry[i][j] = dp_INF;
			}
		}
	}

	Job curJob;
	int tempVal = 0;
	for (int j = 1; j <= job_count; j++)
	{
		curJob = job_ids[j - 1];
		for (int t = 0; t <= T; t++)
		{
			if (t < curJob.proc_time[machine_id] + curJob.due_date)
			{
				if (t < curJob.proc_time[machine_id])
				{
					tempVal = dp_INF;
				}
				else
				{
					tempVal = run_arry[j - 1][t - curJob.proc_time[machine_id]];
				}
				run_arry[j][t] = min(tempVal + max(t - curJob.due_date, 0), run_arry[j - 1][t] + curJob.proc_time[machine_id]);
			}
			else
			{
				run_arry[j][t] = run_arry[j - 1][t] + curJob.proc_time[machine_id];
			}
		}
	}
	int totalLateWork = dp_INF;
	for (int i = 0; i <= T; i++)
	{
		if (run_arry[job_count][i] < totalLateWork)
		{
			totalLateWork = run_arry[job_count][i];
		}
	}

	return totalLateWork;
}

