#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <type_traits>
#include <vector>

namespace fefu {

template<typename T>
class allocator {
 public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = typename std::add_lvalue_reference<T>::type;
  using const_reference = typename std::add_lvalue_reference<const T>::type;
  using value_type = T;

  allocator() noexcept = default;

  allocator(const allocator &) noexcept = default;

  template<class U>
  allocator(const allocator<U> &) noexcept {

  }

  ~allocator() {

  }

  pointer allocate(size_type size) {
    return static_cast<pointer>(::operator new(size * sizeof(value_type)));
  }

  void deallocate(pointer p, size_type n) noexcept {
    if (p != nullptr) {
      ::operator delete(p, n);
    }
  }
};

template<typename K, typename T,
    typename Hash = std::hash<K>,
    typename Pred = std::equal_to<K>,
    typename Alloc = allocator<std::pair<const K, T>>>
class hash_map;

template<typename ValueType1>
class hash_map_const_iterator;

template<typename ValueType>
class hash_map_iterator {
 public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = ValueType;
  using difference_type = std::ptrdiff_t;
  using reference = ValueType &;
  using pointer = ValueType *;

  hash_map_iterator() noexcept {}

  hash_map_iterator(const hash_map_iterator &other) noexcept:
      i_data(other.i_data),
      i_set(other.i_set),
      i_pos(other.i_pos) {}

  reference operator*() const {
    return *(i_data + i_pos); ////намана
  }
  pointer operator->() const {
    return i_data + i_pos;  ////намана
  }

  // prefix ++
  hash_map_iterator &operator++() {
    auto i = i_pos + 1;
    while (i < i_set->size() && *(i_set->begin() + i) != 1) {
      i++;
    }
    i_pos = i;
    return *this; ////вроде намана
  }
  // postfix ++
  hash_map_iterator operator++(int) {
    hash_map_iterator it(*this);
    ++(*this);
    return it;
  }

  friend bool operator==(const hash_map_iterator<ValueType> &first, const hash_map_iterator<ValueType> &second) {
    return (first.i_data + first.i_pos == second.i_data + second.i_pos);
  }
  friend bool operator!=(const hash_map_iterator<ValueType> &first, const hash_map_iterator<ValueType> &second) {
    return (first.i_data + first.i_pos != second.i_data + second.i_pos);
  }

  template<typename K, typename T,
      typename Hash,
      typename Pred,
      typename Alloc>
  friend
  class hash_map;

  template<typename ValueType1>
  friend
  class hash_map_const_iterator;

 private:
  hash_map_iterator(value_type *data, std::vector<int> *set, size_t pos) {
    i_data = data;
    i_set = set;
    i_pos = pos;
  }
  value_type *i_data;
  std::vector<int> *i_set;
  size_t i_pos;
};

template<typename ValueType1>
class hash_map_const_iterator {
// Shouldn't give non const references on value
 public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = ValueType1;
  using difference_type = std::ptrdiff_t;
  using reference = const ValueType1 &;
  using pointer = const ValueType1 *;

  hash_map_const_iterator() noexcept {}

  hash_map_const_iterator(const hash_map_const_iterator &other) noexcept :
      i_data(other.i_data),
      i_set(other.i_set),
      i_pos(other.i_pos) {}

  hash_map_const_iterator(const hash_map_iterator<ValueType1> &other) noexcept :
      i_data(other.i_data),
      i_set(other.i_set),
      i_pos(other.i_pos) {} ////намана

  reference operator*() const {
    return *(i_data + i_pos); ////намана
  }

  pointer operator->() const {
    return i_data + i_pos;  ////намана
  }

  // prefix ++
  hash_map_const_iterator &operator++() {
    auto i = i_pos + 1;
    while (i < i_set->size() && *(i_set->begin() + i) != 1) {
      i++;
    }
    i_pos = i;
    return *this; ////вроде намана
  }
  // postfix ++
  hash_map_const_iterator operator++(int) {
    hash_map_const_iterator it(*this);
    ++(*this);
    return it;  ////вроде намана
  }

  friend bool operator==(const hash_map_const_iterator<ValueType1> &first,
                         const hash_map_const_iterator<ValueType1> &second) {
    return (first.i_data + first.i_pos == second.i_data + second.i_pos);
  }
  friend bool operator!=(const hash_map_const_iterator<ValueType1> &first,
                         const hash_map_const_iterator<ValueType1> &second) {
    return (first.i_data + first.i_pos != second.i_data + second.i_pos);
  }

