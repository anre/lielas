#ifndef SIGNALS_H
#define SIGNALS_H

	int sig_init();

	int sig_sigint_received();
	int sig_sigusr1_received();
	void sig_reset_sigusr1();

#endif

