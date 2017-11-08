# -*- coding: utf-8 -*-  

import os, sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

import redis
import random
import time
import bz2
import csv
import json

from redisearch import *

WILL_PLAY_TEXT = os.path.abspath(os.path.dirname(__file__) + 'will_play_text.csv.bz2')
TITLES_CSV = os.path.abspath(os.path.dirname(__file__) + 'titles.csv')
ADMIN_GEOJSON_FILE = os.path.abspath(os.path.dirname(__file__) + 'admin.geojson')
SET_REDIS = True

class RedisSearchCreateData():
    def createIndex(self, client, num_docs=100):
        print "insert num_docs=", num_docs
        assert isinstance(client, Client)
        # conn.flushdb()
        # client = Client('test', port=conn.port)
        try:
            if SET_REDIS:
                client.create_index((TextField('name', weight=5.0, no_stem=True),
                                 TextField('parent', no_stem=True),
                                 NumericField('admin_id', sortable=True)), no_term_offsets=True)
            else:
                client.create_index((TextField('name', weight=5.0, no_stem=True),
                                 TextField('parent', no_stem=True),
                                 NumericField('admin_id', sortable=True),
                                 TextField('body', no_stem=True, no_index=True)), no_term_offsets=True)
 
        except Exception, e:
            print Exception, ":", e
            exit(-1)
        # except redis.ResponseError:
        #     client.drop_index()
        #     return self.createIndex(client, num_docs=num_docs)

        num = self.createIndexGEOJSON(client, num_docs)
        if num and num < num_docs:
            self.createIndexRANDOM(client, num_docs - num)
        #self.createIndexCSV(client, num_docs)
        pass

    def createIndexRANDOM(self, client, num_docs = 100):
        try:
            assert isinstance(client, Client)
            ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ="
            print "insert RANDOM index num_docs=", num_docs
            chapters = {}

            admin_id = 70000

            admin_body_list = []
            for ab in range(51200):
                admin_body_list.append(ALPHABET[random.randint(0, len(ALPHABET)-1)])				
            admin_body_common = "".join(admin_body_list)

            for n in range(num_docs):
                admin_id = admin_id + n
                admin_name_list = []
                admin_parent_list = []
                for al in range(16):
                    admin_name_list.append(ALPHABET[random.randint(0, len(ALPHABET)-1)])
                admin_name = "".join(admin_name_list)

                for an in range(4):
                    parent_tmp_list = []
                    for al in range(8):
                        parent_tmp_list.append(ALPHABET[random.randint(0, len(ALPHABET)-1)])
                    admin_parent_list.append("".join(parent_tmp_list))
                admin_parent = "  ".join(admin_parent_list)

                key = str(admin_id)
                d = chapters.setdefault(key, {})
                d['name'] = admin_name
                d['parent'] = admin_parent
                d['admin_id'] = int(admin_id)
                if SET_REDIS:
                    client.redis.set(key, admin_body_common)
                else:
                    d['body'] = admin_body_common

                if len(chapters) % 10000 == 0:
                    print "get chapters size=", len(chapters)

                if len(chapters) == num_docs:
                    break

            indexer = client.batch_indexer(chunk_size=50)
            assert isinstance(indexer, Client.BatchIndexer)
            print "finish get chapters total = ", len(chapters)
            add_num = 0
            for key, doc in chapters.iteritems():
                add_num = add_num + 1
                indexer.add_document(key, **doc)
                if add_num % 10000 == 0:
                    print "add number ", add_num
            print "finish add document"
            indexer.commit()
            print "finish add commit"
            return len(chapters)
        except Exception,e:
            print Exception,":",e
            return None


    def createIndexGEOJSON(self, client, num_docs = 100):
        try:
            assert isinstance(client, Client)
            print "insert geojson index"
            chapters = {}
            body_dict = {}

            with open(ADMIN_GEOJSON_FILE, 'r') as fp:
                fp_buf = fp.read()
                admin_geojson = json.loads(fp_buf)
                for ag in admin_geojson['features']:
                    admin_id = ag['properties']['@id']
                    admin_name = ag['properties']['tags']['admin_name']
                    admin_body = str(ag)
                    admin_parent_raw = ag['properties']['parent_info']
                    admin_parent = []
                    parent_split1 = admin_parent_raw.split(":")
                    for ps1 in parent_split1:
                        parent_split2 = ps1.split(",")
                        admin_parent.append(parent_split2[0])


                    key = str(admin_id)
                    d = chapters.setdefault(key, {})
                    d['name'] = admin_name
                    d['parent'] = " ".join(admin_parent)
                    d['admin_id'] = int(admin_id)
                    if SET_REDIS:
                        body_dict[key] = admin_body
                        #client.redis.set(key, admin_body)
                    else:
                        d['body'] = admin_body
					
                    if len(chapters) % 500 == 0:
                        print "get chapters size=", len(chapters)
                    if len(chapters) == num_docs:
                        break

            indexer = client.batch_indexer(chunk_size=50)
            assert isinstance(indexer, Client.BatchIndexer)
            print "finish get chapters total=", len(chapters)
            for key, doc in chapters.iteritems():
                indexer.add_document(key, **doc)
            print "finish add document"
            indexer.commit()
            print "finish add commit and start set redis"
            self.set_to_redis(body_dict)
            print "finish set redis"
            return len(chapters)
        except Exception,e:
            print Exception,":",e
            return None

    
    def createIndexCSV(self, client, num_docs=100):

        assert isinstance(client, Client)
        print "insert CSV index num_docs=", num_docs
        chapters = {}

        with bz2.BZ2File(WILL_PLAY_TEXT) as fp:

            r = csv.reader(fp, delimiter=';')
            for n, line in enumerate(r):
                # ['62816', 'Merchant of Venice', '9', '3.2.74', 'PORTIA', "I'll begin it,--Ding, dong, bell."]

                play, chapter, character, text = line[1], line[2], line[4], line[5]

                key = '{}:{}'.format(play, chapter).lower()
                d = chapters.setdefault(key, {})
                d['name'] = play
                d['parent'] = d.get('txt', '') + ' ' + text
                d['admin_id'] = int(chapter or 0)

                if len(chapters) == num_docs:
                    break

        indexer = client.batch_indexer(chunk_size=50)
        assert isinstance(indexer, Client.BatchIndexer)

        for key, doc in chapters.iteritems():
            indexer.add_document(key, **doc)
        indexer.commit()

    def getCleanClient(self, name):
        """
        Gets a client client attached to an index name which is ready to be
        created
		flushdb : 删除当前数据库的所有数据
        """
        client = Client(name, port=6379)
        print "clinet connect index name ", name
        client.redis.flushdb()
        print "flushdb"
        try:
            print "start drop index"
            client.drop_index()
            print "drop index finish"
        except Exception, e:
            print Exception, ":", e

        return client

    def testClient(self):
        try:
            num_docs = 300000
            client = self.getCleanClient('admin_index')
            print "get client"
            self.createIndex(client, num_docs =num_docs)
            info = client.info()
            for k in [  'index_name', 'index_options', 'fields', 'num_docs',
                        'max_doc_id', 'num_terms', 'num_records', 'inverted_sz_mb',
                        'offset_vectors_sz_mb', 'doc_table_size_mb', 'key_table_size_mb',
                        'records_per_doc_avg', 'bytes_per_record_avg', 'offsets_per_term_avg',
                        'offset_bits_per_record_avg' ]:
                assert (k in info) == True


            assert client.index_name == info['index_name']
            assert num_docs == int(info['num_docs'])
        except Exception,e:
            print Exception,":",e

def set_to_redis(d):
        r = redis.Redis()
        for k in d:
            r.set(k, d[k])

if __name__ == '__main__':
    rc = RedisSearchCreateData()
    rc.testClient()
    


