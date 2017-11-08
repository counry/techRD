# -*- coding: utf-8 -*-  

import os, sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

import redis
import random
import time
import bz2
import csv
import json

ADMIN_GEOJSON_FILE = os.path.abspath(os.path.dirname(__file__) + 'admin.geojson')

class RedisSearchCreateData():
    def __init__(self):
        self.redis_dict_data = {}

    def createIndexRANDOM(self, num_docs = 100):
        try:
            ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ="
            print "insert RANDOM index num_docs=", num_docs
            admin_id = 70000
            admin_body_list = []
            for ab in range(51200):
                admin_body_list.append(ALPHABET[random.randint(0, len(ALPHABET)-1)])				
            admin_body_common = "".join(admin_body_list)

            for n in range(num_docs):
                admin_id = admin_id + 1
                self.redis_dict_data[str(admin_id)] = admin_body_common
                if len(self.redis_dict_data) % 10000 == 0:
                    print "get self.redis_dict_data ize=", len(self.redis_dict_data)

                if len(self.redis_dict_data) == num_docs:
                    break
            print "finish get self.redis_dict_data total = ", len(self.redis_dict_data)
            return len(self.redis_dict_data)
        except Exception,e:
            print Exception,":",e
            return None


    def createIndexGEOJSON(self, num_docs = 100):
        try:
            print "insert geojson index"

            with open(ADMIN_GEOJSON_FILE, 'r') as fp:
                fp_buf = fp.read()
                admin_geojson = json.loads(fp_buf)
                for ag in admin_geojson['features']:
                    admin_id = ag['properties']['@id']
                    admin_body = str(ag)
                    key = str(admin_id)
                    self.redis_dict_data[key] = admin_body
					
                    if len(self.redis_dict_data) % 500 == 0:
                        print "get self.redis_dict_data size=", len(self.redis_dict_data)
                    if len(self.redis_dict_data) == num_docs:
                        break

            print "finish get self.redis_dict_data total=", len(self.redis_dict_data)
            return len(self.redis_dict_data)
        except Exception,e:
            print Exception,":",e
            return None

    def testClient(self):
        try:
            num_docs = 30000
            num = self.createIndexGEOJSON(client, num_docs)
            if num and num < num_docs:
                self.createIndexRANDOM(client, num_docs - num)
        except Exception,e:
            print Exception,":",e

def set_to_redis(d):
    print "set to redis size ", len(d)
    NUM = 0
    r = redis.Redis()
    for k in d:
        NUM = NUM + 1
        r.set(k, d[k])
        if NUM % 10000 == 0:
            print "doing set to redis key : ", k, " size ", NUM 

if __name__ == '__main__':
    rc = RedisSearchCreateData()
    rc.testClient()
    print "start set to redis"
    set_to_redis(rc.redis_dict_data)
    print "finish set to redis"
    


