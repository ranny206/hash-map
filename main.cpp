#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <iostream>
#include "hash_map1.hpp"

#include <vector>
#include <string>
#include <list>
#include <forward_list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <stack>
#include <queue>
#include <deque>

#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <tuple>
#include <type_traits>
#include <functional>
#include <utility>
#include <atomic>
#include <thread>
#include <future>
#include <chrono>
#include <iterator>
#include <memory>


using namespace std;
using namespace fefu;

TEST_CASE("constructor") {
  hash_map<int, int> h(8);
  REQUIRE(h.size() == 0);
  REQUIRE(h.bucket_count() == 8);

  //Copy
  auto x = std::make_pair(5, 6);
  h.insert(x);
  hash_map<int, int> h1(h);
  REQUIRE(h1.size() == 1);
  REQUIRE(h1.bucket_count() == 8);
  REQUIRE(h1[5] == 6);
}

TEST_CASE("move constructor") {
  hash_map<int, int> h(8);
  auto x = std::make_pair(5, 6);
  h.insert(x);
  hash_map<int, int> h1(std::move(h));
  REQUIRE(h1.size() == 1);
  REQUIRE(h1.bucket_count() == 8);
  REQUIRE(h1[5] == 6);
}

TEST_CASE("copy =") {
  hash_map<int, int> h(8);
  hash_map<int, int> h1(9);
  auto x = std::make_pair(5, 6);
  h1.insert(x);
  h = h1;
  REQUIRE(h.size() == 1);
  REQUIRE(h.bucket_count() == 9);
  REQUIRE(h[5] == 6);
}

TEST_CASE("move =") {
  hash_map<int, int> h(8);
  auto x = std::make_pair(5, 6);
  h.insert(x);
  hash_map<int, int> h1 = std::move(h);
  REQUIRE(h1.size() == 1);
  REQUIRE(h1.bucket_count() == 8);
  REQUIRE(h1[5] == 6);
}

TEST_CASE("constructor list") {
  initializer_list<pair<int, int>> l = {{1, 1}, {2, 2}, {3, 3}};
  hash_map<int, int> h(l.begin(), l.end(), 6);
  REQUIRE(h.size() == 3);
  REQUIRE(h.bucket_count() == 12);
}

TEST_CASE("insert") {
  hash_map<int, int> h(8);
  pair<int, int> x(5, 6);
  auto p = h.insert(x);
  REQUIRE(h.size() == 1);
  REQUIRE(p.second);

  h.erase(5);
  p = h.insert(x);
  REQUIRE(h.size() == 1);
  REQUIRE(p.second);

  int a = 5;
  int b = 6;
  pair<int, int> x1(a, b);
  auto p1 = h.insert(x1);
  REQUIRE(h.size() == 1);
  REQUIRE_FALSE(p1.second);

  a = 4;
  b = 4;
  pair<int, int> x2(a, b);
  auto p2 = h.insert(x2);
  REQUIRE(h.size() == 2);
  REQUIRE(p2.second);

  a = 7;
  b = 8;
  pair<int, int> x3(a, b);
  auto p3 = h.insert(x3);
  REQUIRE(h.size() == 3);
  REQUIRE(p3.second);

  a = 9;
  b = 7;
  pair<int, int> x4(a, b);
  auto p4 = h.insert(x4);
  REQUIRE(h.size() == 4);
  REQUIRE(h.bucket_count() == 16);
  REQUIRE(p4.second);

  h.erase(9);
  p4 = h.insert(x4);
  REQUIRE(h.size() == 4);
  REQUIRE(h.bucket_count() == 16);
  REQUIRE(p4.second);

  hash_map<int, int> hh(6);
  pair<int, int> s(2, 6);
  pair<int, int> s1(3, 7);
  pair<int, int> s2(4, 4);
  hh.insert(s);
  hh.insert(s1);
  hh.insert(s2);
  hh.erase(3);
  pair<int, int> s3(15, 8);
  hh.insert(s3);
  REQUIRE(hh.size() == 3);
  REQUIRE(hh.bucket_count() == 12);

  hash_map<int, int> h3(5);
  pair<int, int> s5(4, 6);
  h3.insert(s5);
  h3.erase(4);
  h3.insert(s5);
  REQUIRE(h3.size() == 1);
}

