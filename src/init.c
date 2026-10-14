#include "../lib/codexion.h"

static int	init_dongles(t_props *props)
{
	int	i;

	i = 0;
	while (i < props->number_of_coders)
	{
		if (pthread_mutex_init(&props->dongles[i].mutex, NULL))
			break ;
		if (pthread_cond_init(&props->dongles[i].cond, NULL))
		{
			pthread_mutex_destroy(&props->dongles[i].mutex);
			break ;
		}
		props->dongles[i].is_free = 1;
		props->dongles[i].free_at = 0;
		props->dongles[i].seq = 0;
		props->dongles[i].heap_size = 0;
		i++;
	}
	props->dongles_ready = i;
	return (i != props->number_of_coders);
}

static void	init_coders(t_props *props)
{
	t_dongle	*left;
	t_dongle	*right;
	int			n;
	int			i;

	n = props->number_of_coders;
	i = -1;
	while (++i < n)
	{
		left = &props->dongles[(i - 1 + n) % n];
		right = &props->dongles[i];
		props->coders[i].id = i + 1;
		props->coders[i].nb_comp = 0;
		props->coders[i].last_compile_start = 0;
		props->coders[i].props = props;
		props->coders[i].first = right;
		props->coders[i].second = left;
		if ((i + 1) % 2 == 0)
		{
			props->coders[i].first = left;
			props->coders[i].second = right;
		}
	}
}

static int	init_locks(t_props *props)
{
	if (pthread_mutex_init(&props->print, NULL))
		return (1);
	if (pthread_mutex_init(&props->state, NULL))
	{
		pthread_mutex_destroy(&props->print);
		return (1);
	}
	return (0);
}

void	free_all(t_props *props)
{
	int	i;

	i = -1;
	while (++i < props->dongles_ready)
	{
		pthread_mutex_destroy(&props->dongles[i].mutex);
		pthread_cond_destroy(&props->dongles[i].cond);
	}
	free(props->coders);
	free(props->dongles);
	pthread_mutex_destroy(&props->print);
	pthread_mutex_destroy(&props->state);
}

int	init_all(t_props *props)
{
	props->dead = 0;
	props->dongles_ready = 0;
	if (init_locks(props))
		return (1);
	props->coders = malloc(sizeof(t_coder) * props->number_of_coders);
	props->dongles = malloc(sizeof(t_dongle) * props->number_of_coders);
	if (!props->coders || !props->dongles || init_dongles(props))
	{
		free_all(props);
		return (1);
	}
	init_coders(props);
	return (0);
}
