#pragma once
#include <cstdlib>
#include <cstdint>
#include <exception>
#include <string>

template<typename K, typename T>
class Linhash2 {
private:
	typedef uint64_t Linhash2_Index;

	const unsigned int BASE_CAPACITY = 4;
	const unsigned int MAX_MEMBERS = 4;
	const double OVER_LOAD = 0.74;
	const double UNDER_LOAD = 0.25;

	struct hashListElement_s
	{
		hashListElement_s *next;
		K key;
		T data;
	};
	struct hashHeadElement_s
	{
		hashListElement_s *first;
		size_t size;
	};

	Linhash2_Index usedSize;
	Linhash2_Index capacity;
	Linhash2_Index usedCapacity;
	Linhash2_Index split;
	Linhash2_Index level;
	T* lastData;

	hashHeadElement_s* table;

	void init()
	{
		capacity = BASE_CAPACITY;
		table = (hashHeadElement_s*)calloc(BASE_CAPACITY, sizeof(hashHeadElement_s));
		if (!table) throw std::exception("Exception in Linhash2.init(): failure to calloc.");
		level = BASE_CAPACITY;
		split = 0;
		usedSize = 0;
		usedCapacity = capacity;
		lastData = nullptr;
	}

	void clean()
	{
		Linhash2_Index i;
		hashListElement_s *curEntry;
		hashListElement_s *nextEntry;
		for (i = 0; i < usedCapacity; i++)
		{
			if (curEntry = table[i].first)
			{
				do
				{
					nextEntry = curEntry->next;
					delete curEntry;
				} while (curEntry = nextEntry);
				table[i].first = nullptr;
				table[i].size = 0;
			}
		}
	}

	Linhash2_Index getSplitHash(Linhash2_Index _loc)
	{
		if ((_loc % level) < split)
		{
			return _loc % (level * 2);
		}
		else
		{
			return _loc % level;
		}
	}

	static Linhash2_Index hash(int _key)
	{
		Linhash2_Index hash_out;
		hash_out = ((_key >> 16) ^ _key) * 0x45d9f3b;
		hash_out = ((hash_out >> 16) ^ hash_out) * 0x45d9f3b;
		hash_out = ((hash_out >> 16) ^ hash_out);
		return hash_out;
	}

	static Linhash2_Index hash(const std::string& _key) 
	{
		Linhash2_Index hashV;
		char cur_char;
		unsigned int str_pos;

		hashV = 0;
		str_pos = 0;
		while (cur_char = _key[str_pos++])
		{
			hashV = cur_char + (hashV << 6) + (hashV << 16) - hashV;
		}
		return hashV;
	}

	static Linhash2_Index hash(void* key)
	{
		return hash((int)key);
	}

	void up_splitEntry()
	{
		hashListElement_s *curEntry;
		hashListElement_s *oldEntry;
		hashListElement_s *nextEntry;
		Linhash2_Index nextLevel = level * 2;
		Linhash2_Index newPos;

		++usedCapacity;
		if (usedCapacity > capacity)
		{
			capacity *= 2;
			table = (hashHeadElement_s*)realloc(table, sizeof(hashHeadElement_s) * capacity);
			if (!table) throw std::exception("Exception in Linhash2.up_splitEntry(): failure to realloc.");
		}
		table[usedCapacity - 1].first = nullptr;

		curEntry = table[split].first;

		if (curEntry)
		{
			oldEntry = nullptr;
			do
			{
				nextEntry = curEntry->next;
				if ((newPos = (hash(curEntry->key) % nextLevel)) != split)
				{

					table[split].size--;
					table[newPos].size++;

					if (oldEntry)
					{
						oldEntry->next = curEntry->next;
					}
					else
					{
						table[split].first = curEntry->next;
					}

					curEntry->next = table[newPos].first;
					table[newPos].first = curEntry;
				}
				else
				{
					oldEntry = curEntry;
				}
			} while (curEntry = nextEntry);
		}

		if (++split >= level)
		{
			level = nextLevel;
			split = 0;
		}
	}

	void down_splitEntry()
	{
		hashListElement_s *curEntry;
		hashListElement_s *nextEntry;
		Linhash2_Index newPos;

		if (usedCapacity <= BASE_CAPACITY) return;

		if (--split < 0)
		{
			level = (Linhash2_Index)(level * 0.5);
			split = level - 1;
		}

		curEntry = table[usedCapacity - 1].first;
		if (curEntry != nullptr)
		{
			newPos = hash(curEntry->key) % level;

			do
			{
				nextEntry = curEntry->next;

				curEntry->next = table[newPos].first;
				table[newPos].first = curEntry;

				table[newPos].size++;

			} while (curEntry = nextEntry);
		}
		usedCapacity--;
	}

	bool over_load()
	{
		if ((usedSize / (double)(usedCapacity * MAX_MEMBERS)) > OVER_LOAD) return true;
		return false;
	}

	bool under_load()
	{
		if ((usedSize / (double)(usedCapacity * MAX_MEMBERS)) < UNDER_LOAD) return true;
		return false;
	}
	public:
		Linhash2()
		{
			init();
		}

		~Linhash2()
		{
			clean();
			free(table);
		}

		void flush()
		{
			clean();
			level = BASE_CAPACITY;
			split = 0;
			usedSize = 0;
			usedCapacity = capacity;
		}

		void insert(const K& _key, const T& _data)
		{
			Linhash2_Index keyloc;
			hashListElement_s *hashData;

			keyloc = getSplitHash(hash(_key));

			hashData = new hashListElement_s;

			hashData->data = _data;
			hashData->key = _key;

			hashData->next = table[keyloc].first;
			table[keyloc].first = hashData;

			table[keyloc].size++;
			usedSize++;

			if (over_load()) up_splitEntry();
		}

		void remove(const K& _key)
		{
			Linhash2_Index keyloc;
			hashListElement_s *curEntry;
			hashListElement_s *nextEntry;
			hashListElement_s *oldEntry;

			keyloc = getSplitHash(hash(_key));

			curEntry = table[keyloc].first;
			if (curEntry)
			{
				oldEntry = nullptr;
				do
				{
					nextEntry = curEntry->next;
					if (curEntry->key == _key)
					{
						if (!oldEntry)
						{
							table[keyloc].first = nextEntry;
						}
						else
						{
							oldEntry->next = nextEntry;
						}

						delete curEntry;
						table[keyloc].size--;
						usedSize--;

						if (under_load()) down_splitEntry();

						return;
					}
					oldEntry = curEntry;
					curEntry = nextEntry;
				} while (curEntry);
			}
		}

		bool exist(const K& _key)
		{
			Linhash2_Index keyloc;
			hashListElement_s *curEntry;
			hashListElement_s *nextEntry;

			keyloc = getSplitHash(hash(_key));

			curEntry = table[keyloc].first;
			if (curEntry)
			{
				do
				{
					nextEntry = curEntry->next;
					if (curEntry->key == _key)
					{
						lastData = &(curEntry->data);
						return true;
					}
				} while (curEntry = nextEntry);
			}
			return false;
		}

		T& retrieve()
		{
			return *lastData;
		}

		T& retrieve(const K& _key)
		{
			Linhash2_Index keyloc;
			hashListElement_s *curEntry;
			hashListElement_s *nextEntry;

			keyloc = getSplitHash(hash(_key));

			curEntry = table[keyloc].first;
			if (curEntry)
			{
				do
				{
					nextEntry = curEntry->next;
					if (curEntry->key == _key) return curEntry->data;
				} while (curEntry = nextEntry);
			}
			throw std::exception("Exception in Linhash2.retrieve(): key does not exist.")
		}
};