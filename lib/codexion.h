#ifndef CODEXION_H
# define CODEXION_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>

typedef struct s_props	t_props;

typedef struct s_req
{
	long	key;
	int		id;
}	t_req;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				is_free;
	long			free_at;
	long			seq;
	t_req			heap[2];
	int				heap_size;
}	t_dongle;

typedef struct s_coder
{
	pthread_t	thread;
	int			id;
	long		last_compile_start;
	int			nb_comp;
	t_dongle	*first;
	t_dongle	*second;
	t_props		*props;
}	t_coder;

typedef struct s_props
{
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;
	int				is_edf;
	int				dead;
	int				dongles_ready;
	long			start_time;
	pthread_t		monitor;
	pthread_mutex_t	print;
	pthread_mutex_t	state;
	t_coder			*coders;
	t_dongle		*dongles;
}	t_props;

int		parse_args(t_props *props, char **argv);

int		init_all(t_props *props);
void	free_all(t_props *props);

void	heap_push(t_dongle *dongle, long key, int id);
void	heap_remove(t_dongle *dongle, int id);

int		take_dongle(t_coder *coder, t_dongle *dongle);
void	release_dongle(t_coder *coder, t_dongle *dongle);

void	*coder_routine(void *arg);

void	*monitor_routine(void *arg);
void	stop_simulation(t_props *props, t_coder *burned);

long	get_time_ms(void);
int		is_stopped(t_props *props);
void	ms_sleep(t_props *props, long ms);
void	log_state(t_coder *coder, char *msg);

#endif
