##
## EPITECH PROJECT, 2026
## C_paint
## File description:
## Makefile for C-Paint Professional Edition
##

# ═══════════════════════════════════════════════════════════════════════════
#                             PROJECT CONFIGURATION                            
# ═══════════════════════════════════════════════════════════════════════════

NAME        = my_paint

# Detect Apple Silicon (M1/M2/M3) for Homebrew paths
ARCH		=	$(shell uname -m)

ifeq ($(ARCH),arm64)
	HOMEBREW_PREFIX	=	/opt/homebrew
else
	HOMEBREW_PREFIX	=	/usr/local
endif

CC          = gcc
CFLAGS      = -Wall -Wextra -Werror -I./include


ifeq ($(ARCH),arm64)
	CFLAGS	+=	-I$(HOMEBREW_PREFIX)/include
endif

LDFLAGS     = -L$(HOMEBREW_PREFIX)/lib -lcsfml-graphics -lcsfml-window -lcsfml-system -lm

SRC_DIR     = src
LIB_DIR     = lib
OBJ_DIR     = obj
INC_DIR     = include

# ═══════════════════════════════════════════════════════════════════════════
#                                 SOURCE FILES                                 
# ═══════════════════════════════════════════════════════════════════════════

SRC         = $(SRC_DIR)/main.c \
              $(SRC_DIR)/app.c \
              $(SRC_DIR)/paint.c \
              $(SRC_DIR)/ui.c \
              $(SRC_DIR)/utils.c \
              $(LIB_DIR)/cJSON.c \
              $(SRC_DIR)/config.c \
              $(SRC_DIR)/file_explorer.c

OBJ         = $(OBJ_DIR)/main.o \
              $(OBJ_DIR)/app.o \
              $(OBJ_DIR)/paint.o \
              $(OBJ_DIR)/ui.o \
              $(OBJ_DIR)/utils.o \
              $(OBJ_DIR)/cJSON.o \
              $(OBJ_DIR)/config.o \
              $(OBJ_DIR)/file_explorer.o

# ═══════════════════════════════════════════════════════════════════════════
#                                COLOR CODES                                   
# ═══════════════════════════════════════════════════════════════════════════

