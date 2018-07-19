#include "log.h"
#include "config.h"
#include "cmd.h"


cmd_node_t * g_cmd_root_node;

int init_cmd_module()
{
	g_cmd_root_node = cmd_alloc_node((char *)"root",(char *)"root node",CMD_ROOT_HASH_SIZE, NULL); 
	if (! g_cmd_root_node) {
		LOG("Error: init root cmd node error ! ");
		return -1;
	}
	cmd_init_buildin();
	return 0;
}

int cmd_register_top_node(cmd_node_t * node)
{
	return cmd_register_node(g_cmd_root_node, node);

}
cmd_node_t * cmd_find_sub_node(cmd_node_t * father, char * cmdName)
{
	cmd_node_t * pos, *ret = NULL;
	if (! father  || !cmdName) {
		LOG("Error:  %s invalid arguments ", __func__);
		return NULL;
	}
	father->mutex->lock();
	list_for_each_entry(pos, &father->hashTbl[cmd_node_hash_name(father, cmdName)], hNode ) {
		if ( strcmp( pos->name, cmdName ) == 0  ) {
			ret = pos;
		}
	}
	father->mutex->unlock();
	return ret;
}

cmd_node_t * cmd_find_sub_node_exec(cmd_node_t * father, char * cmdName, int argc, char **argv)
{
	cmd_node_t * pos, *ret = NULL;
	if (!cmdName ) {
		LOG("%s input cmdName error, argc:%d ",__func__, argc);
		return NULL;
	}
	if (!father ) {
		LOG("%s father cmd is null, cmdName:%s, argc:%d ",__func__, cmdName, argc);
		return NULL;
	}
#ifdef TEST_TRACE
        LOG("%s father cmd : %s, to find :%s, argc:%d ",__func__, father->name, cmdName, argc);
#endif
	father->mutex->lock();
#ifdef TEST_TRACE
        LOG("%s cmd:%s, lock ",__func__, father->name );
#endif
	list_for_each_entry(pos, &father->hashTbl[cmd_node_hash_name(father, cmdName)], hNode ) {
		if ( strcmp( pos->name, cmdName ) == 0  ) {
#ifdef TEST_TRACE
        		LOG("%s find cmd %s in father %s, execute",__func__, cmdName, father->name );
#endif
			int rf = pos->func(argc, argv);
			if (rf) {
        			LOG("%s  cmd %s (father %s), execute fail, ret: %d",__func__, cmdName, father->name, rf );
			}
#ifdef TEST_TRACE
        		LOG("%s find cmd %s in father %s, execute finish, ret: %d",__func__, cmdName, father->name, rf );
#endif
			ret = pos;
			break;
		}
	}
	father->mutex->unlock();
#ifdef TEST_TRACE
        LOG("%s cmd:%s, unlock ",__func__, father->name );
#endif
	return ret;
}

cmd_node_t * cmd_find_top_node(char * cmdName)
{
   return cmd_find_sub_node(g_cmd_root_node, cmdName);
}
cmd_node_t * cmd_find_top_node_exec(char * cmdName, int argc, char **argv)
{
   return cmd_find_sub_node_exec(g_cmd_root_node, cmdName, argc, argv);
}
int cmd_register_node(cmd_node_t * father, cmd_node_t * node)
{
	unsigned int i = 0;
#ifdef TEST_TRACE
        LOG("register cmd %s to father cmd: %s", node->name, father->name);
#endif
	if (! father->hashTbl){
		father->hashTbl = (struct list_head *) malloc (sizeof(struct list_head) * father->hashSize); 
		if (! father->hashTbl){
			LOG("register cmd alloc memory fail !");
			return -1;
		}
		
		for(i = 0; i < father->hashSize; i++) {
			INIT_LIST_HEAD(& father->hashTbl[i]);
		}

	} 	

	if (! father->hashTbl){
		LOG("cmd register error ! ");
		return -1;
	}
	
	father->mutex->lock();
	list_add_tail(&node->hNode, &father->hashTbl[cmd_node_hash_name(father, node->name)]);
	
	// insert to order list
	cmd_node_t * pos, *n;
	int ret = 0, flag = 0;
	list_for_each_entry_safe( pos, n, &father->orderList, oNode ) {
		if ( (ret = strcmp(node->name, pos->name) ) < 0 ) {
			list_add_tail(&node->oNode, & pos->oNode);
			flag = 1;
			break;
		} else if (ret == 0) {
			father->mutex->unlock();
			LOG("duplicate register cmd %s ", node->name);
			return -1;
		} else {
			continue;
		}
	}

	if (! flag) {
		list_add_tail(&node->oNode, &father->orderList );
	}

        father->mutex->unlock();
	node->father = father;
#ifdef TEST_TRACE
        LOG("register cmd %s success", node->name);
#endif
	return 0;
}


// this func will free the cmd node
int cmd_unregister_free_node( cmd_node_t * node) 
{
	cmd_node_t * father = node->father;
	cmd_node_t * pos, *n;

	father->mutex->lock();
	list_del(&node->oNode)	;
	list_del(&node->hNode)	;
	father->mutex->unlock();

	list_for_each_entry_safe(pos, n, & node->orderList, oNode) {
		cmd_unregister_node(pos);	
	}
	if (node->hashTbl) {
		free(node->hashTbl);
	}
	if(node->name)
		free(node->name);
	if(node->note)
		free(node->note);
	if(node->mutex)
		delete node->mutex;
	free(node);
	return 0;
}

int cmd_unregister_node( cmd_node_t * node) 
{
	cmd_node_t * father = node->father;
	father->mutex->lock();
	list_del(&node->oNode)	;
	list_del(&node->hNode)	;
	father->mutex->unlock();
	return 0;
}
int cmd_free_node( cmd_node_t * node) 
{

	if (node->hashTbl) {
		free(node->hashTbl);
	}
	if(node->name)
		free(node->name);
	if(node->note)
		free(node->note);
	if(node->mutex)
		delete node->mutex;
	free(node);
	return 0;
}

int cmd_show_sub_cmd( cmd_node_t * node, int level) 
{
	cmd_node_t * pos;
#ifdef TEST_TRACE
	LOG("show cmd %s, level: %d ",node->name, level);
#endif	
	node->mutex->lock();
	list_for_each_entry(pos, & node->orderList, oNode) {
		LOG_RT("%s",pos->name);
		if (level == CMD_SHOW_DETAIL) {
			LOG_RT("    --%s",pos->note);
		}
	}

	node->mutex->unlock();	
#ifdef TEST_TRACE
	LOG("show cmd %s finish ",node->name);
#endif	
	return 0;
}
cmd_node_t * cmd_alloc_node(char *name,char *note, unsigned int hashSize, cmd_func_t func)
{
    cmd_node_t * cmd = (cmd_node_t * ) malloc (sizeof(cmd_node_t));
    if (!cmd)
	    return NULL;
    memset(cmd, 0, sizeof(cmd_node_t));
    cmd->mutex = new Mutex();
    INIT_LIST_HEAD(& cmd->orderList);
    cmd->func = func;
    cmd->name = strdup(name);
    cmd->note = strdup(note);
    cmd->hashSize = hashSize;
    cmd->hashMask = hashSize - 1;

   return cmd; 
}
