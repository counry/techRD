#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sstream>
#include <iostream>
#include <marisa.h>
#include <time_util.hpp>
#include <unistd.h>
#include <thread>
#include <future>
#include <vector>
#include <algorithm>
#include <iomanip>

const int G_THREAD_NUM = 8;
const long G_START = 0;
const long G_END = 100000;


namespace {


#define ASSERT //
#define TEST_START() 
#define TEST_END()

char ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ=";

void MakeKeyset(std::size_t num_keys, marisa::TailMode tail_mode, marisa::Keyset *keyset);

marisa::Trie gtrie;
      marisa::Keyset gkeyset;
    void InitTinyTrie() {
      MakeKeyset(100000000, MARISA_DEFAULT_TAIL, &gkeyset);
      gtrie.build(gkeyset);
    std::cout << "finish build trie..." << std::endl;
    std::cout << "gkeyset key number:[" <<  gkeyset.size()  << "]total length in byte:[" << gkeyset.total_length() << "]" << std::endl;
    std::cout << "gtrie key number:[" <<  gtrie.size()  << "]total length in byte:[" << gtrie.io_size() << "]" << std::endl;
    }


void TestTinyTrie() {
  Timer timer;
  Timer gt;
  gt.reset();
  for (std::size_t i = G_START; i < G_END; ++i) {
  //for (std::size_t i = 20; i < 30; ++i) {
    
  	marisa::Agent agent;
    agent.set_query(gkeyset[i].ptr(), gkeyset[i].length()); 
    //timer.reset();
    gtrie.lookup(agent);
    //gtrie.predictive_search(agent);
    //timer.write_text_to_log_file("test_marisa_trie", "test_marisa_trie");
    //std::cout.write(agent.key().ptr(), agent.key().length());
    //std::cout << ": " << agent.key().id() << std::endl;

//    agent.set_query(keyset[i].id());
//    trie.reverse_lookup(agent);
//    std::cout << "rlookup " << keyset[i].id() << " result : " << agent.key().ptr() << std::endl;
  }
  gt.write_text_to_screen("test_marisa_trie_finish");
}

std::vector<std::size_t> TestTinyThreadTrie(int idx, long start, long end) {
  std::vector<std::size_t> ret;
  for (std::size_t i = start; i < end; ++i) {
  marisa::Agent agent;
    agent.set_query(gkeyset[i].ptr(),gkeyset[i].length());
    gtrie.lookup(agent);
    ret.push_back(agent.key().id());

    //std::cout.write(agent.key().ptr(), agent.key().length());
    //std::cout << ": " << agent.key().id() << std::endl;

//    agent.set_query(keyset[i].id());
//    trie.reverse_lookup(agent);
//    std::cout << "rlookup " << keyset[i].id() << " result : " << agent.key().ptr() << std::endl;
  }
  return ret;
}

void MakeKeyset(std::size_t num_keys, marisa::TailMode tail_mode,
    marisa::Keyset *keyset) {
  char key_buf[16];
  for (std::size_t i = 0; i < num_keys; ++i) {
    //const std::size_t length = (std::rand() % (sizeof(key_buf)-5) + 5);
      std::size_t length = sizeof(key_buf);
      for (std::size_t j = 0; j < length; ++j) {
          key_buf[j] = ALPHABET[(std::rand() % sizeof(ALPHABET)-1)];
    }
    //std::cout << "keybuf:" << key_buf << std::endl;
    keyset->push_back(key_buf, length);
  }
}

void TestLookup(const marisa::Trie &trie, const marisa::Keyset &keyset) {
  marisa::Agent agent;
  for (std::size_t i = 0; i < keyset.size(); ++i) {
    agent.set_query(keyset[i].ptr(), keyset[i].length());
    ASSERT(trie.lookup(agent));
    ASSERT(agent.key().id() == keyset[i].id());

    agent.set_query(keyset[i].id());
    trie.reverse_lookup(agent);
    ASSERT(agent.key().length() == keyset[i].length());
    ASSERT(std::memcmp(agent.key().ptr(), keyset[i].ptr(),
        agent.key().length()) == 0);
  }
}

void TestCommonPrefixSearch(const marisa::Trie &trie,
    const marisa::Keyset &keyset) {
  marisa::Agent agent;
  for (std::size_t i = 0; i < keyset.size(); ++i) {
    agent.set_query(keyset[i].ptr(), keyset[i].length());
    ASSERT(trie.common_prefix_search(agent));
    ASSERT(agent.key().id() <= keyset[i].id());
    while (trie.common_prefix_search(agent)) {
      ASSERT(agent.key().id() <= keyset[i].id());
    }
    ASSERT(agent.key().id() == keyset[i].id());
  }
}

void TestPredictiveSearch(const marisa::Trie &trie,
    const marisa::Keyset &keyset) {
  marisa::Agent agent;
  for (std::size_t i = 0; i < keyset.size(); ++i) {
    agent.set_query(keyset[i].ptr(), keyset[i].length());
    ASSERT(trie.predictive_search(agent));
    ASSERT(agent.key().id() == keyset[i].id());
    while (trie.predictive_search(agent)) {
      ASSERT(agent.key().id() > keyset[i].id());
    }
  }
}

void TestTrie(int num_tries, marisa::TailMode tail_mode,
    marisa::NodeOrder node_order, marisa::Keyset &keyset) {
  for (std::size_t i = 0; i < keyset.size(); ++i) {
    keyset[i].set_weight(1.0F);
  }

  marisa::Trie trie;
  trie.build(keyset, num_tries | tail_mode | node_order);

  ASSERT(trie.num_tries() == (std::size_t)num_tries);
  ASSERT(trie.num_keys() <= keyset.size());

  ASSERT(trie.tail_mode() == tail_mode);
  ASSERT(trie.node_order() == node_order);

  TestLookup(trie, keyset);
  TestCommonPrefixSearch(trie, keyset);
  TestPredictiveSearch(trie, keyset);

  trie.save("marisa-test.dat");

  trie.clear();
  trie.load("marisa-test.dat");

  ASSERT(trie.num_tries() == (std::size_t)num_tries);
  ASSERT(trie.num_keys() <= keyset.size());

  ASSERT(trie.tail_mode() == tail_mode);
  ASSERT(trie.node_order() == node_order);

  TestLookup(trie, keyset);

  {
    std::FILE *file;
#ifdef _MSC_VER
    ASSERT(::fopen_s(&file, "marisa-test.dat", "wb") == 0);
#else  // _MSC_VER
    file = std::fopen("marisa-test.dat", "wb");
    ASSERT(file != NULL);
#endif  // _MSC_VER
    marisa::fwrite(file, trie);
    std::fclose(file);
    trie.clear();
#ifdef _MSC_VER
    ASSERT(::fopen_s(&file, "marisa-test.dat", "rb") == 0);
#else  // _MSC_VER
    file = std::fopen("marisa-test.dat", "rb");
    ASSERT(file != NULL);
#endif  // _MSC_VER
    marisa::fread(file, &trie);
    std::fclose(file);
  }

  ASSERT(trie.num_tries() == (std::size_t)num_tries);
  ASSERT(trie.num_keys() <= keyset.size());

  ASSERT(trie.tail_mode() == tail_mode);
  ASSERT(trie.node_order() == node_order);

  TestLookup(trie, keyset);

  trie.clear();
  trie.mmap("marisa-test.dat");

  ASSERT(trie.num_tries() == (std::size_t)num_tries);
  ASSERT(trie.num_keys() <= keyset.size());

  ASSERT(trie.tail_mode() == tail_mode);
  ASSERT(trie.node_order() == node_order);

  TestLookup(trie, keyset);

  {
    std::stringstream stream;
    stream << trie;
    trie.clear();
    stream >> trie;
  }

  ASSERT(trie.num_tries() == (std::size_t)num_tries);
  ASSERT(trie.num_keys() <= keyset.size());

  ASSERT(trie.tail_mode() == tail_mode);
  ASSERT(trie.node_order() == node_order);

  TestLookup(trie, keyset);
}

void TestTrie(marisa::TailMode tail_mode, marisa::NodeOrder node_order,
    marisa::Keyset &keyset) {
  TEST_START();
  std::cout << ((tail_mode == MARISA_TEXT_TAIL) ? "TEXT" : "BINARY") << ", ";
  std::cout << ((node_order == MARISA_WEIGHT_ORDER) ?
      "WEIGHT" : "LABEL") << ": ";

  for (int i = 1; i < 5; ++i) {
    TestTrie(i, tail_mode, node_order, keyset);
  }

  TEST_END();
}

void TestTrie(marisa::TailMode tail_mode) {
  marisa::Keyset keyset;
  MakeKeyset(1000, tail_mode, &keyset);

  TestTrie(tail_mode, MARISA_WEIGHT_ORDER, keyset);
  TestTrie(tail_mode, MARISA_LABEL_ORDER, keyset);
}

void TestTrie() {
  TestTrie(MARISA_TEXT_TAIL);
  TestTrie(MARISA_BINARY_TAIL);
}

}  // namespace

