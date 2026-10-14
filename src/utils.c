#include "../lib/codexion.h"

long	get_time_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000L + time.tv_usec / 1000);
}

int	is_stopped(t_props *props)
{
	int	dead;

	pthread_mutex_lock(&props->print);
	dead = props->dead;
	pthread_mutex_unlock(&props->print);
	return (dead);
}

void	ms_sleep(t_props *props, long ms)
{
	long	end;

	end = get_time_ms() + ms;
	while (get_time_ms() < end && !is_stopped(props))
		usleep(500);
}

void	log_state(t_coder *coder, char *msg)
{
	t_props	*props;

	props = coder->props;
	pthread_mutex_lock(&props->print);
	if (!props->dead)
		printf("%ld %d %s\n", get_time_ms() - props->start_time,
			coder->id, msg);
	pthread_mutex_unlock(&props->print);
}
