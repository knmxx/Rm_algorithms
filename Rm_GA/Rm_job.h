#pragma once
#define VALUE_DEFAULT -1
#include <algorithm>
#include <vector>
#include "Rm_ListAlgorithm.h"
using namespace std;

const int MAX_M = 5;
const int MAX_N = 50;

const int JOB_PROCTIME_MIN = 1;
const int JOB_PROCTIME_MAX = 10;
const int RUN_TIME_OUT = -2;
/*
Job:
proc_time i: processing time when the job being proceed on machine i


test_data is a global variable, stores the current solving problem
*/

struct Job
{
	int proc_time[MAX_M];
	int due_date;
}job_set[MAX_N];

struct Configure
{
	int m, n;
	double b;
	int caseindex;
} current_config;

int get_latework(int job_index, int machine_index, int completion_time)
{
	return min(job_set[job_index].proc_time[machine_index], max(0, completion_time - job_set[job_index].due_date));
}

int job_late_work(Job& job, int machine_index, int completion_time_finished_this_job)
{
	return min(job.proc_time[machine_index], max(0, completion_time_finished_this_job - job.due_date));
}

//job≈≈–Ú
bool cmp_edd(Job j1, Job j2)
{
	return j1.due_date < j2.due_date;
}
int cmp_spt(Job j1, Job j2)
{

	return sum(j1.proc_time, current_config.m) < sum(j2.proc_time, current_config.m);
}

int cmp_lpt(Job j1, Job j2)
{

	return sum(j1.proc_time, current_config.m) > sum(j2.proc_time, current_config.m);
}
//int cmp_edd(int index_l, int index_r)
//{
//	return job_set[index_l].due_date < job_set[index_r].due_date;
//}
//int cmp_spt(int index_l, int index_r)
//{
//
//	return sum(job_set[index_l].proc_time, current_config.m) < sum(job_set[index_r].proc_time, current_config.m);
//}
//
//int cmp_lpt(int index_l, int index_r)
//{
//
//	return sum(job_set[index_l].proc_time, current_config.m) > sum(job_set[index_r].proc_time, current_config.m);
//}