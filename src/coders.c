#include "../lib/codexion.h"

void	take_dongles(t_coders   *coder)
{
	if (coder->dongles[0].is_free && coder->dongles[1].is_free)
	{
		pthread_mutex_lock(&(coder->dongles[0].dongle));
		coder->dongles[0].is_free = 0;
		printf("%li %i has taken a dongle\n", get_current_time() - coder->start_time, coder->coder_id);

		pthread_mutex_lock(&(coder->dongles[1].dongle));
		coder->dongles[1].is_free = 0;
		printf("%li %i has taken a dongle\n", get_current_time() - coder->start_time, coder->coder_id);
	}
	return ;
}

void	*coder_routine(void *arg)
{
	t_coders    *coder;

	coder = (t_coders *)arg;
	take_dongles(coder);
	// printf("Coder %i is working ", coder->coder_id);
	// printf("with dongles %i and %i\n", coder->dongles[0].dongle_id, coder->dongles[1].dongle_id);
	return NULL;
}

void	assign_dongles(t_props *props)
{
	int	i;

	i = 0;
	props->coders[0].dongles[0] = props->dongles[props->number_of_coders - 1];
	if (props->number_of_coders == 1)
		return ;
	props->coders[0].dongles[1] = props->dongles[0];
	while (++i < props->number_of_coders)
	{
		props->coders[i].dongles[1] = props->dongles[i];
		props->coders[i].dongles[0] = props->dongles[i - 1];
	}
	return ;
}

void	map_coders(t_props *props)
{
	int	i;

	i = 0;
	while (i < props->number_of_coders)
	{
		if (props->number_of_coders == 1)
			props->coders[i].dongles = malloc(sizeof(t_dongles));
		else
			props->coders[i].dongles = malloc(sizeof(t_dongles) * 2);
		props->coders[i].coder_id = i;
		props->coders[i].start_time = props->start_time;
		i++;
	}
	assign_dongles(props);
	i = -1;
	while (++i < props->number_of_coders)
		pthread_create(&props->coders[i].coder, NULL, coder_routine, &props->coders[i]);
	join_coders(props);
	return ;
}

void join_coders(t_props *props)
{
	int i;

	i = 0;
	while (i < props->number_of_coders)
	{
		pthread_join(props->coders[i].coder, NULL);
		i++;
	}
}
