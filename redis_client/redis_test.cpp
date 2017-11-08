/*
 * =====================================================================================
 *
 *       Filename:  redis_test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/31/2017 03:28:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <hiredis/hiredis.h>
#include <iostream>
#include <string>
#include <stdio.h>

int main(int argc, char **argv)
{
    struct timeval timeout = {2, 0};    //2s的超时时间
    //redisContext是Redis操作对象
    redisContext *pRedisContext = (redisContext*)redisConnectWithTimeout("127.0.0.1", 6379, timeout);
    if ( (NULL == pRedisContext) || (pRedisContext->err) )
    {
        if (pRedisContext)
        {
            std::cout << "connect error:" << pRedisContext->errstr << std::endl;
        }
        else
        {
            std::cout << "connect error: can't allocate redis context." << std::endl;
        }
        return -1;
    }
    //redisReply是Redis命令回复对象 redis返回的信息保存在redisReply对象中
    //redisReply *pRedisReply = (redisReply*)redisCommand(pRedisContext, "INFO");  //执行INFO命令
    redisReply *pRedisReply = (redisReply*)redisCommand(pRedisContext, "FT.SEARCH myIdx hello world LIMIT 0 10");  //执行INFO命令
    if (pRedisReply->type == REDIS_REPLY_STRING) {
		std::cout << pRedisReply->str << std::endl;
	} else if (pRedisReply->type == REDIS_REPLY_ARRAY) {
		for (unsigned int i = 0; i < pRedisReply->elements; i++) {
			if (pRedisReply->element[i]->type == REDIS_REPLY_INTEGER)  {
				std::cout << i << " --> " << pRedisReply->element[i]->integer << std::endl;
			} else if (pRedisReply->element[i]->type == REDIS_REPLY_STRING){
				std::cout << i << " --> " << pRedisReply->element[i]->str << std::endl;
			} else if (pRedisReply->element[i]->type == REDIS_REPLY_ARRAY) {
				for (unsigned int j = 0; j < pRedisReply->element[i]->elements; j++) {
					if (pRedisReply->element[i]->element[j]->type == REDIS_REPLY_STRING) {
						std::cout << i << "->" << j << " --> " << pRedisReply->element[i]->element[j]->str << std::endl;
					} else {
						std::cout << i << " -> " << j << " type is " << pRedisReply->element[i]->element[j]->type << std::endl;
					}
				}
			} else {
				std::cout << i << " type is " << pRedisReply->element[i]->type << std::endl;
			}
		}
	} else if (pRedisReply->type == REDIS_REPLY_ERROR) {
		std::cout << pRedisReply->str << std::endl;
	} else {
		std::cout << "type : "<< pRedisReply->type << std::endl;
	}
    //当多条Redis命令使用同一个redisReply对象时 
    //每一次执行完Redis命令后需要清空redisReply 以免对下一次的Redis操作造成影响
    freeReplyObject(pRedisReply);   
    
    return 0;
}