TEST_CASE("[]") {
  hash_map<int, int> h(8);
  pair<int, int> x(5, 6);
  pair<int, int> x1(7, 8);
  h.insert(x1);
  h.insert(x);
  REQUIRE(h[5] == 6);
  REQUIRE(h[7] == 8);
  REQUIRE_FALSE(h[3] == 4);
}

TEST_CASE("insert_or_assign") {
  hash_map<int, int> h(8);
  pair<int, int> x(5, 6);
  pair<int, int> x1(5, 8);
  h.insert(x);
  REQUIRE(h[5] == 6);
  h.insert_or_assign(x1.first, x1.second);
  REQUIRE(h[5] == 8);
  pair<int, int> x2(7, 6);
  h.insert_or_assign(x2.first, x2.second);
  h.insert_or_assign(6, 4);
  REQUIRE(h[6] == 4);
  h.insert_or_assign(6, 7);
  REQUIRE(h[6] == 7);
}

TEST_CASE("erase") {
  hash_map<int, int> h(8);
  pair<int, int> x(5, 6);
  pair<int, int> x1(7, 8);
  h.insert(x);
  h.insert(x1);
  h.erase(5);
  h.erase(7);
  REQUIRE(h.size() == 0);
  auto a = h.erase(2);
  REQUIRE(a == 0);
  auto p = h.insert(x1);
  REQUIRE(p.second);
  auto i = h.erase(h.cend());
}

TEST_CASE("erase_iterator") {
  hash_map<int, int> h1(8);
  pair<int, int> x(5, 6);
  h1.insert(x);
  h1.erase(h1.begin());
  REQUIRE(h1.empty());
  h1.insert(x);
  const pair<int, int> x1(7, 8);
  h1.insert(x1);
  h1.erase(h1.begin());
  REQUIRE_FALSE(h1.cbegin() == h1.cend());
  REQUIRE(h1.begin().operator*().second == 8);
  auto i = h1.erase(h1.end());
  REQUIRE(i == h1.end());

}

TEST_CASE("erase_range") {
  hash_map<int, int> h1(8);
  pair<int, int> x(5, 6);
  h1.insert(x);
  pair<int, int> x1(7, 8);
  h1.insert(x1);
  h1.erase(h1.begin(), h1.end());
  REQUIRE(h1.empty());
}

TEST_CASE("clear") {
  hash_map<int, int> h1(8);
  pair<int, int> x(5, 6);
  h1.insert(x);
  pair<int, int> x1(7, 8);
  h1.insert(x1);
  h1.clear();
  REQUIRE(h1.size() == 0);
  REQUIRE(h1.empty());
}

TEST_CASE("find") {
  hash_map<int, int> h1(8);
  pair<int, int> x(5, 6);
  h1.insert(x);
  REQUIRE_FALSE(h1.find(5) == h1.end());
  REQUIRE(h1.find(6) == h1.end());
}

TEST_CASE("constructor_list") {
  hash_map<int, int> h({{1, 1}, {2, 2}, {3, 3}}, 3);
  REQUIRE(h.size() == 3);
  REQUIRE(h[1] == 1);
  REQUIRE(h[2] == 2);
  REQUIRE(h[3] == 3);
}

TEST_CASE("==") {
  hash_map<int, int> h1(8);
  hash_map<int, int> h2(9);
  pair<int, int> x(5, 6);
  h1.insert(x);
  h2.insert(x);
  REQUIRE(h1 == h2);
  pair<int, int> x1(7, 8);
  h1.insert(x1);
  REQUIRE_FALSE(h1 == h2);
  pair<int, int> x2(4, 8);
  h2.insert(x2);
  REQUIRE_FALSE(h1 == h2);
}

