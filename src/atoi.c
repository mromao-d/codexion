#include "../lib/codexion.h"

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

int	ft_atoi(const char *nptr)
{
	int	i;
	int	sign;
	int	out;

	i = 0;
	out = 0;
	sign = 1;
	while (nptr[i] == ' ' || nptr[i] == '\t' || nptr[i] == '\n' 
        || nptr[i] == '\v' || nptr[i] == '\f' || nptr[i] == '\r')
		i++;
	if (nptr[i] == '-')
		return (-1);
	else if (nptr[i] == '+')
		i++;	
	while (ft_isdigit(nptr[i]))
		out = out * 10 + nptr[i++] - 48;
    if (nptr[i] && !ft_isdigit(nptr[i]))
        return (-1);
	return (out);
}