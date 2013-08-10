/**
 *******************************************************************************
 * @file shell.c
 * @author Keidan
 * @date 05/08/2013
 * @par Project
 * tk
 *
 * @par Copyright
 * Copyright 2011-2013 Keidan, all right reserved
 *
 * This software is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY.
 *
 * Licence summary : 
 *    You can modify and redistribute the sources code and binaries.
 *    You can send me the bug-fix
 *
 * Term of the licence in in the file licence.txt.
 *
 *******************************************************************************
 */
#include <tk/shell/shell.h>
#include <tk/sys/log.h>
#include <stdlib.h>
#include <string.h>

struct shell_s {
    shell_term_t term;
    stringbuffer_t prompt;
    _Bool icanon;
    _Bool exit_loop;
};


/**
 * @fn shell_t shell_new(_Bool icanon)
 * @brief Allocation of a new shell.
 * @param icanon Enable canonical read.
 * @return The shell else NULL on error.
 */
shell_t shell_new(_Bool icanon) {
  struct shell_s *s = malloc(sizeof(struct shell_s));
  if(!s) {
    logger(LOG_ERR, "%s: Not enough memory.\n", __func__);
    return NULL;
  }
  memset(s, 0, sizeof(struct shell_s));
  if((s->term = shell_term_new()) == NULL) {
    shell_delete(s);
    logger(LOG_ERR, "%s: Unable to allocate a new terminal.\n", __func__);
    return NULL;
  }
  s->icanon = icanon;
  shell_term_icanon(s->icanon);
  if((s->prompt = stringbuffer_new()) == NULL) {
    shell_delete(s);
    logger(LOG_ERR, "%s: Unable to allocate a new buffer.\n", __func__);
    return NULL;
  }
  return s;
}

/**
 * @fn void shell_delete(shell_t shell)
 * @brief Delete an allocated shell.
 * @param shell The shell.
 */
void shell_delete(shell_t shell) {
  struct shell_s *s = (struct shell_s*)shell;
  if(s) {
    shell_exit_loop(s);
    if(s->term)
      shell_term_delete(s->term), s->term = NULL;
    if(s->prompt)
      stringbuffer_delete(s->prompt), s->prompt = NULL;
    shell_term_icanon(!s->icanon);
    free(s);
  }
}

/**
 * @fn void shell_set_prompt(shell_t shell, const char* prompt)
 * @brief Update the prompt value.
 * @param shell The shell.
 * @param prompt The prompt value else NULL to erase it.
 */
void shell_set_prompt(shell_t shell, const char* prompt) {
  struct shell_s *s = (struct shell_s*)shell;
  if(!s) return;
  if(prompt) stringbuffer_copy(s->prompt, prompt);
  else stringbuffer_clear(s->prompt);
}

/**
 * @fn const char* shell_get_prompt(shell_t shell)
 * @brief Get the prompt value.
 * @param shell The shell.
 * @return The prompt value else NULL.
 */
const char* shell_get_prompt(shell_t shell) {
  struct shell_s *s = (struct shell_s*)shell;
  if(!s) return NULL;
  return stringbuffer_to_str(s->prompt);
}

/**
 * @fn void shell_print_prompt(shell_t shell)
 * @brief Print the prompt.
 * @param shell The shell.
 */
void shell_print_prompt(shell_t shell) {
  struct shell_s *s = (struct shell_s*)shell;
  if(!s) return;
  shell_term_set_foreground(s->term, STC_BLUE);
  shell_term_set_bold(s->term, 1);
  shell_term_printf(s->term, "%s ", stringbuffer_to_str(s->prompt));
  shell_term_restore(s->term);
}

/**
 * @fn shell_term_t shell_get_term(shell_t shell)
 * @brief Get the associated terminal.
 * @param shell The shell.
 * @return The term.
 */
shell_term_t shell_get_term(shell_t shell) {
  struct shell_s *s = (struct shell_s*)shell;
  if(!s) return NULL;
  return s->term;
}

/**
 * @fn int shell_exit_loop(shell_t shell)
 * @brief Kill the main loop
 * @param shell The shell.
 * @return -1 on error else 0.
 */
int shell_exit_loop(shell_t shell) {
  struct shell_s *s = (struct shell_s*)shell;
  if(!s) return -1;
  s->exit_loop = 1;
  return 0;
}

/**
 * @fn int shell_main_loop(shell_t shell)
 * @brief Entering into the main loop.
 * @param shell The shell.
 * @return -1 on error else 0.
 */
int shell_main_loop(shell_t shell) {
  struct shell_s *s = (struct shell_s*)shell;
  if(!s) return -1;
  int ch = VK_ENTER;
  _Bool managed, reloop;
  while(!s->exit_loop) {
    if(ch == VK_ENTER)
      shell_print_prompt(s);
    managed = 1;
    reloop = 0;
    for(;;) {
      ch = shell_term_getch();
      switch(ch) {
	case VK_PAGE_UP:         /*shell_scroll_buffer_move_up();*/ break;
	case VK_PAGE_DOWN:       /*shell_scroll_buffer_move_down();*/ break;
	case VK_UP:              /*shell_buffer_history_change_dir(true);*/ break;
	case VK_DOWN:            /*shell_buffer_history_change_dir(false);*/ break;
        case VK_TAB:             /*shell_buffer_history_completion();*/ break;
	case VK_CTRL_K:          /*shell_buffer_erase_after_cursor();*/ break;
	case VK_CTRL_A:          /*shell_buffer_move_cursor_at(SCD_FIRST);*/ break;
	case VK_CTRL_E:          /*shell_buffer_move_cursor_at(SCD_LAST);*/ break;
	case VK_CTRL_L:          shell_term_clrscr(); reloop = 1; break;
	case VK_CTRL_D:          exit(0); break; /* logout ? */
	case VK_RIGHT:           /*shell_buffer_move_cursor_at(SCD_RIGHT);*/ break;
	case VK_LEFT:            /*shell_buffer_move_cursor_at(SCD_LEFT);*/ break;
	case VK_BACKSPACE:       /*shell_buffer_remove_and_display(true);*/ break;
	case VK_BACKSPACE_ALT:   /*shell_buffer_remove_and_display(true);*/ break;
	case VK_DEL:             /*shell_buffer_remove_and_display(false);*/ break;
	case VK_ENTER:           /*shell_buffer_new_line();*/ break;
	default: managed = 0; break;
      }
      if(reloop) break;
      if(ch < 1) break;
      if(ch == VK_ENTER) shell_term_printf(s->term, "\n"); break;
      if(!managed) shell_term_printf(s->term, "%c", ch);
    }
    if(ch < 1) continue;
    //shell_command_fire();
  }
  return 0;
}