TEST_CASE("try_emplace") {
  hash_map<int, int> h(8);
  auto p = h.try_emplace(5, 6);
  REQUIRE(h.size() == 1);
  REQUIRE(p.second);

  auto p1 = h.try_emplace(5, 6);
  REQUIRE(h.size() == 1);
  REQUIRE_FALSE(p1.second);

  auto p2 = h.try_emplace(4, 4);
  REQUIRE(h.size() == 2);
  REQUIRE(p2.second);

  auto p3 = h.try_emplace(7, 8);
  REQUIRE(h.size() == 3);
  REQUIRE(p3.second);

  auto p4 = h.try_emplace(9, 7);
  REQUIRE(h.size() == 4);
  REQUIRE(h.bucket_count() == 16);
  REQUIRE(p4.second);

  h.erase(9);
  p4 = h.try_emplace(9, 7);
  REQUIRE(h.size() == 4);
  REQUIRE(h.bucket_count() == 16);
  REQUIRE(p4.second);

  hash_map<int, int> h1(4);
  pair<int, int> x(3, 2);
  pair<int, int> x1(4, 5);
  auto p5 = h1.try_emplace(x1.first, x1.second);
  REQUIRE(h1.size() == 1);
  REQUIRE(h1.bucket_count() == 4);
  REQUIRE(p4.second);

  p5 = h1.try_emplace(x.first, x.second);
  REQUIRE(h1.size() == 2);
  REQUIRE(h1.bucket_count() == 8);
  REQUIRE(p4.second);

  hash_map<int, int> hh(4);
  pair<int, int> s(2, 8);
  hh.try_emplace(s.first, s.second);
  hh.erase(2);
  hh.try_emplace(s.first, s.second);
  REQUIRE(hh.size() == 1);
}

TEST_CASE("swap") {
  hash_map<int, int> h1(8);
  pair<int, int> x(5, 6);
  h1.insert(x);
  pair<int, int> x1(7, 8);
  h1.insert(x1);
  hash_map<int, int> h2(7);
  pair<int, int> x2(6, 4);
  h2.insert(x2);
  h1.swap(h2);
  REQUIRE(h1.size() == 1);
  REQUIRE(h2.size() == 2);
  REQUIRE(h1.bucket_count() == 7);
  REQUIRE(h2.bucket_count() == 8);
  REQUIRE(h1[6] == 4);
  REQUIRE(h2[5] == 6);
  REQUIRE(h2[7] == 8);
}

TEST_CASE("contains") {
  hash_map<int, int> h1(8);
  pair<int, int> x(5, 6);
  h1.insert(x);
  REQUIRE(h1.contains(5));
  REQUIRE_FALSE(h1.contains(7));
}

TEST_CASE("at") {
  hash_map<int, int> h1(8);
  pair<int, int> x(5, 6);
  h1.insert(x);
  REQUIRE(h1.at(5) == 6);
  REQUIRE_THROWS(h1.at(7));
  const pair<int, int> x1(7, 5);
  h1.insert(x1);
  REQUIRE(h1.at(7) == 5);
  REQUIRE_THROWS(h1.at(2));
}

TEST_CASE("bucket") {
  hash_map<int, int> h1(8);
  pair<int, int> x(5, 6);
  h1.insert(x);
  REQUIRE(h1.bucket(5) == 5);
  REQUIRE(h1.bucket(4) == 8);
}

TEST_CASE("max_load_factor") {
  hash_map<int, int> h1(8);
  float e = 0.0000001;
  REQUIRE(h1.max_load_factor() - 0.5 < e);
  h1.max_load_factor(0.4);
  REQUIRE(h1.max_load_factor() - 0.4 < e);
}

TEST_CASE("merge") {
  hash_map<int, int> h1(8);
  hash_map<int, int> h2(9);
  pair<int, int> x(5, 6);
  h1.insert(x);
  h2.insert(x);
  pair<int, int> x1(7, 7);
  h1.insert(x1);
  pair<int, int> x2(8, 4);
  h1.insert(x2);
  h2.merge(h1);
  REQUIRE(h2.size() == 3);
}

TEST_CASE("begin") {
  hash_map<int, int> h(8);
  REQUIRE(h.begin() == h.end());
  REQUIRE(h.begin() == h.end());
  const pair<int, int> x(5, 6);
  h.insert(x);
  REQUIRE(h.begin().operator*().second == 6);
}

TEST_CASE("cbegin") {
  hash_map<int, int> h1(9);
  REQUIRE(h1.cbegin() == h1.end());
  pair<int, int> x(5, 6);
  h1.insert(x);
  REQUIRE_FALSE(h1.cbegin() == h1.end());
}

TEST_CASE("emplace") {
  hash_map<int, int> h1(9);
  pair<int, int> x(5, 6);
  h1.emplace(x);
  REQUIRE(h1[5] == 6);
  REQUIRE(h1.size() == 1);
}

TEST_CASE("*") {
  hash_map<int, int> h1(9);
  pair<int, int> x(5, 6);
  h1.insert(x);
  auto p = h1.begin();
  REQUIRE(p.operator*().second == 6);
}







