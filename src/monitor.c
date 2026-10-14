#include "../lib/codexion.h"

void	stop_simulation(t_props *props, t_coder *burned)
{
	int	i;

	pthread_mutex_lock(&props->print);
	if (burned && !props->dead)
		printf("%ld %d burned out\n", get_time_ms() - props->start_time,
			burned->id);
	props->dead = 1;
	pthread_mutex_unlock(&props->print);
	i = -1;
	while (++i < props->number_of_coders)
	{
		pthread_mutex_lock(&props->dongles[i].mutex);
		pthread_cond_broadcast(&props->dongles[i].cond);
		pthread_mutex_unlock(&props->dongles[i].mutex);
	}
}

static int	check_coders(t_props *props, t_coder **burned)
{
	t_coder	*coder;
	long	now;
	int		all_done;
	int		i;

	*burned = NULL;
	all_done = 1;
	pthread_mutex_lock(&props->state);
	now = get_time_ms();
	i = -1;
	while (++i < props->number_of_coders)
	{
		coder = &props->coders[i];
		if (!*burned
			&& now - coder->last_compile_start >= props->time_to_burnout)
			*burned = coder;
		if (coder->nb_comp < props->number_of_compiles_required)
			all_done = 0;
	}
	pthread_mutex_unlock(&props->state);
	if (all_done)
		*burned = NULL;
	return (all_done || *burned != NULL);
}

void	*monitor_routine(void *arg)
{
	t_props	*props;
	t_coder	*burned;

	props = (t_props *)arg;
	while (!check_coders(props, &burned))
		usleep(500);
	stop_simulation(props, burned);
	return (NULL);
}
