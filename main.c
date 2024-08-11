/*********************************************************************************
 *      Copyright:  (C) 2024 yanglu
 *                  All rights reserved.
 *
 *       Filename:  main.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(25/07/24)
 *         Author:  yanglu <yanglu@2012174595@qq.com>
 *      ChangeLog:  1, Release initial version on "25/07/24 16:12:52"
 *                 
 ********************************************************************************/


#include "nbiot.h"



int main (int argc, char **argv)
{

	int                  daemon_run = 0;
	pthread_t            report_thread,receive_thread,state_thread;
	pthread_t            leds_process_thread;
	char                *dev = NULL;
	int                  rv;
	nb_config_t          nbiot_data;
	comport_t            comport;
	char                *logfile = "nbiot.log";
	int                  loglevel = LOG_LEVEL_INFO;
	int                  logsize = 10;
	int                  ch=-1;

	struct option       opts[]={
		{"devname",required_argument,NULL,'n'},
		{"temp_interval",required_argument,NULL,'t'},
		{"background",no_argument,NULL,'b'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};

	while((ch=getopt_long(argc,argv,"n:t:bh",opts,NULL))!=-1)
	{
		switch(ch)
		{
			case 'n':
				dev = optarg;
				break;
			case 't':
				g_temp_interval = atoi(optarg);
				break;
			case 'b':
				daemon_run = 1;
				logfile = "console";
				loglevel = LOG_LEVEL_DEBUG;
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
		}
	}

	log_info("Program start running.\n");

	if(!dev || !g_temp_interval)
	{
		print_usage(argv[0]);
		return 0;
	}

	if( daemon_run )
	{
		daemon(0,0);
	}

	if( log_open(logfile, loglevel, logsize, THREAD_LOCK_NONE) < 0 )
	{
		fprintf(stderr, "Initial log system failed\n");
		return 1;
	}


	rv = comport_open(&nbiot_data.comport, dev, 9600, "8n1e");
	if( rv<0 )
	{
		perror("Failed to open comport");
		return -1;
	}
	nbiot_data.current_state = 0;


	if(pthread_create(&state_thread, NULL, nbiot_mgr_thread, &nbiot_data) != 0)
	{
		perror("Failed to create state thread");
		return -2;
	}
	if( pthread_create(&report_thread, NULL, report_data,
				&nbiot_data) != 0){
		perror("Failed to create report thread");
		return -3;
	}

	if ( pthread_create(&receive_thread, NULL, receive_data,
				&nbiot_data) !=0 )
	{
		perror("Failed to create receive thread");
		return -4;
	}

	if ( pthread_create(&leds_process_thread, NULL, asyn_process_leds,
				&nbiot_data) !=0 )
	{   
		perror("Failed to create receive thread");
		return -5; 
	}


	pthread_join(state_thread, NULL);
	pthread_join(report_thread, NULL);
	pthread_join(receive_thread, NULL);
	pthread_join(leds_process_thread, NULL);

	comport_close(&nbiot_data.comport);

	return 0;
} 
