/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsq.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjover-a <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/25 09:20:53 by jjover-a          #+#    #+#             */
/*   Updated: 2023/07/26 13:03:13 by jjover-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct
{
	int		size;
	int		x;
	int		y;
}			t_square;
typedef struct s_map_info
{
	int		width;
	int		height;
	char	empty;
	char	obstacle;
	char	full;
}			t_map_info;

typedef struct s_point
{
	int		x;
	int		y;
}			t_point;

t_map_info	get_map_info(char *first_line);
char		**read_map(int fd, t_map_info *map_info);
t_square	find_biggest_square(char **map, t_map_info *map_info);
void		fill_square(char **map, t_map_info *map_info, t_square max_square);
void		print_map(char **map, t_map_info *map_info);
void		free_map(char **map, t_map_info *map_info);
void		handle_error(void);

int	main(int argc, char **argv)
{
	int			fd;
	t_map_info	map_info;
	char		**map;
	t_square	max_square;
	int z;

	if (argc < 2)
		handle_error();
	z = 1;
	while(z <= argc)
	{

	fd = open(argv[z], O_RDONLY);
	if (fd == -1)
		handle_error();
	char first_line[13];
	if (read(fd, first_line, 12) <= 0)
		handle_error();
	map_info = get_map_info(first_line);
	map = read_map(fd, &map_info);
	max_square = find_biggest_square(map, &map_info);
	fill_square(map, &map_info, max_square);
	print_map(map, &map_info);
	free_map(map, &map_info);
	if (close(fd) == -1)
		handle_error();
	z++;
	}
	return (0);
}

t_map_info	get_map_info(char *first_line)
{
	t_map_info	map_info;
	int			i;
	char		height[i + 1];

	i = 0;
	
	while (first_line[i] != '\n' && first_line[i] != '.')
		i++;
	int j = 0;
	while (j < i)
	{
		height[j] = first_line[j];
		j++;
	}
	height[i] = '\0';
	map_info.height = atoi(height);
	
	if (first_line[i] == '.' && first_line[i + 1] != '\n')
	{
		map_info.empty = first_line[i];
		map_info.width = map_info.height;
		if (first_line[i + 2] != '\n')
		{
			map_info.obstacle = first_line[i + 1];
			map_info.full = first_line[i + 2];
		}
		else
		{
			// handle error as obstacle or full char is missing
			handle_error();
		}
	}
	else
	{
		// handle error as empty char is missing
		handle_error();
	}
	return (map_info);
}

char	**read_map(int fd, t_map_info *map_info)
{
	char	**map;
	int		bytes_read;
	char	buffer;

	int i, j;
	map = (char **)malloc(map_info->height * sizeof(char *));
	if (map == NULL)
	{
		handle_error();
	}
	i = 0;
	while ( i < map_info->height)
	{
		map[i] = (char *)malloc(1000 * sizeof(char));
		
		if (map[i] == NULL)
		{
			handle_error();
		}
		j = 0;
		while ((bytes_read = read(fd, &buffer, 1)) > 0 && buffer != '\n')
		{
			map[i][j] = buffer;
			j++;
		}
		if (bytes_read == -1)
		{
			handle_error();
		}
		map_info->width = j;
			if (j != map_info->width)
		{
			handle_error();
		}
		map[i][j] = '\0';
		i++;
	}
	return (map);
}

t_square	find_biggest_square(char **map, t_map_info *map_info)
{
	int			**s;
	int			i;
	int			j;
	t_square	max_square;
	int			min;

	i = 0;
	max_square.x = 0;
	max_square.y = 0;
	max_square.size = 0;
	s = (int **)malloc(map_info->height * sizeof(int *));
	while (i < map_info->height)
	{
		s[i] = (int *)malloc(map_info->width * sizeof(int));
		i++;
	}

	i = 0;
	while (i < map_info->height)
	{
		s[i][0] = (map[i][0] == map_info->empty) ? 1 : 0;
		i++;
	}

	j = 0;
	while (j < map_info->width)
	{
		s[0][j] = (map[0][j] == map_info->empty) ? 1 : 0;
		j++;
	}

	i = 1;
	while (i < map_info->height)
	{
		j = 1;
		while (j < map_info->width)
		{
			if (map[i][j] == map_info->empty)
			{
				min = s[i][j - 1];
				if (s[i - 1][j] < min)
					min = s[i - 1][j];
				if (s[i - 1][j - 1] < min)
					min = s[i - 1][j - 1];
				s[i][j] = min + 1;
			}
			else
			{
				s[i][j] = 0;
			}
			if (s[i][j] > max_square.size)
			{
				max_square.size = s[i][j];
				max_square.x = i;
				max_square.y = j;
			}
			j++;
		}
		i++;
	}
	i = 0;
	while (i < map_info->height)
	{
		free(s[i]);
		i++;
	}
	free(s);
	return (max_square);
}

void	fill_square(char **map, t_map_info *map_info, t_square max_square)
{
	int	i;
	int	j;

	i = max_square.x - max_square.size + 1;
	while (i <= max_square.x)
	{
		j = max_square.y - max_square.size + 1;
		while (j <= max_square.y)
		{
			map[i][j] = map_info->full;
			j++;
		}
		i++;
	}
}
void	print_map(char **map, t_map_info *map_info)
{
	int	i;
	int	j;

	i = 0;
	while (i < map_info->height)
	{
		j = 0;
		while (j < map_info->width)
		{
			write(1, &map[i][j], 1);
			j++;
		}
		write(1, "\n", 1); 
		i++;
	}
}
void	free_map(char **map, t_map_info *map_info)
{
	int	i;

	i = 0;
	while (i < map_info->height)
	{
		free(map[i]); 
		i++;
	}
	free(map);
}
void	handle_error(void)
{
	write(2, "map error\n", 10);
	exit(1);
}
