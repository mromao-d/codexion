#include "../lib/codexion.h"

static void	compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->props->state);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->props->state);
	log_state(coder, "is compiling");
	ms_sleep(coder->props, coder->props->time_to_compile);
	release_dongle(coder, coder->first);
	release_dongle(coder, coder->second);
	pthread_mutex_lock(&coder->props->state);
	coder->nb_comp++;
	pthread_mutex_unlock(&coder->props->state);
}

static void	single_coder(t_coder *coder)
{
	if (!take_dongle(coder, coder->first))
		return ;
	while (!is_stopped(coder->props))
		usleep(500);
	release_dongle(coder, coder->first);
}

static void	work_cycle(t_coder *coder)
{
	if (coder->id % 2 == 0)
		usleep(1000);
	while (!is_stopped(coder->props))
	{
		if (!take_dongle(coder, coder->first))
			break ;
		if (!take_dongle(coder, coder->second))
		{
			release_dongle(coder, coder->first);
			break ;
		}
		compile(coder);
		log_state(coder, "is debugging");
		ms_sleep(coder->props, coder->props->time_to_debug);
		log_state(coder, "is refactoring");
		ms_sleep(coder->props, coder->props->time_to_refactor);
	}
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->props->number_of_coders == 1)
		single_coder(coder);
	else
		work_cycle(coder);
	return (NULL);
}
