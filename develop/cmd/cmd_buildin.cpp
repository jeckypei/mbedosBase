#include "mbed.h"
#include "cmd.h"
#include "log.h"
#include "config.h"
#include "timeUtil.h"
 
/*help command*/
cmd_node_t * g_cmd_buildin_help ;

int cmd_buildin_help(int arg, char **argv) {
     cmd_show_sub_cmd(g_cmd_root_node, CMD_SHOW_DETAIL);
     return 0;	
}

int cmd_init_buildin_help ()
{
	g_cmd_buildin_help = cmd_alloc_node((char *)"help",(char *) "show all commands ", 0, cmd_buildin_help );
	cmd_register_top_node(g_cmd_buildin_help);
	return 0;
}

/******************************************************************************************/
/*version command*/
cmd_node_t * g_cmd_buildin_version;

int cmd_buildin_version(int arg, char **argv) {
	LOG_RT("WuLian OS version %s ", WL_OS_VERSION);
     return 0;	
}

int cmd_init_buildin_version()
{
	g_cmd_buildin_version = cmd_alloc_node((char *)"version", (char*)"show system version", 0, cmd_buildin_version );
	cmd_register_top_node(g_cmd_buildin_version);
	return 0;
}

/******************************************************************************************/
/*reboot command*/
cmd_node_t * g_cmd_buildin_reboot;

int cmd_buildin_reboot(int arg, char **argv) {
	LOG_RT("WuLian OS reboot ... " );
	LOG_RT("WuLian OS reboot ... FIXME: don't know how to reboot " );
     return 0;	
}

int cmd_init_buildin_reboot()
{
	g_cmd_buildin_reboot= cmd_alloc_node((char *)"reboot", (char *)"warm reboot system ", 0, cmd_buildin_reboot);
	cmd_register_top_node(g_cmd_buildin_reboot);
	return 0;
}

/******************************************************************************************/
/*board command*/

int cmd_buildin_func_board(int arg, char **argv) {
	LOG_RT("Board Type: ,FIXME: don't know how to get" );
	LOG_RT("SN: ,FIXME: don't know how to get" );
     return 0;	
}

int cmd_init_buildin_board()
{
	cmd_node_t * board = cmd_alloc_node((char *)"board", (char *)"show board info", 0, cmd_buildin_func_board);
	cmd_register_top_node(board);
	return 0;
}

/******************************************************************************************/
/*board command*/

void showDate()
{
	char buffer[64];
	time_t seconds = time(NULL);
	strftime(buffer, sizeof(buffer), "date UTC-0:  %Y-%m-%d %H:%M:%S %p", localtime(&seconds));
	LOG_RT("%s", buffer);
	LOG_RT("timeStamp : %u \n", seconds);

	seconds = getSysDefaultTZStamp(); 
	strftime(buffer, sizeof(buffer), "date UTC-8:  %Y-%m-%d %H:%M:%S %p", localtime(&seconds));
	LOG_RT("%s", buffer);
	LOG_RT("timeStamp : %u ", seconds);
}



void setDate(int year, int month, int day, int hour, int minute, int second)
{
	time_t  time;
	struct tm tminfo;
	make_time_info( year, month, day, hour, minute, second, &tminfo);
	LOG_RT("set sys date %d-%d-%d %d:%d:%d", year, month, day, hour, minute , second);
	time = mktime(&tminfo);
	time = getSysUtc0Stamp(time);  //convert to utc 0 time, from utc+8
	//set_time(time);
	set_time(time);
}

int cmd_buildin_func_date(int argc, char **argv) 
{
     if( argc == 1 ) {
		showDate();
		return 0;
     }

	 
    if( argc == 7 ) {
		setDate(atoi(argv[1]),  atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6])  );
		return 0;
    }

   LOG_RT("date:  error arguments ");
	
     return 0;	
}

int cmd_init_buildin_date()
{
	cmd_node_t * cmd = cmd_alloc_node((char *)"date", (char *)" show date:  date \n\t set date: date  2017 08 09 18 00 00", 0, cmd_buildin_func_date);
	cmd_register_top_node(cmd);
	return 0;
}

/******************************************************************************************/
// add new command here




/******************************************************************************************/





int cmd_init_buildin()
{
	cmd_init_buildin_version();
	cmd_init_buildin_help();
	cmd_init_buildin_reboot();
	cmd_init_buildin_board();
	cmd_init_buildin_date();
		
	return 0;
}


