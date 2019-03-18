#include <functional>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <list>

std::out_of_range OUT_OF_RANGE("");
template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap
{
	const static size_t MAX_LOAD_FACTOR = 2;
	const static size_t START_OBR_LOAD_FACTOR = 2;
using Pair = std::pair<const KeyType, ValueType>;
using List = std::list<Pair>;
public:
	typedef typename List::iterator iterator;
	typedef typename List::const_iterator const_iterator;
private:
	typedef typename std::vector<iterator>::iterator sub_iterator;
	typedef typename std::vector<iterator>::const_iterator const_sub_iterator;
	List values;
	std::vector<std::vector<iterator>> table;
	Hash hash;
	size_t current_size;

	size_t get_pos(const KeyType &key) const
	{
		size_t x = hash(key);
		return x % table.size();
	}

	void rebuild()
	{
		for (auto fst = values.begin(); fst != values.end(); ++fst)
		{
			table[get_pos(fst->first)].push_back(fst);
		}
	}

	std::pair<size_t, sub_iterator> find_in_table(const KeyType &key)
	{
		size_t row = get_pos(key);
		for (sub_iterator j = table[row].begin(); j != table[row].end(); ++j)
			if ((*j)->first == key)
				return {row, j};
		return {-1, table[0].end()};
	}

	std::pair<size_t, const_sub_iterator> find_in_table(const KeyType &key) const
	{
		size_t row = get_pos(key);
		for (const_sub_iterator j = table[row].begin(); j != table[row].end(); ++j)
			if ((*j)->first == key)
				return {row, j};
		return {-1, table[0].end()};
	}

	void unsafe_insert(const Pair &p)
	{
		values.push_front(p);
		table[get_pos(p.first)].push_back(values.begin());
		current_size++;
		if (current_size >= table.size() * MAX_LOAD_FACTOR)
		{
			table.clear();
			table.resize(current_size * START_OBR_LOAD_FACTOR);
			rebuild();
		}
	}
public:

	HashMap &operator =(const HashMap &other)
	{
		if (&other != this)
		{
			this->clear();
			values = List(other.values.begin(), other.values.end());
			hash = other.hash;
			current_size = other.current_size;
			table.resize(std::max(table.size(), current_size * START_OBR_LOAD_FACTOR + 1ul));
			rebuild();
		}
		return (*this);
	}

	void insert(const Pair &p)
	{
		auto x = find_in_table(p.first);
		if (x.first == (size_t)-1)
		{
			unsafe_insert(p);
		}
	}

	HashMap(const HashMap &other)
		: values(other.values)
		, table(other.size() * START_OBR_LOAD_FACTOR + 1ul)
		, hash(other.hash)
		, current_size(other.size())
	{
		rebuild();
	}

	HashMap(Hash _hash = Hash())
		: values()
		, table(1ul)
		, hash(_hash)
		, current_size(0)
	{}

	template<typename Iter>
	HashMap(Iter first, Iter last, Hash _hash = Hash())
		: values()
		, table(1ul)
		, hash(_hash)
		, current_size(0)
	{
		for (; first != last; first++)
		{
			insert(*first);
		}
	}

	HashMap(const std::initializer_list<Pair> &ini_list, Hash _hash = Hash())
		: HashMap(ini_list.begin(), ini_list.end(), _hash)
	{}

	iterator begin()
	{
		return values.begin();
	}

	iterator end()
	{
		return values.end();
	}

	const_iterator begin() const
	{
		return values.cbegin();
	}

	const_iterator end() const
	{
		return values.cend();
	}
	
	size_t size() const
	{
		return current_size;
	}

	bool empty() const
	{
		return current_size == 0;
	}

	Hash hash_function() const
	{
		return hash;
	}

	iterator find(const KeyType &key)
	{
		auto pos = find_in_table(key);
		if (pos.first == (size_t)-1)
		{
			return values.end();
		}
		return *pos.second;
	}

	const_iterator find(const KeyType &key) const
	{
		auto pos = find_in_table(key);
		if (pos.first == (size_t)-1)
		{
			return values.end();
		}
		return *pos.second;
	}

	void erase(const KeyType &key)
	{
		auto pos = find_in_table(key);
		if (pos.first == (size_t)-1)
			return;
		current_size--;
		values.erase(*pos.second);
		table[pos.first].erase(pos.second);
	}

	ValueType &operator [](const KeyType &key)
	{
		auto pos = find_in_table(key);
		if (pos.first == (size_t)-1)
		{
			unsafe_insert(Pair(key, ValueType()));
			return values.begin()->second;
		}
		return (*pos.second)->second;
	}

	const ValueType &at(const KeyType &key) const
	{
		auto pos = find_in_table(key);
		if (pos.first == (size_t)-1)
			throw OUT_OF_RANGE;
		return (*pos.second)->second;
	}

	void clear()
	{
		for (auto &i: values)
		{
			table[get_pos(i.first)].clear();
		}
		values.clear();
		current_size = 0;
	}
};
