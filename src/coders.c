#include "../lib/codexion.h"

void	*drop_dongles(void *arg)
{
	t_coders	*coder;

	coder = (t_coders *) arg;
	usleep(coder->props->dongle_cooldown);
	pthread_mutex_lock(&coder->props->scheduler_mutex);
	coder->dongles[0]->is_free = 1;
	coder->dongles[1]->is_free = 1;
	pthread_cond_broadcast(&coder->props->scheduler_cond);
	pthread_mutex_unlock(&coder->props->scheduler_mutex);
	return (NULL);
}

int	take_dongles_one(t_coders *coder)
{
	pthread_mutex_lock(&coder->props->scheduler_mutex);
	while (!coder->props->dead)
		pthread_cond_wait(&coder->props->scheduler_cond, &coder->props->scheduler_mutex);
	pthread_mutex_unlock(&coder->props->scheduler_mutex);
	return (0);
}

int	take_dongles(t_coders *coder)
{
	pthread_mutex_lock(&(coder->props->scheduler_mutex));
	add_coder_queue(coder);
	while (!(coder->props->dead) && (coder->dongles[0]->is_free == 0
			|| coder->dongles[1]->is_free == 0
			|| is_blocked_by_rival(coder)))
	{
		pthread_cond_wait(&coder->props->scheduler_cond,
			&coder->props->scheduler_mutex);
	}
	if (coder->props->dead)
	{
		remove_coder_queue(coder);
		pthread_mutex_unlock(&coder->props->scheduler_mutex);
		return (0);
	}
	coder->dongles[0]->is_free = 0;
	coder->dongles[1]->is_free = 0;
	coder->last_compile_start = get_current_time();
	pthread_mutex_lock(&coder->props->print);
	printf("%ld %d has taken a dongle\n", coder->last_compile_start - coder->props->start_time, coder->coder_id);
	printf("%ld %d has taken a dongle\n", coder->last_compile_start - coder->props->start_time, coder->coder_id);
	printf("%ld %d is compiling\n", coder->last_compile_start - coder->props->start_time, coder->coder_id);
	pthread_mutex_unlock(&coder->props->print);
	remove_coder_queue(coder);
	coder->nb_comp++;
	pthread_mutex_unlock(&coder->props->scheduler_mutex);
	return (1);
}

static void	run_compile_cycle(t_coders *coder)
{
	usleep(coder->props->time_to_compile);
	pthread_create(&coder->cool_down, NULL, drop_dongles, coder);
	pthread_mutex_lock(&coder->props->print);
	if (!coder->props->dead)
		printf("%ld %d is debugging\n", get_current_time() - coder->props->start_time, coder->coder_id);
	pthread_mutex_unlock(&coder->props->print);
	usleep(coder->props->time_to_debug);
	pthread_mutex_lock(&coder->props->print);
	if (!coder->props->dead)
		printf("%ld %d is refactoring\n", get_current_time() - coder->props->start_time, coder->coder_id);
	pthread_mutex_unlock(&coder->props->print);
	usleep(coder->props->time_to_refactor);
	pthread_join(coder->cool_down, NULL);
}

void	*coder_routine(void *arg)
{
	t_coders	*coder;

	coder = (t_coders *)arg;
	pthread_mutex_lock(&coder->props->scheduler_mutex);
	while (!coder->props->start)
		pthread_cond_wait(&coder->props->scheduler_cond, &coder->props->scheduler_mutex);
	pthread_mutex_unlock(&coder->props->scheduler_mutex);
	while (!(coder->props->dead) && coder->nb_comp < coder->props->number_of_compiles_required)
	{
		if (coder->props->number_of_coders == 1)
		{
			if (!take_dongles_one(coder))
				break ;
		}
		else
		{
			if (!take_dongles(coder))
				break ;
			run_compile_cycle(coder);
		}
	}
	return (NULL);
}

void	assign_dongles(t_props *props)
{
	int	i;

	i = 0;
	props->coders[0].dongles[0] = &props->dongles[props->number_of_coders - 1];
	if (props->number_of_coders == 1)
		return ;
	props->coders[0].dongles[1] = &props->dongles[0];
	while (++i < props->number_of_coders)
	{
		props->coders[i].dongles[1] = &props->dongles[i];
		props->coders[i].dongles[0] = &props->dongles[i - 1];
	}
	(*props).start_time = get_current_time();
	return ;
}