  template<typename K, typename T,
      typename Hash,
      typename Pred,
      typename Alloc>
  friend
  class hash_map;

 private:
  hash_map_const_iterator(value_type *data, const std::vector<int> *set, size_t pos) {
    i_data = data;
    i_set = set;
    i_pos = pos;
  }
  value_type *i_data;
  const std::vector<int> *i_set;
  size_t i_pos;
};

template<typename K, typename T,
    typename Hash,
    typename Pred,
    typename Alloc>
class hash_map {
 public:
  using key_type = K;
  using mapped_type = T;
  using hasher = Hash;
  using key_equal = Pred;
  using allocator_type = Alloc;
  using value_type = std::pair<const key_type, mapped_type>;
  using reference = value_type &;
  using const_reference = const value_type &;
  using iterator = hash_map_iterator<value_type>;
  using const_iterator = hash_map_const_iterator<value_type>;
  using size_type = std::size_t;

  /// Default constructor.
  hash_map() = default;

  /**
  *  @brief  Default constructor creates no elements.
  *  @param n  Minimal initial number of buckets.
  */
  explicit hash_map(size_type n) :
      m_allocator(),
      m_size(0),
      m_set(n, 0),
      two_size(0),
      m_hash(),
      m_key(),
      load_f(0.5){
    if (n != 0) {
      m_data = m_allocator.allocate(n);
    }
  }

  /**
   *  @brief  Builds an %hash_map from a range.
   *  @param  first  An input iterator.
   *  @param  last  An input iterator.
   *  @param  n  Minimal initial number of buckets.
   *
   *  Create an %hash_map consisting of copies of the elements from
   *  [first,last).  This is linear in N (where N is
   *  distance(first,last)).
   */
  template<typename InputIterator>
  hash_map(InputIterator first, InputIterator last, size_type n = 0):
      m_allocator(),
      m_set(n, 0),
      m_size(0),
      two_size(0),
      m_hash(),
      m_key(),
      load_f(0.5){
    if (n != 0) {
      m_data = m_allocator.allocate(n);
    }
    insert(first, last);
  }

  /// Copy constructor.
  hash_map(const hash_map &scr) :
      m_set(scr.m_set),
      m_size(scr.m_size),
      m_allocator(scr.m_allocator),
      m_hash(scr.m_hash),
      two_size(scr.two_size),
      m_key(scr.m_key),
      load_f(scr.load_f){
    if (scr.m_set.size() != 0) {
      m_data = m_allocator.allocate(scr.m_set.size());
    }
    copy_data(scr);
  }

  /// Move constructor.
  hash_map(hash_map &&scr) {
    swap(scr);
  }

  /**
   *  @brief Creates an %hash_map with no elements.
   *  @param a An allocator object.
   */
  explicit hash_map(const allocator_type &a) :
      m_allocator(a),
      m_size(0),
      m_set(0),
      two_size(0),
      m_hash(),
      m_key(),
      load_f(0.5){
  }

  /*
  *  @brief Copy constructor with allocator argument.
  * @param  uset  Input %hash_map to copy.
  * @param  a  An allocator object.
  */
  hash_map(const hash_map &umap, const allocator_type &a) :
      m_set(umap.m_set),
      m_size(umap.m_size),
      m_allocator(a),
      m_hash(umap.m_hash),
      two_size(umap.two_size),
      m_key(umap.m_key),
      load_f(umap.load_f){
    if (umap.m_set.size() != 0) {
      m_data = m_allocator.allocate(umap.m_set.size());
    }
    copy_data(umap);
  }

  /**
   *  @brief  Builds an %hash_map from an initializer_list.
   *  @param  l  An initializer_list.
   *  @param n  Minimal initial number of buckets.
   *
   *  Create an %hash_map consisting of copies of the elements in the
   *  list. This is linear in N (where N is @a l.size()).
   */
  hash_map(std::initializer_list<value_type> l, size_type n = 0) :
      m_allocator(),
      m_set(n, 0),
      m_size(0),
      two_size(0),
      m_hash(),
      m_key(),
      load_f(0.5){
    if (n != 0) {
      m_data = m_allocator.allocate(n);
    }
    insert(l);
  }

