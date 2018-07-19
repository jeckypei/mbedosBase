/*
 * ===============================================================
 *  Natural Tiny Shell (NT-Shell) Application example.
 *  Version 0.0.6
 * ===============================================================
 * Copyright (c) 2010-2011 Shinichiro Nakamura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * ===============================================================
 */

#include "mbed.h"
#include "ntshell.h"
#include "ntopt.h"

#include "config.h"
#include "log.h"
#include "cmd.h"

#define CMD_TABLE_SIZE  (1 << 6) 
#define CMD_TABLE_MASK  (CMD_TABLE_SIZE - 1) 

ntshell_t ntshell;



int serial_func_read(char *buf, int cnt);
int serial_func_write(const char *buf, int cnt);
int cmd_cb_shell(const char *text);
void cmd_cb_opt(int argc, char **argv);

/**
 * Serial read function.
 */
int serial_func_read(char *buf, int cnt, void *term) {

    BetterSerial *serial = (BetterSerial *) term;
    for (int i = 0; i < cnt; i++) {
        buf[i] = serial->getc();
    }
    return 0;
}

/**
 * Serial write function.
 */
int serial_func_write(const char *buf, int cnt, void * term) {
    BetterSerial *serial = (BetterSerial *)term;
    for (int i = 0; i < cnt; i++) {
        serial->putc(buf[i]);
    }
    return 0;
}

/**
 * Callback function for ntshell module.
 */
int cmd_cb_shell(const char *text) {
    return ntopt_parse(text, cmd_cb_opt);
}

/**
 * Callback function for ntopt module.
 */
void cmd_cb_opt(int argc, char **argv) {
    if (argc == 0) {
	//LOG("no argument for input, from command line");
        return;
    }    
    cmd_node_t * ret;
#ifdef TEST_TRACE
    char buf[1024] = "";
    int i;
    for(i = 0; i < argc; i++) {
	    sprintf(buf, "%s %s", buf, argv[i]);
    }
    LOG("cmd input : %s ", buf);
#endif
    ret = cmd_find_top_node_exec(argv[0], argc, argv);
    if (!ret) {
	    LOG("Sorry: no such command: %s ! ", argv[0]);
    }
}

/**
 * Application code.
 */
void register_serial_shell(BetterSerial *serial) {
    shell_terminal_t * shell_term = (shell_terminal_t *) malloc(sizeof( shell_terminal_t )); 
    ntshell_t *ntshell = (ntshell_t *) malloc (sizeof(ntshell_t));
    if (!shell_term)
       return ;
    shell_term->terminal = (void *)serial;
    shell_term->func_read = serial_func_read;
    shell_term->func_write = serial_func_write;
    shell_term->func_cb = cmd_cb_shell;
    ntshell_execute(ntshell, shell_term);
}
int console_register_serial(BetterSerial *serial, char * prompt) 
{
   Thread *thread = new Thread();
   serial->printf("%s\r\n", prompt);
   thread->start(callback(register_serial_shell, serial));
   return 0;
}
