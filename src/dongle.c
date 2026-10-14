#include "../lib/codexion.h"

static int	can_take(t_dongle *dongle, int id)
{
	if (!dongle->is_free || dongle->heap[0].id != id)
		return (0);
	return (get_time_ms() >= dongle->free_at);
}

static void	wait_dongle(t_dongle *dongle, int id)
{
	struct timespec	until;

	if (dongle->is_free && dongle->heap[0].id == id)
	{
		until.tv_sec = dongle->free_at / 1000;
		until.tv_nsec = (dongle->free_at % 1000) * 1000000;
		pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &until);
	}
	else
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
}

int	take_dongle(t_coder *coder, t_dongle *dongle)
{
	long	key;

	pthread_mutex_lock(&dongle->mutex);
	key = dongle->seq++;
	if (coder->props->is_edf)
		key = coder->last_compile_start + coder->props->time_to_burnout;
	heap_push(dongle, key, coder->id);
	while (!is_stopped(coder->props) && !can_take(dongle, coder->id))
		wait_dongle(dongle, coder->id);
	heap_remove(dongle, coder->id);
	if (is_stopped(coder->props))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	dongle->is_free = 0;
	pthread_mutex_unlock(&dongle->mutex);
	log_state(coder, "has taken a dongle");
	return (1);
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_free = 1;
	dongle->free_at = get_time_ms() + coder->props->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
