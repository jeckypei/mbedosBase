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

#ifndef CMD_H
#define CMD_H

#include <mbed.h>
#include "BetterSerial.h" 


//#ifdef __cplusplus             //告诉编译器，这部分代码按C语言的格式进行编译，而不是C++的
//extern "C"{
//#endif
#include "list.h" 

enum {
	CMD_SHOW_NAME = 0,
	CMD_SHOW_DETAIL,
};
typedef struct cmd_node {
    Mutex * mutex;
    struct list_head * hashTbl; 
    unsigned int hashSize; 
    unsigned int hashMask; 
    struct list_head  orderList; //order list  
    struct list_head  hNode; //hash node 
    struct list_head  oNode; //order node 
    struct cmd_node *father;    
    char *name;
    char *note;
    int (*func)(int argc, char **argv);
} cmd_node_t;

typedef int (*cmd_func_t)(int, char**);

#define CMD_ROOT_HASH_SIZE  (1 << 5) 
#define CMD_SUB_HASH_SIZE  (1 << 4) 

extern cmd_node_t * g_cmd_root_node;

inline unsigned int cmd_node_hash_name(cmd_node_t * cmd , char *cmdName) 
{
    unsigned int i = 0,hash = 0;
    while(*cmdName != '\0' && i <= 12) {
	hash += *cmdName;
	cmdName++;
	i++;
    }

    return (cmd->hashMask & hash);
}

inline unsigned int cmd_top_node_hash(char *cmdName) 
{
    return cmd_node_hash_name(g_cmd_root_node, cmdName);
}

cmd_node_t * cmd_find_top_node_exec(char *cmdName, int argc, char **argv);
cmd_node_t * cmd_find_top_node(char *cmdName );
cmd_node_t * cmd_find_sub_node(cmd_node_t * father, char *cmdName );
cmd_node_t * cmd_find_sub_node_exec(cmd_node_t * father, char *cmdName ,int argc, char **argv );

int cmd_register_node(cmd_node_t *father, cmd_node_t * cmd);
int cmd_register_top_node(cmd_node_t * cmd);
int init_cmd_module();
int cmd_show_sub_cmd(cmd_node_t * cmd, int level);

cmd_node_t * cmd_alloc_node(char *name,char *note, unsigned int hashSize, cmd_func_t func);

int cmd_unregister_node( cmd_node_t * node); 
int cmd_free_node( cmd_node_t * node); 
int cmd_unregister_free_node( cmd_node_t * node); 

int console_register_serial(BetterSerial *serial, char * prompt);

/*
int cmd_init_buildin_help ();
int cmd_init_buildin_version ();
int cmd_init_buildin_reboot();
int cmd_init_buildin_board();
int cmd_init_buildin_date();
*/

extern int cmd_init_buildin();




// Please add your commands.
//#ifdef __cplusplus            
//}
//#endif

#endif
