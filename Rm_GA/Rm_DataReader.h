#pragma once


#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Rm_job.h"


using namespace std;



class DataReader
{
public:
	DataReader(const char* filename);
	virtual ~DataReader();
	vector<Job> next_case();
	bool has_next();
	static bool to_array(Job* jobArray, int max_size, vector<Job> jobs);
	Configure get_current_configure();
	int get_current_case();

	vector<vector<Job> > data;
	
protected:

private:
	int current_case;
	vector<Configure> configs;
};

DataReader::DataReader(const char* filename)
{
	data.clear();
	configs.clear();
	FILE *fp;
	errno_t err = fopen_s(&fp,filename, "r");
	if (err!=0)
	{
		printf("[DATAREADER]Cannot open %s\n", filename);
		return;
	}
	int c, n, m;
	double b;
	while (!feof(fp)) 
	{
		if (fscanf_s(fp, "R = (cases = %d, n = %d, m = %d, b = %lf)\n", &c, &n, &m, &b) != 4)
			break;
		string casehead = "[";
		for (int i = 1; i <= m; ++i)
		{
			char  pi[5];
			sprintf_s(pi, "p%d,", i);
			casehead.append(pi);
		}
		casehead.append("d] = ");
		for (int c_i = 0; c_i < c; c_i++)
		{
			vector<Job> job_data;
			fscanf_s(fp, casehead.c_str());
			for (int id = 0; id < n; id++)
			{
				Job job;
				fscanf_s(fp, "[");
				for (int im = 0; im < m; ++im)
				{
					fscanf_s(fp, "%d,", &job.proc_time[im]);
				}
				fscanf_s(fp, "%d] ", &job.due_date);
				job_data.push_back(job);

			}
			data.push_back(job_data);
			fscanf_s(fp, "\n");

			Configure config;
			config.b = b;
			config.m = m;
			config.n = n;
			configs.push_back(config);
		}
	}
	current_case = 0;
	printf("%d case(s) loaded\n", data.size());
}

DataReader::~DataReader()
{
	//dtor
}

vector<Job> DataReader::next_case()
{
	if (current_case < data.size())
	{
		vector<Job> current_jobs = data[current_case];
		DataReader::to_array(job_set, MAX_N, current_jobs);

		Configure& config = configs[current_case];
		current_config.m = config.m;
		current_config.n = config.n;
		current_config.b = config.b;
		current_config.caseindex = current_case;
		current_case++;
		return current_jobs;
	}

	return vector<Job>();
}

bool DataReader::has_next()
{
	return current_case < data.size();
}

bool DataReader::to_array(Job* jobArray, int max_size, vector<Job> jobs)
{
	if (jobs.size() > max_size)
	{
		printf("[DATARAEDER] n = %d is too large\n", jobs.size());
		return false;
	}
	for (int i = 0; i < jobs.size(); i++)
	{
		jobArray[i].due_date = jobs[i].due_date;
		memcpy(jobArray[i].proc_time, jobs[i].proc_time, sizeof(jobs[i].proc_time));
	}
	return true;
}

Configure DataReader::get_current_configure()
{
	return configs[current_case - 1];
}

int DataReader::get_current_case()
{
	return current_case;
}