RESET       = \033[0m
BOLD        = \033[1m
DIM         = \033[2m
UNDERLINE   = \033[4m
BLINK       = \033[5m

# Regular colors
BLACK       = \033[0;30m
RED         = \033[0;31m
GREEN       = \033[0;32m
YELLOW      = \033[0;33m
BLUE        = \033[0;34m
PURPLE      = \033[0;35m
CYAN        = \033[0;36m
WHITE       = \033[0;37m

# Bold colors
BBLACK      = \033[1;30m
BRED        = \033[1;31m
BGREEN      = \033[1;32m
BYELLOW     = \033[1;33m
BBLUE       = \033[1;34m
BPURPLE     = \033[1;35m
BCYAN       = \033[1;36m
BWHITE      = \033[1;37m

# ═══════════════════════════════════════════════════════════════════════════
#                                  RULES                                       
# ═══════════════════════════════════════════════════════════════════════════

.PHONY: all clean fclean re header help run debug install check

all: header $(NAME) footer

header:
	@echo ""
	@echo "$(BCYAN)╔════════════════════════════════════════════════════════════════╗$(RESET)"
	@echo "$(BCYAN)║                                                                ║$(RESET)"
	@echo "$(BCYAN)║$(BWHITE)            🎨  C-PAINT PROFESSIONAL EDITION  🎨             $(BCYAN)║$(RESET)"
	@echo "$(BCYAN)║                                                                ║$(RESET)"
	@echo "$(BCYAN)╚════════════════════════════════════════════════════════════════╝$(RESET)"
	@echo ""
	@echo "$(BBLUE)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo "$(BWHITE)  🚀 Starting build process...$(RESET)"
	@echo "$(BBLUE)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo ""

$(NAME): $(OBJ)
	@echo ""
	@echo "$(BYELLOW)🔗 Linking object files...$(RESET)"
	@$(CC) $(OBJ) -o $(NAME) $(LDFLAGS)
	@echo "$(BGREEN)✅ Linking complete!$(RESET)"
	@echo ""

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "$(BCYAN)📦 Compiling:$(RESET) $(YELLOW)$<$(RESET) with command $(BCYAN)📦 Compiling:$(RESET) $(CC) $(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "   $(GREEN)├─ Generated:$(RESET) $@"
	@echo "   $(GREEN)└─ Status:$(RESET) $(BGREEN)✓ Success$(RESET)"
	@echo ""

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.c | $(OBJ_DIR)
	@echo "$(BCYAN)📦 Compiling:$(RESET) $(YELLOW)$<$(RESET) with command $(BCYAN)📦 Compiling:$(RESET) $(CC) $(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "   $(GREEN)├─ Generated:$(RESET) $@"
	@echo "   $(GREEN)└─ Status:$(RESET) $(BGREEN)✓ Success$(RESET)"
	@echo ""

$(OBJ_DIR):
	@echo "$(BPURPLE)📁 Creating object directory...$(RESET)"
	@mkdir -p $(OBJ_DIR)
	@echo "$(BGREEN)✅ Directory created: $(RESET)$(OBJ_DIR)"
	@echo ""

footer:
	@echo "$(BBLUE)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo "$(BGREEN)✨ Build completed successfully! ✨$(RESET)"
	@echo "$(BBLUE)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo ""
	@echo "$(BWHITE)📊 Build Statistics:$(RESET)"
	@echo "   $(CYAN)├─$(RESET) Binary name:    $(BWHITE)$(NAME)$(RESET)"
	@echo "   $(CYAN)├─$(RESET) Source files:   $(BWHITE)$(words $(SRC)) files$(RESET)"
	@echo "   $(CYAN)├─$(RESET) Object files:   $(BWHITE)$(words $(OBJ)) files$(RESET)"
	@echo "   $(CYAN)├─$(RESET) Compiler:       $(BWHITE)$(CC)$(RESET)"
	@echo "   $(CYAN)└─$(RESET) Flags:          $(BWHITE)$(CFLAGS)$(RESET)"
	@echo ""
	@echo "$(BGREEN)🎮 Ready to launch! Run with:$(RESET) $(BCYAN)./$(NAME)$(RESET)"
	@echo ""

clean:
	@echo ""
	@echo "$(BYELLOW)🧹 Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@echo "$(BGREEN)✅ Object files cleaned!$(RESET)"
	@echo ""

fclean: clean
	@echo "$(BYELLOW)🗑️  Removing binary...$(RESET)"
	@rm -f $(NAME)
	@echo "$(BGREEN)✅ Binary removed!$(RESET)"
	@echo ""
	@echo "$(BBLUE)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo "$(BGREEN)✨ Full clean completed! ✨$(RESET)"
	@echo "$(BBLUE)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo ""

re: fclean all

run: all
	@echo "$(BGREEN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo "$(BWHITE)🚀 Launching $(NAME)...$(RESET)"
	@echo "$(BGREEN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@./$(NAME)

debug: CFLAGS += -g3 -DDEBUG
debug: fclean all
	@echo "$(BYELLOW)🐛 Debug build complete!$(RESET)"
	@echo "$(CYAN)Run with: $(BWHITE)gdb ./$(NAME)$(RESET)"
	@echo ""

check:
	@echo ""
	@echo "$(BCYAN)╔════════════════════════════════════════════════════════════════╗$(RESET)"
	@echo "$(BCYAN)║                    DEPENDENCY CHECK                            ║$(RESET)"
	@echo "$(BCYAN)╚════════════════════════════════════════════════════════════════╝$(RESET)"
	@echo ""
	@echo "$(BWHITE)📋 Checking required dependencies...$(RESET)"
	@echo ""
	@which $(CC) > /dev/null 2>&1 && \
		echo "$(GREEN)✅ Compiler ($(CC)):$(RESET) $(BGREEN)Found$(RESET)" || \
		echo "$(RED)❌ Compiler ($(CC)):$(RESET) $(BRED)Not found$(RESET)"
	@echo ""
	@pkg-config --exists csfml-graphics 2>/dev/null && \
		echo "$(GREEN)✅ CSFML Graphics:$(RESET) $(BGREEN)Found$(RESET)" || \
		echo "$(RED)❌ CSFML Graphics:$(RESET) $(BRED)Not found$(RESET)"
	@pkg-config --exists csfml-window 2>/dev/null && \
		echo "$(GREEN)✅ CSFML Window:$(RESET) $(BGREEN)Found$(RESET)" || \
		echo "$(RED)❌ CSFML Window:$(RESET) $(BRED)Not found$(RESET)"
	@pkg-config --exists csfml-system 2>/dev/null && \
		echo "$(GREEN)✅ CSFML System:$(RESET) $(BGREEN)Found$(RESET)" || \
		echo "$(RED)❌ CSFML System:$(RESET) $(BRED)Not found$(RESET)"
	@echo ""
	@echo "$(BCYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo ""

install:
	@echo ""
	@echo "$(BYELLOW)📦 Installing CSFML dependencies...$(RESET)"
	@echo ""
	@if [ "$$(uname)" = "Darwin" ]; then \
		echo "$(CYAN)🍎 Detected macOS - using Homebrew$(RESET)"; \
		echo ""; \
		brew install csfml || echo "$(RED)Failed to install CSFML$(RESET)"; \
	elif [ "$$(uname)" = "Linux" ]; then \
		echo "$(CYAN)🐧 Detected Linux - using apt$(RESET)"; \
		echo ""; \
		sudo apt-get update && sudo apt-get install -y libcsfml-dev || echo "$(RED)Failed to install CSFML$(RESET)"; \
	else \
		echo "$(RED)❌ Unsupported OS$(RESET)"; \
	fi
	@echo ""
	@echo "$(BGREEN)✅ Installation complete!$(RESET)"
	@echo ""

help:
	@echo ""
	@echo "$(BCYAN)╔════════════════════════════════════════════════════════════════╗$(RESET)"
	@echo "$(BCYAN)║                         MAKEFILE HELP                          ║$(RESET)"
	@echo "$(BCYAN)╚════════════════════════════════════════════════════════════════╝$(RESET)"
	@echo ""
	@echo "$(BWHITE)📖 Available targets:$(RESET)"
	@echo ""
	@echo "  $(BGREEN)make$(RESET) or $(BGREEN)make all$(RESET)"
	@echo "     $(CYAN)└─$(RESET) Build the project"
	@echo ""
	@echo "  $(BGREEN)make clean$(RESET)"
	@echo "     $(CYAN)└─$(RESET) Remove object files"
	@echo ""
	@echo "  $(BGREEN)make fclean$(RESET)"
	@echo "     $(CYAN)└─$(RESET) Remove object files and binary"
	@echo ""
	@echo "  $(BGREEN)make re$(RESET)"
	@echo "     $(CYAN)└─$(RESET) Rebuild the entire project"
	@echo ""
	@echo "  $(BGREEN)make run$(RESET)"
	@echo "     $(CYAN)└─$(RESET) Build and run the program"
	@echo ""
	@echo "  $(BGREEN)make debug$(RESET)"
	@echo "     $(CYAN)└─$(RESET) Build with debug symbols"
	@echo ""
	@echo "  $(BGREEN)make check$(RESET)"
	@echo "     $(CYAN)└─$(RESET) Check for required dependencies"
	@echo ""
	@echo "  $(BGREEN)make install$(RESET)"
	@echo "     $(CYAN)└─$(RESET) Install CSFML dependencies"
	@echo ""
	@echo "  $(BGREEN)make help$(RESET)"
	@echo "     $(CYAN)└─$(RESET) Display this help message"
	@echo ""
	@echo "$(BCYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo "$(BWHITE)🎨 C-Paint Professional Edition$(RESET)"
	@echo "$(BCYAN)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo ""

# ═══════════════════════════════════════════════════════════════════════════
#                                END OF MAKEFILE                               
# ═══════════════════════════════════════════════════════════════════════════
