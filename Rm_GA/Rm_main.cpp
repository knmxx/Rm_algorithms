#include<iostream>
#include <SDKDDKVer.h>
#include <mutex>
#include "Rm_DataReader.h"
#include "Rm_GA.h"
#include "Rm_Tabu.h"
#include "Rm_ListSchedule.h"
#include "Rm_PDP.h"
#include "Rm_DPm.h"
#include "Rm_BB.h"

using namespace std;

timed_mutex mtx;
const int TASK_TABU_RSPT_MINP = 1, TASK_GA = 2, TASK_PDP = 4, TASK_DPm = 8, TASK_BB_DP = 16, TASK_RAMDOM = 32,TASK_SINGLE_DP = 64;
int result_error = 0;

void dump_data(FILE* fp, const char* algo_name, double result, double running_time, double rate, long long nodes)
{
		fprintf(fp, "%d, %d, %f, %s, %f, %f, %f, %ld, %d\n", current_config.m, current_config.n, current_config.b, algo_name, result,  running_time, rate, nodes ,1);
	// flush immediately to avoid data loss due to program crash
	fflush(fp);

	printf("[%s] result = %.0f, running_time = %.4f\n", algo_name, result, running_time);
}

void run_algorithm(FILE* result_file, int flags)
{
	double tabu_spt_minp_result = -1, ga_result = -1, DPm_result = -1, pdp_result = -1, bb_result = -1, random_result = -1, single_dp_result = -1,final_result = -1;
	int correct_answer = pdp_solve();

	if (flags & TASK_BB_DP)
	{
		bb_result = bblb_search(true);
		
		if (bb_result == RUN_TIME_OUT)
		{
			printf("BB run time out.\n");
		}
		else
		{
			final_result = bb_result;
			dump_data(result_file, "BB_DP", bb_result, 1.0 * bb_running_time, 1, bblb_nodes_count);//second
		}
			
	}

	if (flags & TASK_PDP)
	{
		pdp_result = pdp_solve();
		if (pdp_result == RUN_TIME_OUT)
		{
			printf("PDP run time out.\n");
		}
		else
		{
			final_result = pdp_result;
			dump_data(result_file, "PDP", pdp_result, 1.0 * pdp_running_time, 1, 0);
		}
			
	}
	if (bb_result == RUN_TIME_OUT && pdp_result == RUN_TIME_OUT)
		return;

	if (flags & TASK_TABU_RSPT_MINP)
	{
		TabuOptimizer t1(false, 50, 100, 50);
		tabu_spt_minp_result = evalcost(t1.minimize(SPT_MINP_RULE));
		if (tabu_spt_minp_result == correct_answer)
		{
			dump_data(result_file, "TABU", tabu_spt_minp_result, 1000.0 * t1.get_ts_running_time(), 1,0);
		}
		else
			dump_data(result_file, "TABU", tabu_spt_minp_result, 1000.0 *t1.get_ts_running_time(), 1.0 * tabu_spt_minp_result / correct_answer,0);
	}

	if (flags & TASK_GA)
	{
		ga_result = ga_solve(TRUE, 200, 0.95, 0.2);
		if(ga_result == correct_answer)
			dump_data(result_file, "GA", ga_result, 1000.0 * ga_running_time, 1,0);
		else
			dump_data(result_file, "GA", ga_result, 1000.0 * ga_running_time,1.0 * ga_result / correct_answer,0 );
	}
	if (flags & TASK_DPm)
	{
		DPm_result = dpm_solve();
		dump_data(result_file, "DPm", DPm_result, 1.0 * dpm_running_time,1,0);
	}


	if (flags & TASK_SINGLE_DP && current_config.m == 1)
	{
		single_dp_result = dp_p1_single_dp(current_config.m - 1, job_set, current_config.n);
		dump_data(result_file, "SINGLE_DP", single_dp_result, 1000.0 * 0,1,0);
	}
	
	//if ( bb_result != correct_answer && bb_result != RUN_TIME_OUT && pdp_result != RUN_TIME_OUT)
	//{
	//	printf("BB_error!\n");
	//	result_error = 1;
	//	getchar();
	//}
	
}

