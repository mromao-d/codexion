NAME	= codexion

CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread

SRC_DIR	= src
SRC		= $(SRC_DIR)/codexion.c \
		  $(SRC_DIR)/atoi.c \
		  $(SRC_DIR)/utils.c \
		  $(SRC_DIR)/init_props.c \
		  $(SRC_DIR)/init_coders.c \
		  $(SRC_DIR)/init_dongles.c \
		  $(SRC_DIR)/coders.c \
		  $(SRC_DIR)/queue.c

OBJ		= $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
