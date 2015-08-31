/*

	ipmsg

 */

#define THREAD_NUM 3

int main(int argc, char *argv[])
{
	pthread_t tid[THREAD_NUM];	
	const char *user;
	const char *host;
	int c;

	// get user,host from command line
	// or read from ipmsg.conf ?
	user = DEFAULT_USER; 
	host = DEFAULT_HOST;
 	while ( (c = getopt(argc, argv, "u:h:")) != -1) {
		switch (c) {
		case 'u':
			user = optarg;		
			break;

		case 'h':
			host = optarg;	
			break;
		}
	}
	if (optind == 1) {
		fprintf(stderr, "Usage: ipmsg  [ -u user ] [ -h host ]\n");
		exit(1);
	}
	printf("user:%s\nhost:%s\n", user, host);

	/* init */
	init(user, host);

	/* create thread  */ 
	pthread_create(&tid[0], NULL, recv_msg_thread, NULL);
	pthread_create(&tid[1], NULL, user_interface , NULL);
	pthread_create(&tid[2], NULL, sendfile_thread, NULL);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	pthread_join(tid[2], NULL);

	return 0;	
}