void thread_func(FILE* result_file, int flags)
{
	mtx.lock();
	double tabu_spt_minp_result = -1, ga_result = -1, DPm_result = -1, pdp_result = -1, bb_result = -1, random_result = -1, single_dp_result = -1,final_result = -1;

	if (flags & TASK_BB_DP)
	{
		bb_result = bblb_search(true);
		final_result = bb_result;
		dump_data(result_file, "BB_DP", bb_result, bb_running_time, 1, bblb_nodes_count);//second
	}

	if (flags & TASK_PDP)
	{
		pdp_result = pdp_solve();
		final_result = pdp_result;
		dump_data(result_file, "PDP", pdp_result, 1000.0 * pdp_running_time, 1, 0);
	}

	if (flags & TASK_TABU_RSPT_MINP)
	{
		TabuOptimizer t1(false, 50, 100, 50);
		tabu_spt_minp_result = evalcost(t1.minimize(SPT_MINP_RULE));

		dump_data(result_file, "TABU", tabu_spt_minp_result, 1000.0 * t1.get_ts_running_time(), 1.0 * tabu_spt_minp_result / final_result,0);
	}
	if (flags & TASK_GA)
	{
		ga_result = ga_solve(TRUE, 200, 0.95, 0.2);
		dump_data(result_file, "GA", ga_result, 1000.0 * ga_running_time, 1.0 * ga_result / pdp_result, 0);
	}
	if (flags & TASK_DPm)
	{
		DPm_result = dpm_solve();
		dump_data(result_file, "DPm", DPm_result, 1000.0 * dpm_running_time, 1, 0);
	}



	if (flags & TASK_SINGLE_DP && current_config.m == 1)
	{
		single_dp_result = dp_p1_single_dp(current_config.m - 1, job_set, current_config.n);
		dump_data(result_file, "SINGLE_DP", single_dp_result, 1000.0 * 0, 1, 0);
	}
	if (bb_result != pdp_result)
	{
		result_error = 1;
	}
	mtx.unlock();
}


void run_algorithm_with_timeout(FILE* result_file, int seconds, int flags)
{
	thread t(thread_func, result_file, flags);
	t.detach();
	if (!mtx.try_lock_for(std::chrono::seconds(seconds)))
	{
		printf("thread timeout\n");
		mtx.lock();
	}
	mtx.unlock();

}


void dump_table_header(FILE* fp)
{
	//fprintf(fp, "M, N, B, Name, Time, OPT_Rate, Nodes,Timeout\n");
	fprintf(fp, "M, N, B, Name, Result, Time, OPT_Rate, Nodes,Timeout\n");
	fflush(fp);
}
void start_algorithm(const char* data_filename, const char* output_filename, int algorithms, int timeout_seconds)
{
	FILE* result_file;
	errno_t err = fopen_s(&result_file, output_filename, "w");
	if (err != 0)
	{
		printf("[DATAREADER]Cannot open %s\n", output_filename);
		return;
	}
	dump_table_header(result_file);
	DataReader reader(data_filename);
	double result, ga_result;
	while (reader.has_next())
	{
		vector<Job> cj = reader.next_case();

		run_algorithm(result_file, algorithms);
		//run_algorithm_with_timeout(result_file, timeout_seconds, algorithms);
		if (result_error) {
			printf("result error found while running at instance %0d\n", reader.get_current_case());
			getchar();
			break;
		}
	}
}

int main(int argc, char* argv[])
{
	char data_filename[256] = "../testdata.txt";
	char output_filename[256] = "../Rm_result.csv";
	//TASK_TABU_RSPT_MINP | TASK_GA | TASK_PDP | TASK_DPm | TASK_RAMDOM | TASK_BB_DP
	start_algorithm(data_filename, output_filename, TASK_TABU_RSPT_MINP | TASK_GA , 300);
	free_dp_pn_table();
	system("pause");
	return 0;
}