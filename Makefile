# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/27 13:30:26 by ajemraou          #+#    #+#              #
#    Updated: 2023/06/21 16:29:20 by ajemraou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


CC = c++

CFLAGE = -Wall -Werror -Wextra -std=c++98

SRC =	main.cpp \
		parsing.cpp \
		server.cpp \
		socket.cpp \
		client.cpp \
		request.cpp \
		response.cpp \
		user_data.cpp
	
NAME = webserv

OBJ = ${SRC:.cpp=.o}

%.o : %.cpp
	$(CC) $(CFLAGE) -c $< -o $@

all : $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(CFLAGE) $(OBJ) -o $(NAME)

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f $(NAME)

re : fclean all
