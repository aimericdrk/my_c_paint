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

SRC         = src/main.c \
              src/app/app_destroy.c \
              src/app/app_init.c \
              src/app/app_render.c \
              src/app/app_runner.c \
              src/app/events/handle_events.c \
			  src/config/config_manager.c \
              src/config/utils/config_get_default.c \
              src/config/utils/config_get_other.c \
              src/file_explorer/destroy_file_explorer.c \
              src/file_explorer/event/click.c \
              src/file_explorer/event/input_text.c \
              src/file_explorer/event/key_press.c \
              src/file_explorer/init_file_explorer.c \
              src/file_explorer/run_file_explorer.c \
              src/file_explorer/state_manager_file_explorer.c \
              src/file_explorer/utils_file_explorer.c \
			  src/paint/destroy_paint.c \
              src/paint/draw/draw_at_position.c \
              src/paint/draw/draw_circle_point.c \
              src/paint/draw/draw_fill.c \
              src/paint/draw/draw_line.c \
              src/paint/draw/draw_tools.c \
              src/paint/draw/draw_with_symmetry.c \
              src/paint/event_paint.c \
              src/paint/flip_canvas.c \
              src/paint/init_paint_state.c \
              src/paint/utils_paint.c \
			  src/ui/destroy_ui.c \
              src/ui/display_ui.c \
              src/ui/event_ui.c \
              src/ui/init_ui.c \
              src/ui/update_ui.c \
              src/utils/debug.c \
              src/utils/files_manager.c \
              src/utils/mouse.c \
              src/utils/undo_redo.c \
              src/utils/utils.c \
              lib/cJSON.c

# Auto-generate object files preserving directory structure
OBJ         = $(patsubst src/%,$(OBJ_DIR)/%,$(patsubst lib/%,$(OBJ_DIR)/%,$(SRC:.c=.o)))

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

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(BCYAN)📦 Compiling:$(RESET) $(YELLOW)$<$(RESET) with command $(BCYAN)📦 Compiling:$(RESET) $(CC) $(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "   $(GREEN)├─ Generated:$(RESET) $@"
	@echo "   $(GREEN)└─ Status:$(RESET) $(BGREEN)✓ Success$(RESET)"
	@echo ""

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(BCYAN)📦 Compiling:$(RESET) $(YELLOW)$<$(RESET) with command $(BCYAN)📦 Compiling:$(RESET) $(CC) $(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "   $(GREEN)├─ Generated:$(RESET) $@"
	@echo "   $(GREEN)└─ Status:$(RESET) $(BGREEN)✓ Success$(RESET)"
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

getsrcs:
	@echo ""
	@echo "$(BCYAN)📁 Auto-discovered Source Files (Makefile format):$(RESET)"
	@echo ""
	@echo "SRC         = \c"
	@first=1; \
	for file in $$(find $(SRC_DIR) $(LIB_DIR) -name "*.c" 2>/dev/null | sort); do \
		if [ $$first -eq 1 ]; then \
			echo "$$file \\"; \
			first=0; \
		else \
			echo "              $$file \\"; \
		fi; \
	done | sed '$$s/ \\$$//'
	@echo ""
	@echo "$(BWHITE)Total:$(RESET) $(BGREEN)$$(find $(SRC_DIR) $(LIB_DIR) -name '*.c' 2>/dev/null | wc -l | tr -d ' ') files$(RESET)"
	@echo ""

getproto:
	@echo ""
	@echo "$(BCYAN)📋 Auto-generated Function Prototypes (paint.h format):$(RESET)"
	@echo ""
	@find $(SRC_DIR) $(LIB_DIR) -name "*.c" 2>/dev/null | sort | xargs -I{} sh -c 'echo "// From file: {}"; grep -E "^[a-zA-Z_][a-zA-Z0-9_]*[ \t]+[a-zA-Z_][a-zA-Z0-9_]*\s*\(.*\)\s*{" {}' | sed 's/{$$//; s/^[ \t]*//; s/[ \t]*$$//' | sed '/^\/\//! s/$$/;/'
	@echo ""
	@echo "$(BWHITE)Prototypes generated!$(RESET)"
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