  /// Copy assignment operator.
  hash_map &operator=(const hash_map &other) {
    m_allocator.deallocate(m_data, m_set.size() * sizeof(value_type));
    m_set = other.m_set;
    m_size = other.m_size;
    m_hash = other.m_hash;
    two_size = other.two_size;
    m_key = other.m_key;
    m_data = m_allocator.allocate(other.m_set.size());
    load_f = other.load_f;
    copy_data(other);
    return *this;
  }

  /// Move assignment operator.
  hash_map &operator=(hash_map &&other) {
    if (!this->empty()) {
      m_allocator.deallocate(m_data, m_set.size() * sizeof(value_type));
    }
    m_set = std::move(other.m_set);
    m_size = other.m_size;
    m_hash = other.m_hash;
    two_size = other.two_size;
    m_key = other.m_key;
    m_data = other.m_data;
    load_f = other.load_f;
    other.m_data = nullptr;
    return *this;
  }

  /**
   *  @brief  %hash_map list assignment operator.
   *  @param  l  An initializer_list.
   *
   *  This function fills an %hash_map with copies of the elements in
   *  the initializer list @a l.
   *
   *  Note that the assignment completely changes the %hash_map and
   *  that the resulting %hash_map's size is the same as the number
   *  of elements assigned.
   */
  hash_map &operator=(std::initializer_list<value_type> l) {
    this->clear();
    insert(l);
    return *this;
  }

  ///  Returns the allocator object used by the %hash_map.
  allocator_type getAllocator() const noexcept {
    return m_allocator;  ////намана
  }

  // size and capacity:

  ///  Returns true if the %hash_map is empty.
  bool empty() const noexcept {
    return (m_size == 0); ////намана
  }

  ///  Returns the size of the %hash_map.
  size_type size() const noexcept {
    return m_size; ////намана
  }

  ///  Returns the maximum size of the %hash_map.
  size_type max_size() const noexcept {
    return SIZE_MAX;
  }

  // iterators.

  /**
   *  Returns a read/write iterator that points to the first element in the
   *  %hash_map.
   */
  iterator begin() noexcept {
    if (this->empty()) {
      return end();
    }
    auto i = 0;
    while (m_set[i] != 1) {
      i++;
    }
    iterator it(m_data, &m_set, static_cast<size_t>(i));
    return it; ////намана
  }

  /**
   *  Returns a read-only (constant) iterator that points to the first
   *  element in the %hash_map.
   */
  const_iterator begin() const noexcept {
    if (this->empty()) {
      return end();
    }
    size_t i = 0;
    while (m_set[i] != 1) {
      i++;
    }
    const_iterator it(m_data, &m_set, i);
    return it;
  }

  const_iterator cbegin() const noexcept {
    return begin();
  }

  /**
   *  Returns a read/write iterator that points one past the last element in
   *  the %hash_map.
   */
  iterator end() noexcept {
    iterator it(m_data, &m_set, m_set.size());
    return it;  ////намана
  }

  /**
   *  Returns a read-only (constant) iterator that points one past the last
   *  element in the %hash_map.
   */
  const_iterator end() const noexcept {
    const_iterator it(m_data, &m_set, m_set.size());
    return it;  ////намана
  }

  const_iterator cend() const noexcept {
    return end();
  }

  // modifiers.

  /**
   *  @brief Attempts to build and insert a std::pair into the
   *  %hash_map.
   *
   *  @param args  Arguments used to generate a new pair instance (see
   *	        std::piecewise_contruct for passing arguments to each
  *	        part of the pair constructor).
  *
  *  @return  A pair, of which the first element is an iterator that points
  *           to the possibly inserted pair, and the second is a bool that
  *           is true if the pair was actually inserted.
  *
  *  This function attempts to build and insert a (key, value) %pair into
  *  the %hash_map.
  *  An %hash_map relies on unique keys and thus a %pair is only
  *  inserted if its first element (the key) is not already present in the
  *  %hash_map.
  *
  *  Insertion requires amortized constant time.
  */
  template<typename... _Args>
  std::pair<iterator, bool> emplace(_Args &&... args){
    return insert(value_type(std::forward<_Args>(args)...));
  }

