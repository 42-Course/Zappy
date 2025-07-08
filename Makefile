# Makefile for Zappy Server
NAME = Zappy

.RECIPEPREFIX = >

# Core components
SRC =  srcs/main.cpp \
       srcs/core/utils.cpp

SRC += srcs/core/Engine.cpp \
       srcs/core/World.cpp \
       srcs/core/GameLoop.cpp \
       srcs/core/Map.cpp \
       srcs/core/Team.cpp \
       srcs/core/Player.cpp \
       srcs/core/Spectator.cpp \
       srcs/core/Tile.cpp \
       srcs/core/Resource.cpp \
       srcs/core/Inventory.cpp \
       srcs/core/Subject.cpp

# Network components
SRC += srcs/net/NetworkManager.cpp \
       srcs/net/ClientConnection.cpp \
       srcs/net/Socket.cpp \
       srcs/net/EventLoop.cpp \
       srcs/net/ClientManager.cpp 

# Command System
SRC += srcs/commands/Command.cpp \
       srcs/commands/CommandFactory.cpp \
       srcs/commands/CommandDispatcher.cpp \
       srcs/commands/CommandHandler.cpp

# Spectator Commands
SRC += srcs/commands/spectator/MSZCommand.cpp \
       srcs/commands/spectator/BCTCommand.cpp \
       srcs/commands/spectator/MCTCommand.cpp \
       srcs/commands/spectator/TNACommand.cpp \
       srcs/commands/spectator/PPOCommand.cpp \
       srcs/commands/spectator/PLVCommand.cpp \
       srcs/commands/spectator/PINCommand.cpp \
       srcs/commands/spectator/SGTCommand.cpp \
       srcs/commands/spectator/SSTCommand.cpp

# Player Commands
SRC += srcs/commands/player/ForwardCommand.cpp \
       srcs/commands/player/RightCommand.cpp \
       srcs/commands/player/LeftCommand.cpp \
       srcs/commands/player/InventoryCommand.cpp \
       srcs/commands/player/SeeCommand.cpp \
       srcs/commands/player/TakeCommand.cpp

# STDIN Commands
SRC += srcs/commands/server/ExitCommand.cpp \
       srcs/commands/server/StatusCommand.cpp \
       srcs/commands/server/HelpCommand.cpp \
       srcs/commands/server/WatchCommand.cpp \
       srcs/commands/server/ClearCommand.cpp

# Services
SRC += srcs/services/WatchService.cpp \
       srcs/services/Logger.cpp \
       srcs/services/SpectatorBroadcaster.cpp

CFLAGS = -Wall -Wextra -Werror -std=c++17 -g -O3 # -fsanitize=leak,address
#CFLAGS = -O3 -g -Wall -Wextra -Werror -std=c++20 -pedantic -fsanitize=leak

INC = -I includes

# Object directories structure
OBJS_D = objs
OBJS_DIRS = ${OBJS_D}/core \
            ${OBJS_D}/net \
            ${OBJS_D}/services \
            ${OBJS_D}/commands \
            ${OBJS_D}/commands/server \
            ${OBJS_D}/commands/spectator \
            ${OBJS_D}/commands/player

DEBUG_OBJS_D = debug_objs
DEBUG_OBJS_DIRS = ${DEBUG_OBJS_D}/core \
                  ${DEBUG_OBJS_D}/net \
                  ${DEBUG_OBJS_D}/services \
                  ${DEBUG_OBJS_D}/commands \
                  ${DEBUG_OBJS_D}/commands/server \
                  ${DEBUG_OBJS_D}/commands/spectator \
                  ${DEBUG_OBJS_D}/commands/player

OBJ = $(SRC:srcs/%.cpp=$(OBJS_D)/%.o)
DEBUG_OBJ = $(SRC:srcs/%.cpp=$(DEBUG_OBJS_D)/%.o)

UNAME := $(shell uname)
RM = rm -rf

# Compiler selection based on platform
ifeq ($(UNAME), Darwin)
  CC = g++
else ifeq ($(UNAME), FreeBSD)
  CC = clang++
else
  CC = g++
  CFLAGS += -D LINUX
endif

# Main target
$(NAME): $(OBJ)
> @printf "Compiling $(C_YELLOW)$(NAME)$(C_END) ... \n"
> $(CC) $(CFLAGS) $(OBJ) -o $(NAME)
> @printf "$(C_GREEN)DONE$(C_END)\n"

# Object compilation rules
$(OBJS_D)/%.o: srcs/%.cpp
> @mkdir -p $(OBJS_D) ${OBJS_DIRS}
> $(CC) $(CFLAGS) $(INC) -o $@ -c $<

$(DEBUG_OBJS_D)/%.o: srcs/%.cpp
> @mkdir -p $(DEBUG_OBJS_D) ${DEBUG_OBJS_DIRS}
> $(CC) $(CFLAGS) -D DEBUG=1 $(INC) -o $@ -c $<

# Utility targets
test: re
> @printf "$(C_BLUE)Testing $(C_YELLOW)$(NAME)$(C_END)\n"
> @printf "\n$(C_BLUE)********************************************$(C_END)\n"
> @./$(NAME)
> @printf "\n$(C_BLUE)********************************************$(C_END)\n"
> @printf "\n$(C_BLUE)Finished Test...$(C_END)\n"
> @$(MAKE) show
> @$(MAKE) fclean

all: $(NAME)

debug: $(DEBUG_OBJ)
> @printf "Compiling DEBUG $(C_YELLOW)debug_$(NAME)$(C_END) ...\n"
> $(CC) $(CFLAGS) $(DEBUG_OBJ) -o debug_$(NAME)
> @printf "$(C_GREEN)DONE$(C_END)\n"

clean:
> $(RM) $(OBJS_D) $(DEBUG_OBJS_D)
> @printf "$(C_RED)Cleaning objs$(C_END)\n"

fclean: clean
> $(RM) $(NAME) debug_$(NAME)
> @printf "$(C_RED)Deleted Everything$(C_END)\n"

re: fclean all

show:
> @printf "$(C_GREEN)"
> @printf "UNAME    : $(UNAME)\n"
> @printf "NAME     : $(C_YELLOW)$(NAME)$(C_GREEN)\n"
> @printf "CC       : $(CC)\n"
> @printf "CFLAGS   : $(CFLAGS)\n"
> @printf "INCLUDES : $(INC)\n"
> @printf "SRC      : $(C_YELLOW)$(SRC)$(C_GREEN)\n"
> @printf "OBJ      : $(C_YELLOW)[$(OBJS_D)] --> $(OBJ)$(C_END)\n"

.PHONY: all test re clean fclean show debug

# Color definitions
C_BLACK = \033[0;30m
C_RED = \033[0;31m
C_GREEN = \033[0;32m
C_YELLOW = \033[0;33m
C_BLUE = \033[0;34m
C_WHITE = \033[0;37m
C_END = \033[0m
