#include "../lib/codexion.h"

void    *monitor_routine(void *arg)
{
    t_props *props;
    int     i;
    int     all_done;
    time_t  now;
    time_t  deadline;

    props = (t_props *)arg;
    while (1)
    {
        usleep(1000);
        pthread_mutex_lock(&props->scheduler_mutex);
        if (props->dead)
        {
            pthread_mutex_unlock(&props->scheduler_mutex);
            return (NULL);
        }
        now = get_current_time();
        all_done = 1;
        i = -1;
        while (++i < props->number_of_coders)
        {
            if (props->coders[i].nb_comp < props->number_of_compiles_required)
                all_done = 0;
            deadline = props->coders[i].last_compile_start
                       + props->time_to_burnout / 1000;
            if (now >= deadline)
            {
                pthread_mutex_lock(&props->print);
                printf("%ld %d burned out\n",
                    now - props->start_time, props->coders[i].coder_id);
                pthread_mutex_unlock(&props->print);
                props->dead = 1;
                pthread_cond_broadcast(&props->scheduler_cond);
                pthread_mutex_unlock(&props->scheduler_mutex);
                return (NULL);
            }
        }
        if (all_done)
        {
            props->dead = 1;
            pthread_cond_broadcast(&props->scheduler_cond);
            pthread_mutex_unlock(&props->scheduler_mutex);
            return (NULL);
        }
        pthread_mutex_unlock(&props->scheduler_mutex);
    }
    return (NULL);
}


int ft_print(char *str)
{
	while (*str)
	{
		write(1, &(*str), 1);
		str++;
	}
	return (1);
}

void	ft_print_args(t_props **props) {
	printf("Number of Coders is: %i\n", (*props)->number_of_coders);
	for (int i = 0; i < (*props)->number_of_coders; i++)
		printf("This is Coder %i\n", (*props)->coders[i].coder_id);
	return ;
}

int	ft_free_all(t_props	*props)
{
	int	i;

	i = -1;
	while (++i < props->number_of_coders)
	{
		pthread_mutex_destroy(&props->dongles[i].dongle);
		free(props->coders[i].dongles);
	}
	free(props->coders);
	free(props->dongles);
	free(props->queue);
	pthread_mutex_destroy(&props->print);
	pthread_mutex_destroy(&props->scheduler_mutex);
	pthread_cond_destroy(&props->scheduler_cond);
	free(props);
	return (0);
}

void	ft_print_coders_dongles(t_props *props) {
	for (int i = 0; i < props->number_of_coders; i++) {
		printf("coder %i dongles are: %i, %i",props->coders[i].coder_id, props->coders[i].dongles[0]->dongle_id, props->coders[i].dongles[1]->dongle_id);
		printf("\n");
	}
	return ;
}

void	run_codexion(t_props *props)
{
	int	i;

	map_dongles(props);
	map_coders(props);
	i = -1;
	while (++i < props->number_of_coders)
		pthread_create(&props->coders[i].coder, NULL, coder_routine, &props->coders[i]);
	pthread_mutex_lock(&props->scheduler_mutex);
	props->start_time = get_current_time();
	i = -1;
	while (++i < props->number_of_coders)
		props->coders[i].last_compile_start = props->start_time;
	props->start = 1;
	pthread_cond_broadcast(&props->scheduler_cond);
	pthread_mutex_unlock(&props->scheduler_mutex);
	pthread_create(&props->monitor, NULL, monitor_routine, props);
	join_coders(props);
	pthread_join(props->monitor, NULL);
}

int main(int argc, char *argv[])
{
	t_props	*props;

	if (argc != 9)
		return (ft_print("Wrong number of args\n"));
	props = map_args(argv);
	if (validate_args(&props))
	{
		ft_print("Wrong args\n");
		free(props);
		return (1);
	}
	run_codexion(props);
	// ft_print_coders_dongles(props);
	return (ft_free_all(props));
}