  /**
   *  @brief Attempts to build and insert a std::pair into the
   *  %hash_map.
   *
   *  @param k    Key to use for finding a possibly existing pair in
   *                the hash_map.
   *  @param args  Arguments used to generate the .second for a
   *                new pair instance.
   *
   *  @return  A pair, of which the first element is an iterator that points
   *           to the possibly inserted pair, and the second is a bool that
   *           is true if the pair was actually inserted.
   *
   *  This function attempts to build and insert a (key, value) %pair into
   *  the %hash_map.
   *  An %hash_map relies on unique keys and thus a %pair is only
   *  inserted if its first element (the key) is not already present in the
   *  %hash_map.
   *  If a %pair is not inserted, this function has no effect.
   *
   *  Insertion requires amortized constant time.
   */
  template<typename... _Args>
  std::pair<iterator, bool> try_emplace(const key_type &k, _Args &&... args) {
    auto index = get_free_cell(k);
    if (m_set[index] == 1) {
      iterator it(m_data, &m_set, index);
      return std::make_pair(it, false);
    }
    if (m_set[index] == 2) {
      m_data[index].~value_type();
      two_size--;
    }
    m_set[index] = 1;
    new(m_data + index) value_type{k, mapped_type(std::forward<_Args>(args)...)};
    m_size++;
    if (this->load_factor() >= this->max_load_factor()) {
      rehash(this->bucket_count() * 2);
      iterator it_f = this->find(k);
      index = it_f.i_pos;
    }
    iterator it(m_data, &m_set, index);
    return std::make_pair(it, true);
  }

  // move-capable overload
  template<typename... _Args>
  std::pair<iterator, bool> try_emplace(key_type &&k, _Args &&... args) {
    auto index = get_free_cell(k);
    if (m_set[index] == 1) {
      iterator it(m_data, &m_set, index);
      return std::make_pair(it, false);
    }
    if (m_set[index] == 2) {
      m_data[index].~value_type();
      two_size--;
    }
    m_set[index] = 1;
    new(m_data + index) value_type{std::move(k), mapped_type(std::forward<_Args>(args)...)};
    m_size++;
    if (this->load_factor() >= this->max_load_factor()) {
      rehash(this->bucket_count() * 2);
      iterator it_f = this->find(k);
      index = it_f.i_pos;
    }
    iterator it(m_data, &m_set, index);
    return std::make_pair(it, true);
  }

  /**
   *  @brief Attempts to insert a std::pair into the %hash_map.
   *  @param x Pair to be inserted (see std::make_pair for easy
   *	     creation of pairs).
  *
  *  @return  A pair, of which the first element is an iterator that
  *           points to the possibly inserted pair, and the second is
  *           a bool that is true if the pair was actually inserted.
  *
  *  This function attempts to insert a (key, value) %pair into the
  *  %hash_map. An %hash_map relies on unique keys and thus a
  *  %pair is only inserted if its first element (the key) is not already
  *  present in the %hash_map.
  *
  *  Insertion requires amortized constant time.
  */
  std::pair<iterator, bool> insert(const value_type &x) {
    return uinsert(x);
  }

  std::pair<iterator, bool> insert(value_type &&x) {
    return uinsert(std::move(x));
  }

  /**
   *  @brief A template function that attempts to insert a range of
   *  elements.
   *  @param  first  Iterator pointing to the start of the range to be
   *                   inserted.
   *  @param  last  Iterator pointing to the end of the range.
   *
   *  Complexity similar to that of the range constructor.
   */
  template<typename _InputIterator>
  void insert(_InputIterator first, _InputIterator last) {
    for (auto i = first; i != last; ++i) {
      uinsert(*i); ////намана
    }
  }

  /**
   *  @brief Attempts to insert a list of elements into the %hash_map.
   *  @param  l  A std::initializer_list<value_type> of elements
   *               to be inserted.
   *
   *  Complexity similar to that of the range constructor.
   */
  void insert(std::initializer_list<value_type> l) {
    insert(l.begin(), l.end());
  }

  /**
   *  @brief Attempts to insert a std::pair into the %hash_map.
   *  @param k    Key to use for finding a possibly existing pair in
   *                the map.
   *  @param obj  Argument used to generate the .second for a pair
   *                instance.
   *
   *  @return  A pair, of which the first element is an iterator that
   *           points to the possibly inserted pair, and the second is
   *           a bool that is true if the pair was actually inserted.
   *
   *  This function attempts to insert a (key, value) %pair into the
   *  %hash_map. An %hash_map relies on unique keys and thus a
   *  %pair is only inserted if its first element (the key) is not already
   *  present in the %hash_map.
   *  If the %pair was already in the %hash_map, the .second of
   *  the %pair is assigned from obj.
   *
   *  Insertion requires amortized constant time.
   */
  template<typename _Obj>
  std::pair<iterator, bool> insert_or_assign(const key_type &k, _Obj &&obj) {
    auto p = try_emplace(k, obj);
    if (!p.second) {
      m_data[p.first.i_pos].second = std::forward<_Obj>(obj);
      return p;
    } else {
      return p;
    }
  }

