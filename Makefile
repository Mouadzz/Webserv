# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mlasrite <mlasrite@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/11/19 18:51:44 by mlasrite          #+#    #+#              #
#    Updated: 2021/11/21 12:18:06 by mlasrite         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRC	=	main.cpp \
		server/server.cpp 

FLAGE = -Wall -Wextra -Werror -std=c++98

NAME = Webserv

all : $(NAME)

$(NAME) : $(SRC)
	clang++ $(FLAGE) $(SRC) -o $(NAME)

clean :
	rm -rf $(NAME)
fclean :
	rm -rf $(NAME)

re : clean all