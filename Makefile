# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/27 13:30:26 by ajemraou          #+#    #+#              #
#    Updated: 2023/05/28 16:15:36 by ajemraou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


CC = g++

CFLAGE = 

SRC =	main.cpp \
		client.cpp \
		server.cpp \
		socket.cpp \
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