  // move-capable overload
  template<typename _Obj>
  std::pair<iterator, bool> insert_or_assign(key_type &&k, _Obj &&obj) {
    auto p = try_emplace(k, obj);
    if (!p.second) {
      m_data[p.first.i_pos].second = std::forward<_Obj>(obj);
      return p;
    } else {
      return p;
    }
  }

  /**
   *  @brief Erases an element from an %hash_map.
   *  @param  position  An iterator pointing to the element to be erased.
   *  @return An iterator pointing to the element immediately following
   *          @a position prior to th e element being erased. If no such
   *          element exists, end() is returned.
   *
   *  This function erases an element, pointed to by the given iterator,
   *  from an %hash_map.
   *  Note that this function only erases the element, and that if the
   *  element is itself a pointer, the pointed-to memory is not touched in
   *  any way.  Managing the pointer is the user's responsibility.
   */
  iterator erase(const_iterator position) {
    if (m_set[position.i_pos] == 1 && position.i_pos != m_set.size()) {
      m_set[position.i_pos] = 2;
      m_size--;
      two_size++;
      position++;
      iterator it(m_data, &m_set, position.i_pos);
      return it;
    } else {
      return end();
    }////намана
  }

  // LWG 2059.
  iterator erase(iterator position) {
    if (m_set[position.i_pos] == 1 && position.i_pos != m_set.size()) {
      m_set[position.i_pos] = 2;
      m_size--;
      two_size++;
      return position++;
    } else {
      return end();
    }////намана
  }

  /**
   *  @brief Erases elements according to the provided key.
   *  @param  x  Key of element to be erased.
   *  @return  The number of elements erased.
   *
   *  This function erases all the elements located by the given key from
   *  an %hash_map. For an %hash_map the result of this function
   *  can only be 0 (not present) or 1 (present).
   *  Note that this function only erases the element, and that if the
   *  element is itself a pointer, the pointed-to memory is not touched in
   *  any way.  Managing the pointer is the user's responsibility.
   */
  size_type erase(const key_type &x) {
    iterator it = this->find(x);
    if (it == end()) {
      return 0;
    } else {
      erase(it);
      return 1;  ////намана
    }
  }

  /**
   *  @brief Erases a [first,last) range of elements from an
   *  %hash_map.
   *  @param  first  Iterator pointing to the start of the range to be
   *                  erased.
   *  @param last  Iterator pointing to the end of the range to
   *                be erased.
   *  @return The iterator @a last.
   *
   *  This function erases a sequence of elements from an %hash_map.
   *  Note that this function only erases the elements, and that if
   *  the element is itself a pointer, the pointed-to memory is not touched
   *  in any way.  Managing the pointer is the user's responsibility.
   */
  iterator erase(const_iterator first, const_iterator last) {
    for (auto i = first; i != last; ++i) {
      erase(i);////намана
    }
    iterator it(m_data, &m_set, last.i_pos);
    return it;
  }

  /**
   *  Erases all elements in an %hash_map.
   *  Note that this function only erases the elements, and that if the
   *  elements themselves are pointers, the pointed-to memory is not touched
   *  in any way.  Managing the pointer is the user's responsibility.
   */
  void clear() noexcept {
    for (int i = 0; i < m_set.size(); i++) {
      if (m_set[i] != 0) {
        m_data[i].~value_type();
        m_set[i] = 0;
      }
    }
    m_size = 0;
    two_size = 0; ////намана
  }

  /**
   *  @brief  Swaps data with another %hash_map.
   *  @param  x  An %hash_map of the same element and allocator
   *  types.
   *
   *  This exchanges the elements between two %hash_map in constant
   *  time.
   *  Note that the global std::swap() function is specialized such that
   *  std::swap(m1,m2) will feed to this function.
   */
  void swap(hash_map &x) {
    std::swap(m_data, x.m_data);
    std::swap(m_set, x.m_set);
    std::swap(m_size, x.m_size);
    std::swap(m_hash, x.m_hash);
    std::swap(m_key, x.m_key);
    std::swap(load_f, x.load_f);
    std::swap(two_size, x.two_size);
  }

