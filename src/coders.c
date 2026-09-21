#include "../lib/codexion.h"

void	*drop_dongles(void *arg)
{
	t_coders	*coder;

	coder = (t_coders *) arg;
	usleep(coder->props->dongle_cooldown);
	pthread_mutex_lock(&coder->props->scheduler_mutex);
	pthread_mutex_unlock(&(coder->dongles[0]->dongle));
	coder->dongles[0]->is_free = 1;
	pthread_mutex_unlock(&(coder->dongles[1]->dongle));
	coder->dongles[1]->is_free = 1;
	pthread_cond_broadcast(&coder->props->scheduler_cond);
	pthread_mutex_unlock(&coder->props->scheduler_mutex);
	return NULL;
}

int	take_dongles_one(t_coders	*coder)
{
	time_t			deadline;
	struct timespec	timeout;

	pthread_mutex_lock(&(coder->props->scheduler_mutex));
	add_coder_queue(coder);
	deadline = get_current_time() + coder->props->time_to_burnout / 1000;
	timeout.tv_sec = deadline / 1000;
	timeout.tv_nsec = (deadline % 1000) * 1000000;
	pthread_mutex_lock(&coder->dongles[0]->dongle);
	coder->dongles[0]->is_free = 0;
	printf("%li %i has taken a dongle\n", get_current_time() - coder->props->start_time, coder->coder_id);
	while (!(coder->props->dead) && (coder->coder_id != coder->props->queue[0]
			 || coder->dongles[0]->is_free == 0))
	{
		if (pthread_cond_timedwait(&coder->props->scheduler_cond,
						&coder->props->scheduler_mutex, &timeout) == ETIMEDOUT)
		{
			pthread_mutex_lock(&coder->props->print);
			if (!coder->props->dead)
				printf("%li %i burned out\n", get_current_time() - coder->props->start_time, coder->coder_id);
			coder->props->dead = 1;
			pthread_mutex_unlock(&coder->props->print);
			pthread_cond_broadcast(&coder->props->scheduler_cond);
			pthread_mutex_unlock(&coder->props->scheduler_mutex);
			return (0);
		}
	}
	return (0);
}

int	take_dongles(t_coders	*coder)
{
	time_t			deadline;
	struct timespec	timeout;

	pthread_mutex_lock(&(coder->props->scheduler_mutex));
	add_coder_queue(coder);
	deadline = get_current_time() + coder->props->time_to_burnout / 1000;
	timeout.tv_sec = deadline / 1000;
	timeout.tv_nsec = (deadline % 1000) * 1000000;
	while (!(coder->props->dead) && (coder->coder_id != coder->props->queue[0]
			 || coder->dongles[0]->is_free == 0
			 || coder->dongles[1]->is_free == 0))
	{
		if (pthread_cond_timedwait(&coder->props->scheduler_cond,
						&coder->props->scheduler_mutex, &timeout) == ETIMEDOUT)
		{
			pthread_mutex_lock(&coder->props->print);
			if (!coder->props->dead)
				printf("%li %i burned out\n", get_current_time() - coder->props->start_time, coder->coder_id);
			coder->props->dead = 1;
			pthread_mutex_unlock(&coder->props->print);
			pthread_cond_broadcast(&coder->props->scheduler_cond);
			pthread_mutex_unlock(&coder->props->scheduler_mutex);
			return (0);
		}
	}
	if (coder->props->dead)
	{
		pthread_mutex_unlock(&coder->props->scheduler_mutex);
		return (0);
	}
	pthread_mutex_lock(&coder->dongles[0]->dongle);
	coder->dongles[0]->is_free = 0;
	pthread_mutex_lock(&coder->dongles[1]->dongle);
	coder->dongles[1]->is_free = 0;
	pthread_mutex_lock(&coder->props->print);
	printf("%li %i has taken a dongle\n", get_current_time() - coder->props->start_time, coder->coder_id);
	printf("%li %i has taken a dongle\n", get_current_time() - coder->props->start_time, coder->coder_id);
	printf("%li %i is compiling\n", get_current_time() - coder->props->start_time, coder->coder_id);
	pthread_mutex_unlock(&coder->props->print);
	remove_coder_queue(coder);
	coder->nb_comp++;
	pthread_mutex_unlock(&coder->props->scheduler_mutex);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coders    *coder;

	coder = (t_coders *)arg;
	while(!(coder->props->dead) && coder->nb_comp < coder->props->number_of_compiles_required)
	{
		if (coder->props->number_of_coders == 1)
		{
    		if (!take_dongles_one(coder))
				break;
		}
		else
		{
			if (!take_dongles(coder))
				break;
			usleep(coder->props->time_to_compile);
			pthread_create(&coder->cool_down, NULL, drop_dongles, coder);

			pthread_mutex_lock(&(coder->props->print));
			if (!coder->props->dead)
				printf("%li %i is debugging\n", get_current_time() - coder->props->start_time, coder->coder_id);
			pthread_mutex_unlock(&(coder->props->print));
			usleep(coder->props->time_to_debug);

			pthread_mutex_lock(&(coder->props->print));
			if (!coder->props->dead)
				printf("%li %i is refactoring\n", get_current_time() - coder->props->start_time, coder->coder_id);
			pthread_mutex_unlock(&(coder->props->print));
			usleep(coder->props->time_to_refactor);

			pthread_join(coder->cool_down, NULL);
		}
	}
	return NULL;
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
