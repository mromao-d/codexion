#include "../lib/codexion.h"

static int	print_error(char *msg)
{
	write(2, msg, strlen(msg));
	return (1);
}

static void	join_coders(t_props *props, int count)
{
	int	i;

	i = -1;
	while (++i < count)
		pthread_join(props->coders[i].thread, NULL);
}

static int	run_codexion(t_props *props)
{
	int	created;
	int	failed;

	props->start_time = get_time_ms();
	created = -1;
	while (++created < props->number_of_coders)
		props->coders[created].last_compile_start = props->start_time;
	created = 0;
	while (created < props->number_of_coders
		&& !pthread_create(&props->coders[created].thread, NULL,
			coder_routine, &props->coders[created]))
		created++;
	failed = (created != props->number_of_coders);
	if (!failed && pthread_create(&props->monitor, NULL, monitor_routine,
			props))
		failed = 1;
	else if (!failed)
		pthread_join(props->monitor, NULL);
	if (failed)
		stop_simulation(props, NULL);
	join_coders(props, created);
	return (failed);
}

int	main(int argc, char **argv)
{
	t_props	props;
	int		ret;

	if (argc != 9)
		return (print_error("Error: expected 8 arguments\n"));
	if (parse_args(&props, argv))
		return (print_error("Error: invalid arguments\n"));
	if (init_all(&props))
		return (print_error("Error: initialization failed\n"));
	ret = 0;
	if (props.number_of_compiles_required > 0)
		ret = run_codexion(&props);
	free_all(&props);
	if (ret)
		print_error("Error: thread creation failed\n");
	return (ret);
}