  template<typename _H2, typename _P2>
  void merge(hash_map<K, T, _H2, _P2, Alloc> &source) {
    for (int i = 0; i < source.m_set.size(); i++) {
      if (source.m_set[i] == 1) {
        insert(source.m_data[i]); ////намана
      }
    }
  }

  template<typename _H2, typename _P2>
  void merge(hash_map<K, T, _H2, _P2, Alloc> &&source) {
    for (int i = 0; i < source.m_set.size(); i++) {
      if (source.m_set[i] == 1) {
        insert(std::move(source.m_data[i]));////намана
      }
    }
  }

  // observers.

  ///  Returns the hash functor object with which the %hash_map was
  ///  constructed.
  Hash hash_function() const {
    return m_hash;  ////намана
  }

  ///  Returns the key comparison object with which the %hash_map was
  ///  constructed.
  Pred key_eq() const {
    return m_key;  ////намана
  }

  // lookup.

  /**
   *  @brief Tries to locate an element in an %hash_map.
   *  @param  x  Key to be located.
   *  @return  Iterator pointing to sought-after element, or end() if not
   *           found.
   *
   *  This function takes a key and tries to locate the element with which
   *  the key matches.  If successful the function returns an iterator
   *  pointing to the sought after element.  If unsuccessful it returns the
   *  past-the-end ( @c end() ) iterator.
   */
  iterator find(const key_type &x) {
    size_type index = get_free_cell(x);
    if (m_set[index] == 1) {
      iterator it(m_data, &m_set, index);
      return it;
    } else {
      return end();
    }
  }

  const_iterator find(const key_type &x) const {
    const size_type index = get_free_cell(x);
    if (m_set[index] == 1) {
      const_iterator it(m_data, &m_set, index);
      return it;
    } else {
      return end();
    }
  }

  /**
   *  @brief  Finds the number of elements.
   *  @param  x  Key to count.
   *  @return  Number of elements with specified key.
   *
   *  This function only makes sense for %unordered_multimap; for
   *  %hash_map the result will either be 0 (not present) or 1
   *  (present).
   */
  size_type count(const key_type &x) const {
    const_iterator it = this->find(x);
    if (it == end()) {
      return 0;
    } else {
      return 1;
    }
  }

  /**
   *  @brief  Finds whether an element with the given key exists.
   *  @param  x  Key of elements to be located.
   *  @return  True if there is any element with the specified key.
   */
  bool contains(const key_type &x) const {
    return this->count(x) == 1;
  }

  /**
   *  @brief  Subscript ( @c [] ) access to %hash_map data.
   *  @param  k  The key for which data should be retrieved.
   *  @return  A reference to the data of the (key,data) %pair.
   *
   *  Allows for easy lookup with the subscript ( @c [] )operator.  Returns
   *  data associated with the key specified in subscript.  If the key does
   *  not exist, a pair with that key is created using default values, which
   *  is then returned.
   *
   *  Lookup requires constant time.
   */
  mapped_type &operator[](const key_type &k) {
    iterator it = this->find(k);
    if (it == end()) {
      auto x = (k, mapped_type{});
      iterator i = insert(x);
      return m_data[i.i_pos].second;
    } else {
      return m_data[it.i_pos].second;
    }
  }

  mapped_type &operator[](key_type &&k) {
    iterator it = this->find(k);
    if (it == end()) {
      auto x = std::make_pair(std::move(k), mapped_type{});
      auto p = insert(x);
      return m_data[p.first.i_pos].second;
    } else {
      return m_data[it.i_pos].second;
    }
  }

  /**
   *  @brief  Access to %hash_map data.
   *  @param  k  The key for which data should be retrieved.
   *  @return  A reference to the data whose key is equal to @a k, if
   *           such a data is present in the %hash_map.
   *  @throw  std::out_of_range  If no such data is present.
   */
  mapped_type &at(const key_type &k) {
    iterator it = find(k);
    if(it != end()){
      return it.operator*().second;
    }
    else{
      throw std::out_of_range("No such data is present");
    }
  }

