# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/27 13:30:26 by ajemraou          #+#    #+#              #
#    Updated: 2023/06/07 15:26:08 by ajemraou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


CC = g++ 

CFLAGE = -Wall -Werror -Wextra

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

r : re
	clear ; ./webserv config.conf
