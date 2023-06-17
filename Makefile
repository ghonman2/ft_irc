# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tjo <tjo@student.42seoul.kr>               +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/01/25 14:51:43 by joowpark          #+#    #+#              #
#    Updated: 2023/03/06 15:03:12 by joowpark         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME= ircserv
CXX= c++
SHELL = /bin/bash
SRC = CommandManager.o Client.o Server.o main.o Channel.o Bot.o
OFLAGS =
CXXFLAGS += $(OFLAGS) -c
CXXFLAGS += -Wall -Wextra -Werror -std=c++98 
CXXFLAGS += #-I ./include  
FSA = #-fsanitize=address
DBG = -g3
TARGETS = $(NAME)

BONUS_NAME=
ifdef BONUS
	BONUS_NAME=_bonus
endif

  
SPINNER= @$(SHELL) ./.spinner.sh

sp = /-\|/
idx	= 0

%.o : %.cpp
	 @$(CXX) $(CXXFLAGS) $^ -o $@ $(FSA) $(DBG)
	 @printf "$(@) ✅\n"

all : $(TARGETS)$(BONUS_NAME)

$(TARGETS)$(BONUS_NAME): $(SRC)
	@$(CXX) $(FSA) $(DBG) -o $@ $^ 
	@$(SPINNER) $@

.PHONY : bonus

bonus:
	$(MAKE) BONUS=1
	
.PHONY : clean
clean:
	rm -f ${SRC}

.PHONY : file
file :
	@make re
	@make clean

.PHONY : fclean

fclean : clean
	rm -f ${NAME}
	rm -f ${NAME}_bonus

.PHONY : re
re : 
	@make fclean
	@make all