  const mapped_type &at(const key_type &k) const {
    const_iterator it = find(k);
    if(it != end()){
      return it.operator*().second;
    }
    else{
      throw std::out_of_range("No such data is present");
    }
  }

  // bucket interface.

  /// Returns the number of buckets of the %hash_map.
  size_type bucket_count() const noexcept {
    return m_set.size(); ////намана
  }

  /*
  * @brief  Returns the bucket index of a given element.
  * @param  _K  A key instance.
  * @return  The key bucket index.
  */
  size_type bucket(const key_type &_K) const {
    const_iterator it = this->find(_K);
    if (it != end()) {
      return it.i_pos; ////намана
    }
    else{
      return m_set.size();
    }
  }

  // hash policy.

  /// Returns the average number of elements per bucket.
  float load_factor() const noexcept {
    return (static_cast<float>(m_size + two_size)) / m_set.size();
  }

  /// Returns a positive number that the %hash_map tries to keep the
  /// load factor less than or equal to.
  float max_load_factor() const noexcept {
    return load_f;
  }

  /**
   *  @brief  Change the %hash_map maximum load factor.
   *  @param  z The new maximum load factor.
   */
  void max_load_factor(float z) {
    load_f = z;
    while (this->load_factor() >= this->max_load_factor()) {
      rehash(this->bucket_count() * 2);
    }
  }

  /**
   *  @brief  May rehash the %hash_map.
   *  @param  n The new number of buckets.
   *
   *  Rehash will occur only if the new number of buckets respect the
   *  %hash_map maximum load factor.
   */
  void rehash(size_type n) {
    hash_map hm(n);
    for (int i = 0; i < m_set.size(); i++) {
      if (m_set[i] == 1) {
        hm.uinsert(m_data[i]);
      }
    }
    *this = std::move(hm); ////вроде намана
  }

  /**
   *  @brief  Prepare the %hash_map for a specified number of
   *          elements.
   *  @param  n Number of elements required.
   *
   *  Same as rehash(ceil(n / max_load_factor())).
   */
  void reserve(size_type n) {
    rehash(static_cast<size_type>(n / max_load_factor()));
  }

  bool operator==(const hash_map &other) const {
    if (m_size != other.m_size) {
      return false;
    } else {
      for (int i = 0; i < other.m_set.size(); i++) {
        if (other.m_set[i] == 1) {
          const_iterator it = find(other.m_data[i].first);
          if (it == end()) {
            return false;
          }
        }
      }
    }
    return true;
  }

  ~hash_map() {
    if (m_set.size() != 0) {
      m_allocator.deallocate(m_data, m_set.size() * sizeof(value_type));
    }
  }

 private:

  template<typename _In>
  std::pair<iterator, bool> uinsert(_In&& x) {
    auto index = get_free_cell(x.first);
    if (m_set[index] == 1) {
      iterator it(m_data, &m_set, index);
      return std::make_pair(it, false);
    }
    if (m_set[index] == 2) {
      m_data[index].~value_type();
      two_size--;
    }
    m_set[index] = 1;
    new(m_data + index) value_type{std::forward<_In>(x)};
    m_size++;
    if (this->load_factor() >= this->max_load_factor()) {
      rehash(this->bucket_count() * 2);
      iterator it_f = this->find(x.first);
      index = it_f.i_pos;
    }
    iterator it(m_data, &m_set, index);
    return std::make_pair(it, true);
  }

  size_type get_free_cell(const key_type &k) const noexcept {
    const size_type index = m_hash(k) % m_set.size();
    auto i = index;
    size_type erased = m_set.size();
    bool first = false;
    while (m_set[i] != 0) {
      if (m_set[i] == 1 && m_key(m_data[i].first, k)) {
        return i;
      }
      if (m_set[i] == 2 || !first) {
        erased = i;
        first = true;
      }
      i++;
      if (i == m_set.size()) {
        i = 0;
      }
    }
    return (erased < i) ? erased : i;
  }

  void copy_data(const hash_map &scr) {
    for (auto i = 0; i < scr.m_set.size(); ++i) {
      if (scr.m_set[i] != 0) {
        new(m_data + i) value_type{scr.m_data[i].first, scr.m_data[i].second};
      }
    }
  };

  allocator_type m_allocator;
  value_type *m_data;
  size_type m_size;
  size_type two_size;
  std::vector<int> m_set;
  key_equal m_key;
  hasher m_hash;
  float load_f;
};

} // namespace fefu