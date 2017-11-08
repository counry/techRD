# -*- coding: utf-8 -*-  

import os, sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

import redis
import random
import time
import bz2
import csv

from redisearch import *


class RedisSearchTest():

    def testClient(self):
        client = Client('admin_index', port=6379)

        info = client.info()
        for k in ['index_name', 'index_options', 'fields', 'num_docs',
                  'max_doc_id', 'num_terms', 'num_records', 'inverted_sz_mb',
                  'offset_vectors_sz_mb', 'doc_table_size_mb', 'key_table_size_mb',
                  'records_per_doc_avg', 'bytes_per_record_avg', 'offsets_per_term_avg',
                  'offset_bits_per_record_avg']:
            assert (k in info) == True

        assert client.index_name == info['index_name']

        #normal
        res =  client.search(Query(u"海淀区"))
        assert isinstance(res, Result)

        print "res.total=", res.total
        for doc in res.docs:
            print "doc.id=", doc.id
            print "doc.name=", doc.name

        # test no content
        res = client.search(Query(u"@name:朝阳* @parent:中国 吉林省").no_content().paging(0, 100))
        print "res.total=", res.total
        for doc in res.docs:
            print "doc.id=", doc.id
            assert ("name" not in doc.__dict__) == True
            assert ("parent" not in doc.__dict__) == True
            assert ("admin_id" not in doc.__dict__) == True
            assert ("body" not in doc.__dict__) == True
            # test load_document
            doc_content = client.load_document(doc.id)
            if doc_content:
                print "doc.name=", doc_content.name
            else:
                print "no find doc ", doc_content.id

        res = client.search(Query(u"@name:朝阳区 @parent:中国 北京市").no_content().paging(0, 100))
        print "res.total=", res.total
        for doc in res.docs:
            print "doc.id=", doc.id
            assert ("name" not in doc.__dict__) == True
            assert ("parent" not in doc.__dict__) == True
            assert ("admin_id" not in doc.__dict__) == True
            assert ("body" not in doc.__dict__) == True
            doc_content = client.load_document(doc.id)
            if doc_content:
                print "doc.name=", doc_content.name
            else:
                print "no find doc ", doc_content.id

def testClientSearchSimple():
    client = Client('admin_index', port=6379)

    res = client.search(Query(u"@name:朝阳区").no_content().paging(0, 100))

def testClientSearch():
    client = Client('admin_index', port=6379)

    res = client.search(Query(u"@name:朝阳区 @parent:中国 北京市").no_content().paging(0, 100))
    #print "res.total=", res.total
    #for doc in res.docs:
    #    print "doc.id=", doc.id

def testClientSearchPrefix():
    client = Client('admin_index', port=6379)

    res = client.search(Query(u"@name:朝阳* @parent:中国 北京市").no_content().paging(0, 100))
    #print "res.total=", res.total
    #for doc in res.docs:
    #    print "doc.id=", doc.id

def testClientGetObjectRedisearch():
    docid = "60000"
    client = Client('admin_index', port=6379)
    doc_content = client.load_document(docid)
    #if doc_content:
    #    print "doc.name=", doc_content.name
    #else:
    #    print "no find doc ", doc_content.id

def testClientGetObjectRedis():
    docid = "60000"
    r = redis.Redis(connection_pool=redis.ConnectionPool(host='localhost', port=6379))
    #print r.get(docid)

if __name__ == '__main__':
    from timeit import Timer
    res_test = RedisSearchTest()
    res_test.testClient()
    testClientSearch()
    testClientSearchPrefix()
    testClientGetObjectRedisearch()
    #testClientGetObjectRedis()
    testClientSearchSimple()

    EXE_NUMBER = 100000
    REPEAT_NUMBER = 3
    t1 = Timer("testClientSearch()", "from __main__ import testClientSearch")
    t2 = Timer("testClientSearchPrefix()", "from __main__ import testClientSearchPrefix")
    t3 = Timer("testClientGetObjectRedisearch()", "from __main__ import testClientGetObjectRedisearch")
    #t4 = Timer("testClientGetObjectRedis()", "from __main__ import testClientGetObjectRedis")
    t5 = Timer("testClientSearchSimple()", "from __main__ import testClientSearchSimple")

    print "search ", t1.timeit(EXE_NUMBER)
    print "prefix_search ", t2.timeit(EXE_NUMBER)
    print "getdoc ", t3.timeit(EXE_NUMBER)
    #print "getredis ", t4.timeit(EXE_NUMBER)
    print "searchsimple ", t5.timeit(EXE_NUMBER)

    print "search ", t1.repeat(REPEAT_NUMBER, EXE_NUMBER)
    print "prefix_search ", t2.repeat(REPEAT_NUMBER, EXE_NUMBER)
    print "getdoc ", t3.repeat(REPEAT_NUMBER, EXE_NUMBER)
    #print "getredis ", t4.repeat(REPEAT_NUMBER, EXE_NUMBER)
    print "searchsimple ", t5.repeat(REPEAT_NUMBER, EXE_NUMBER)