int main() try {
  std::srand((unsigned int)std::time(NULL));
  InitTinyTrie();
  TestTinyTrie();
  
  Timer timer;
  std::vector<std::vector<std::size_t>> ret_parallel_async;
  std::vector<std::future<std::vector<std::size_t>>> vfuture(G_THREAD_NUM);
  timer.reset();
  long start = G_START;
  for (long i = 0; i < G_THREAD_NUM; i++) {
    vfuture[i] = std::async(std::launch::async, TestTinyThreadTrie, i, start, start + (G_END - G_START)/G_THREAD_NUM);
    start = start + (G_END - G_START)/G_THREAD_NUM;
  }
  for (auto &vf : vfuture) {
    ret_parallel_async.push_back(vf.get());
  }
  ret_parallel_async.push_back(TestTinyThreadTrie(G_THREAD_NUM, start + (G_END - G_START)/G_THREAD_NUM, G_END));
  std::cout << "thread number " << G_THREAD_NUM << std::endl;
  timer.write_text_to_screen("test_marisa_trie_parallel_async");
#if 0
  std::ofstream log_file_parallel_async("dump.marisa.trie.parallel.async", std::ios_base::out);
  for_each(ret_parallel_async.begin(), ret_parallel_async.end(), [&log_file_parallel_async](std::vector<std::size_t > &ret) {
      for_each(ret.begin(), ret.end(),
               [&log_file_parallel_async](std::size_t &st) { log_file_parallel_async << st << std::endl; });
  });
  log_file_parallel_async.close();
#endif
  //TestTrie();

  return 0;
} catch (const marisa::Exception &ex) {
  std::cerr << ex.what() << std::endl;
  throw;
}
