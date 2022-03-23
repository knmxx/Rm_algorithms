#pragma once

#include "Rm_job.h"
struct Solution
{
public:
	int schedule[MAX_M + MAX_N];
	int cost;
	
	Solution()
	{
		cost = -1;
	}
	bool equals(const Solution& solution)
	{
		for (int i = 0; i < current_config.m+ current_config.n-1; i++)
		{
			if (memcmp(schedule, solution.schedule, (current_config.m + current_config.n - 1) * sizeof(int)) != 0)
				return false;
		}
		return true;
	}
};

int value[MAX_M][MAX_N];//value[i][j], the j-th processing job on machine i， just for Tabu

void initvalue()
{
	for (int i = 0; i < MAX_M; i++)
		for (int j = 0; j < MAX_N; j++)
			value[i][j] = VALUE_DEFAULT;//-1
}
int evalcost(const Solution& solution)
{
	int latework = 0;
	int machineindex = 0;
	int completion = 0;//每个job的完工时间
	for (int i = 0; i < current_config.n + current_config.m - 1; i++)
	{
		if (solution.schedule[i] >= current_config.n) // i < N, i is a job id, i >= N, i is a splitter
		{
			machineindex++;
			completion = 0;
		}

		else
		{
			Job& job = job_set[solution.schedule[i]];
			completion += job.proc_time[machineindex];
			latework += min(job.proc_time[machineindex], max(0, completion - job.due_date));
		}
	}
	return latework;
}
Solution valueToSchedule()
{
	Solution solution;
	int machineindex = current_config.n;
	int scheduleindex = 0;
	for (int i = 0; i < current_config.m; i++)
	{
		for (int j = 0; j < current_config.n; j++)
		{
			if (value[i][j] != VALUE_DEFAULT)
			{
				solution.schedule[scheduleindex++] = value[i][j];
			}
		}
		solution.schedule[scheduleindex++] = machineindex++;
	}
	return solution;
}

bool equals(const Solution& solution)
{
	for (int i = 0; i < current_config.m; i++)
	{
		if (memcmp(value[i], value[i], current_config.n * sizeof(int)) != 0)
			return false;
	}
	return true;
}
bool check_solution(const Solution& solution)
{
	int jobs[MAX_N];
	int count = 0;
	for (int i = 0; i < current_config.n; i++)
	{
		jobs[i] = 0;
	}
	for (int i = 0; i < current_config.m; i++)
	{
		for (int j = 0; j < current_config.n; j++)
		{
			if (value[i][j] != VALUE_DEFAULT)
			{
				count++;
				jobs[value[i][j]] ++;
				if (jobs[value[i][j]] > 1) return false;
			}
		}
	}

	return count == current_config.n;
}

void print_solution(const Solution& solution, const char* tagName = NULL)
{
	draw_line(10);
	if (tagName != NULL)
	{
		printf("Solution: %s\n", tagName);
	}
	int proc_time[MAX_M] = { 0 };
	for (int i = 0; i < current_config.m; i++)
	{
		for (int j = 0; j < current_config.n; j++)
		{
			if (value[i][j] != VALUE_DEFAULT)
				proc_time[i] += job_set[value[i][j]].proc_time[i];
		}
	}
	for (int i = 0; i < current_config.m; i++)
	{
		printf("M%d (Process Time = %d): ", i, proc_time[i]);
		for (int j = 0; j < current_config.n; j++)
		{
			if (value[i][j] != VALUE_DEFAULT)
			{
				Job &current_job = job_set[value[i][j]];
				printf("%d ", value[i][j]);
			}
		}
		printf("\n");

	}

	printf("Value = %d\n", evalcost(solution));
	draw_line(10);
}