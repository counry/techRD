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
    struct timeval timeout = {2, 0};    //2s�ĳ�ʱʱ��
    //redisContext��Redis��������
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
    //redisReply��Redis����ظ����� redis���ص���Ϣ������redisReply������
    //redisReply *pRedisReply = (redisReply*)redisCommand(pRedisContext, "INFO");  //ִ��INFO����
    redisReply *pRedisReply = (redisReply*)redisCommand(pRedisContext, "FT.SEARCH myIdx hello world LIMIT 0 10");  //ִ��INFO����
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
    //������Redis����ʹ��ͬһ��redisReply����ʱ 
    //ÿһ��ִ����Redis�������Ҫ���redisReply �������һ�ε�Redis�������Ӱ��
    freeReplyObject(pRedisReply);   
    
    return 0;
}

