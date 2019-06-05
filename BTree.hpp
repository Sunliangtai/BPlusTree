//
// Created by 郑文鑫 on 2019-03-09.
//

#include "utility.hpp"
#include <functional>
#include <cstddef>
#include <fstream>
#include "exception.hpp"
namespace sjtu {
	template <class Key, class Value, class Compare = std::less<Key> >
	class BTree {
		//叶子节点数组大小
		static const int sizeofleaf = (4096 - 4 * sizeof(long) - sizeof(int)) / sizeof(pair<Key, Value>);
		//中间节点数组大小
		static const int sizeofinterval = (4096 - 2 * sizeof(long) - sizeof(int) - sizeof(bool)) / (sizeof(Key) + sizeof(long));
	private:
		struct leaf_node
		{
			long offset;
			long next;
			long father;
			long prev;
			int nowsize;
			pair<Key,Value> *data=new pair<Key,Value>[sizeofleaf];
			leaf_node() {
				offset = next = father = prev = 0x0;
				nowsize = -1;
			}
		};//叶子节点
		struct p2
		{
			Key k;
			long offset;
			p2() { offset = 0x0; }
		};
		struct interval_node
		{
			long offset;
			long father;
			bool judge;
			int nowsize;
			p2 *data=new p2[sizeofinterval];
			interval_node()
			{
				offset = father = 0x0;
				nowsize = -1;
				judge = false;
			}
		};//内部节点
		struct save
		{
			long root;
			long first_leaf;
			long now_for_change;
			save() { root = first_leaf = now_for_change = 0x0; }
		};//保存当前读取信息
		// Your private members go here
	public:
		mutable std::fstream foi;
		save save_data;
		typedef pair<const Key, Value> value_type;
		//打开文件
		void open_file()
		{
			foi.open("MySQL", std::ios::in|std::ios::out|std::ios::binary);
			if (!foi) throw "DS is too difficult.Fuck it!";
			foi.seekp(0, foi.beg);
			save_data.now_for_change = (long)(foi.tellp()) + sizeof(save);
			foi.write(reinterpret_cast<char*>(&save_data), sizeof(save));
		}
		//关闭文件
		void close_file()
		{
			foi.close();
		}
		class const_iterator;
		class iterator {
		private:
			// Your private members go here
		public:
			leaf_node *ptr;
			int position;
			iterator() {
				ptr = nullptr;
				position = -1;
				// TODO Default Constructor
			}
			iterator(const iterator& other) {
				ptr = other.ptr;
				position = other.position;
				// TODO Copy Constructor
			}
			// Return a new iterator which points to the n-next elements
			iterator operator++(int) {
				iterator save = this;
				if (position < ptr->nowsize) position++;
				else {
					if (ptr->next == 0x0) ptr = nullptr;
					else ptr = foi.read(ptr->next, sizeof(leaf_node));
					position = 0;
				}
				return save;
				// Todo iterator++
			}
			iterator& operator++() {
				if (position < ptr->nowsize) position++;
				else {
					if (ptr->next == 0x0) ptr = nullptr;
					else ptr = foi.read(ptr->next, sizeof(leaf_node));
					position = 0;
				}
				return *this;
				// Todo ++iterator
			}
			iterator operator--(int) {
				iterator save = this;
				if (position != 0) position--;
				else {
					if (ptr->prev == 0x0){
						ptr = nullptr;
						position = -1;
					}
					else {
						ptr = foi.read(ptr->prev, sizeof(leaf_node));
						position = ptr->nowsize;
					}
				}
				return save;
				// Todo iterator--
			}
			iterator& operator--() {
				if (position != 0) position--;
				else {
					if (ptr->prev == 0x0) {
						ptr = nullptr;
						position = -1;
					}
					else {
						ptr = foi.read(ptr->prev, sizeof(leaf_node));
						position = ptr->nowsize;
					}
				}
				return *this;
				// Todo --iterator
			}
			//
			bool modify(const Value& value)
			{
				ptr->data[ptr->position].Value = value;
				foi.seekp(ptr->offset, foi.beg);
				foi.write(ptr, sizeof(leaf_node));
				return true;
			}
			// Overloaded of operator '==' and '!='
			// Check whether the iterators are same
			bool operator==(const iterator& rhs) const {
				// Todo operator ==
				return (ptr->offset == rhs.ptr->offset) && (position == rhs.position);
			}
			bool operator==(const const_iterator& rhs) const {
				// Todo operator ==
				return (ptr->offset == rhs.ptr->offset) && (position == rhs.position);
			}
			bool operator!=(const iterator& rhs) const {
				// Todo operator !=
				return !(*this == rhs);
			}
			bool operator!=(const const_iterator& rhs) const {
				// Todo operator !=
				return !(*this == rhs);
			}
		};
		class const_iterator {
			// it should has similar member method as iterator.
			//  and it should be able to construct from an iterator.
		private:
			// Your private members go here
			leaf_node *ptr;
			int position;
		public:
			const_iterator() {
				// TODO
				ptr = nullptr;
				position = -1;
			}
			const_iterator(const const_iterator& other) {
				// TODO
				ptr = other.ptr;
				position = other.position;
			}
			const_iterator(const iterator& other) {
				// TODO
				ptr = other.ptr;
				position = other.position;
			}
			bool operator==(const iterator& rhs) const {
				// Todo operator ==
				return (ptr->offset == rhs.ptr->offset) && (position == rhs.position);
			}
			bool operator==(const const_iterator& rhs) const {
				// Todo operator ==
				return (ptr->offset == rhs.ptr->offset) && (position == rhs.position);
			}
			bool operator!=(const iterator& rhs) const {
				// Todo operator !=
				return !(*this == rhs);
			}
			bool operator!=(const const_iterator& rhs) const {
				// Todo operator !=
				return !(*this == rhs);
			}
			// And other methods in iterator, please fill by yourself.
		};
		// Default Constructor and Copy Constructor
		BTree() {
			// Todo Default
			open_file();
		}
		BTree(const BTree& other) {
			// Todo Copy
			foi = other.foi;
			save_data = other.save_data;
		}
		BTree& operator=(const BTree& other) {
			// Todo Assignment
			foi = other.foi;
			save_data = other.save_data;
			return *this;
		}
		~BTree() {
			// Todo Destructor
			close_file();
			save_data.root = save_data.first_leaf = save_data.now_for_change = 0x0;
		}
		/**
		 * Finds an element with key equivalent to key.
		 * key value of the element to search for.
		 * Iterator to an element with key equivalent to key.
		 *   If no such element is found, past-the-end (see end()) iterator is
		 * returned.
		 */
		iterator find(const Key& key) {
			iterator tmp;
			if (save_data.root == save_data.first_leaf)
			{
				foi.seekg(save_data.root, foi.beg);
				foi.read((char*)(&tmp.ptr), sizeof(leaf_node));
				for (int i = 0; i <= tmp.ptr->nowsize; i++)
				{
					if (!(tmp.ptr->data[i].first > key || tmp.ptr->data[i].first < key))
					{
						tmp.position = i;
						return tmp;
					}
				}
				tmp = end();
				return tmp;
			}
			else {
				interval_node *t;
				foi.seekg(save_data.root, foi.beg);
				foi.read((char*)(&t), sizeof(interval_node));
				while (!t->judge)
				{
					for (int i = 0; i <= t->nowsize; i++)
					{
						if (!(t->data[i].k < key))
						{
							foi.seekg(t->data[i].offset,foi.beg);
							foi.read((char*)(&t), sizeof(interval_node));
							continue;
						}
					}
					tmp = end();
					return tmp;
				}
				for (int i = 0; i <= t->nowsize; i++)
				{
					if (!(t->data[i].k < key))
					{
						foi.seekg(t->data[i].offset, foi.beg);
						foi.read((char*)(&tmp.ptr), sizeof(leaf_node));
						break;
					}
				}
				for (int i = 0; i <= tmp.ptr->nowsize; i++)
				{
					if (!(tmp.ptr->data[i].first > key || tmp.ptr->data[i].first < key))
					{
						tmp.position = i;
						return tmp;
					}
				}
				tmp = end();
				return tmp;
			}
		}
		const_iterator find(const Key& key) const {
			const_iterator *tmp;
			if (save_data.root == save_data.first_leaf)
			{
				tmp->ptr = foi.read(save_data.root, sizeof(leaf_node));
				for (int i = 0; i <= tmp->ptr->nowsize; i++)
				{
					if (!(tmp->ptr->data[i].Key > key || tmp->ptr->data[i].Key < key))
					{
						tmp->position = i;
						return tmp;
					}
				}
				tmp = end();
				return tmp;
			}
			else {
				interval_node *t = foi.read(save_data.root, sizeof(interval_node));
				while (!t->judge)
				{
					for (int i = 0; i <= t->nowsize; i++)
					{
						if (!(t->data[i].k < key))
						{
							t = foi.read(t->data[i].offset, sizeof(interval_node));
							continue;
						}
					}
					tmp = end();
					return tmp;
				}
				for (int i = 0; i <= t->nowsize; i++)
				{
					if (!(t->data[i].k < key))
					{
						tmp->ptr = foi.read(t->data[i].offset, sizeof(leaf_node));
						break;
					}
				}
				for (int i = 0; i <= tmp->ptr->nowsize; i++)
				{
					if (!(tmp->ptr->data[i].Key > key || tmp->ptr->data[i].Key < key))
					{
						tmp->position = i;
						return tmp;
					}
				}
				tmp = end();
				return tmp;
			}
		}
		//分裂叶节点
		void split_leaf(leaf_node *&t)
		{
			leaf_node *tmp1=new leaf_node, *tmp2=new leaf_node;
			for (int i = 0; i < t->nowsize / 2; i++)
			{
				tmp1->data[++tmp1->nowsize].first = t->data[i].first;
				tmp1->data[tmp1->nowsize].second = t->data[i].second;
			}
			for (int i = t->nowsize / 2; i <= t->nowsize; i++)
			{
				tmp2->data[++tmp2->nowsize].first = t->data[i].first;
				tmp2->data[tmp2->nowsize].second = t->data[i].second;
			}
			tmp1->offset = t->offset;
			tmp2->offset = save_data.now_for_change;
			save_data.now_for_change += sizeof(leaf_node);
			foi.seekp(0, foi.beg);
			foi.write(reinterpret_cast<char*>(&save_data), sizeof(save));
			tmp1->next = tmp2->offset;
			tmp2->prev = tmp1->offset;
			if (t->father != 0x0)
			{
				tmp1->father = tmp2->father = t->father;
				interval_node *tmp;
				foi.seekg(t->father, foi.beg);
				foi.read((char*)(&tmp), sizeof(interval_node));
				int i;
				for (i = 0; i <= tmp->nowsize; i++)
				{
					if (tmp->data[i].offset == t->offset) break;
				}
				tmp->data[i].k = tmp1->data[tmp1->nowsize].first;
				tmp->nowsize++;
				for (int j = tmp->nowsize; j > i + 1; j--)
				{
					tmp->data[j] = tmp->data[j - 1];
				}
				tmp->data[i + 1].k = tmp2->data[tmp2->nowsize].first;
				tmp->data[i + 1].offset = tmp2->offset;
				foi.seekp(tmp->offset, foi.beg);
				foi.write(reinterpret_cast<char*>(&tmp), sizeof(interval_node));
				split_interval(tmp);
				foi.seekp(tmp1->offset, foi.beg);
				foi.write(reinterpret_cast<char*>(&tmp1), sizeof(leaf_node));
				foi.seekp(tmp2->offset, foi.beg);
				foi.write(reinterpret_cast<char*>(&tmp2), sizeof(leaf_node));
			}
			else {
				interval_node *tmp=new interval_node;
				tmp->offset = save_data.now_for_change;
				save_data.now_for_change += sizeof(interval_node);
				foi.seekp(0, foi.beg);
				foi.write(reinterpret_cast<char*>(&save_data), sizeof(save));
				tmp->judge = true;
				tmp1->father = tmp2->father = tmp->offset;
				tmp->data[++tmp->nowsize].k = tmp1->data[tmp1->nowsize].first;
				tmp->data[tmp->nowsize].offset = tmp1->offset;
				tmp->data[++tmp->nowsize].k = tmp2->data[tmp2->nowsize].first;
				tmp->data[tmp->nowsize].offset = tmp2->offset;
				foi.seekp(tmp->offset, foi.beg);
				foi.write(reinterpret_cast<char*>(&tmp), sizeof(interval_node));
				foi.seekp(tmp1->offset, foi.beg);
				foi.write(reinterpret_cast<char*>(&tmp1), sizeof(leaf_node));
				foi.seekp(tmp2->offset, foi.beg);
				foi.write(reinterpret_cast<char*>(&tmp2), sizeof(leaf_node));
				save_data.root = tmp->offset;
				foi.seekp(0, foi.beg);
				foi.write(reinterpret_cast<char*>(&save_data), sizeof(save));
			}
		}
		//分裂内部节点
		void split_interval(interval_node *&t)
		{
			if (t->nowsize != sizeofinterval - 1) return;
			else {
				interval_node *tmp1=new interval_node, *tmp2=new interval_node;
				for (int i = 0; i < t->nowsize / 2; i++)
				{
					tmp1->data[++tmp1->nowsize] = t->data[i];
				}
				for (int i = t->nowsize / 2; i <= t->nowsize; i++)
				{
					tmp2->data[++tmp2->nowsize] = t->data[i];
				}
				tmp1->offset = t->offset;
				tmp2->offset = save_data.now_for_change;
				save_data.now_for_change += sizeof(interval_node);
				foi.seekp(0, foi.beg);
				foi.write(reinterpret_cast<char*>(&save_data), sizeof(save));
				tmp1->judge = tmp2->judge = t->judge;
				if (t->father != 0x0)
				{
					tmp1->father = tmp2->father = t->father;
					interval_node *tmp;
					foi.seekg(tmp1->father, foi.beg);
					foi.read((char*)(&tmp), sizeof(interval_node));
					int i;
					for (i = 0; i <= tmp->nowsize; i++)
					{
						if (tmp->data[i].offset == t->offset) break;
					}
					tmp->data[i].k = tmp1->data[tmp1->nowsize].k;
					tmp->nowsize++;
					for (int j = tmp->nowsize; j > i + 1; j--)
					{
						tmp->data[j] = tmp->data[j - 1];
					}
					tmp->data[i + 1].k = tmp2->data[tmp2->nowsize].k;
					tmp->data[i + 1].offset = tmp2->offset;
					foi.seekp(tmp->offset, foi.beg);
					foi.write(reinterpret_cast<char*>(&tmp), sizeof(interval_node));
					split_interval(tmp);
					foi.seekp(tmp1->offset, foi.beg);
					foi.write(reinterpret_cast<char*>(&tmp1), sizeof(interval_node));
					foi.seekp(tmp2->offset, foi.beg);
					foi.write(reinterpret_cast<char*>(&tmp2), sizeof(interval_node));
				}
				else {
					interval_node *tmp=new interval_node;
					tmp->offset = save_data.now_for_change;
					tmp1->father = tmp2->father = tmp->offset;
					tmp->judge = false;
					tmp->data[++tmp->nowsize].k = tmp1->data[tmp1->nowsize].k;
					tmp->data[tmp->nowsize].offset = tmp1->offset;
					tmp->data[++tmp->nowsize].k = tmp2->data[tmp2->nowsize].k;
					tmp->data[tmp->nowsize].offset = tmp2->offset;
					save_data.root = tmp->offset;
					save_data.now_for_change += sizeof(interval_node);
					foi.seekp(0, foi.beg);
					foi.write(reinterpret_cast<char*>(&save_data), sizeof(save));
					foi.seekp(tmp->offset, foi.beg);
					foi.write(reinterpret_cast<char*>(&tmp), sizeof(interval_node));
					foi.seekp(tmp1->offset, foi.beg);
					foi.write(reinterpret_cast<char*>(&tmp1), sizeof(interval_node));
					foi.seekp(tmp2->offset, foi.beg);
					foi.write(reinterpret_cast<char*>(&tmp2), sizeof(interval_node));
				}
			}
		}
		// Insert: Insert certain Key-Value into the database
		// Return a pair, the first of the pair is the iterator point to the new
		// element, the second of the pair is Success if it is successfully inserted
		pair<iterator, OperationResult> insert(const Key& key, const Value& value) {
			// TODO insert function
			if (save_data.root == 0x0)
			{
				leaf_node *tmp=new leaf_node;
				tmp->offset = save_data.now_for_change;
				save_data.root = tmp->offset;
				save_data.first_leaf = tmp->offset;
				save_data.now_for_change += sizeof(leaf_node);
				tmp->nowsize++;
				tmp->data[tmp->nowsize].first = key;
				tmp->data[tmp->nowsize].second = value;
				foi.seekp(tmp->offset, foi.beg);
				foi.write(reinterpret_cast<char*>(&tmp), sizeof(leaf_node));
				foi.seekp(0, foi.beg);
				foi.write(reinterpret_cast<char*>(&save_data), sizeof(save));
				iterator res;
				res.ptr = tmp;
				res.position = 0;
				return pair<iterator, OperationResult>(res,Success);
			}
			else {
				iterator tmp;
				if (save_data.first_leaf == save_data.root)
				{
					foi.seekg(save_data.root, foi.beg);
					foi.read((char*)(&tmp.ptr), sizeof(leaf_node));
					if (tmp.ptr->nowsize != sizeofleaf - 2)
					{
						tmp.ptr->data[++tmp.ptr->nowsize].first = key;
						tmp.ptr->data[tmp.ptr->nowsize].second = value;
						int i;
						for (i = tmp.ptr->nowsize; i > 0; i--)
						{
							if (tmp.ptr->data[i - 1].first > key)
							{
								tmp.ptr->data[i].first = tmp.ptr->data[i - 1].first;
								tmp.ptr->data[i].second = tmp.ptr->data[i - 1].second;
							}
						}
						tmp.ptr->data[i].first = key;
						tmp.ptr->data[i].second = value;
						tmp.position = i;
						foi.seekp(tmp.ptr->offset, foi.beg);
						foi.write(reinterpret_cast<char*>(&tmp.ptr), sizeof(leaf_node));
						return pair<iterator, OperationResult>(tmp,Success);
					}
					else {
						tmp.ptr->data[++tmp.ptr->nowsize].first = key;
						tmp.ptr->data[tmp.ptr->nowsize].second = value;
						int i;
						for (i = tmp.ptr->nowsize; i > 0; i--)
						{
							if (tmp.ptr->data[i - 1].first > key)
							{
								tmp.ptr->data[i].first = tmp.ptr->data[i - 1].first;
								tmp.ptr->data[i].second = tmp.ptr->data[i - 1].second;
							}
						}
						tmp.ptr->data[i].first = key;
						tmp.ptr->data[i].second = value;
						split_leaf(tmp.ptr);
						return pair<iterator, OperationResult>(find(key),Success);
					}
				}
				else {
					foi.seekg(save_data.root, foi.beg);
					interval_node *t;
					foi.read((char*)(&t), sizeof(interval_node));
					if (key > t->data[t->nowsize].k)
					{
						t->data[t->nowsize].k = key;
						foi.seekp(t->offset, foi.beg);
						foi.write(reinterpret_cast<char*>(&t), sizeof(interval_node));
						while (!t->judge)
						{
							foi.seekg(t->data[t->nowsize].offset, foi.beg);
							foi.read((char*)(&t), sizeof(interval_node));
							t->data[t->nowsize].k = key;
							foi.seekp(t->offset, foi.beg);
							foi.write(reinterpret_cast<char*>(&t), sizeof(interval_node));
						}
						foi.seekg(t->data[t->nowsize].offset, foi.beg);
						foi.read((char*)(&tmp.ptr), sizeof(leaf_node));
						tmp.ptr->data[++tmp.ptr->nowsize].first = key;
						tmp.ptr->data[tmp.ptr->nowsize].second = value;
						if (tmp.ptr->nowsize == sizeofleaf - 1)
						{
							split_leaf(tmp.ptr);
							return pair<iterator,OperationResult>(find(key),Success);
						}
						else {
							foi.seekp(tmp.ptr->offset,foi.beg);
							foi.write(reinterpret_cast<char*>(&tmp.ptr), sizeof(leaf_node));
							tmp.position = tmp.ptr->nowsize;
							return pair<iterator,OperationResult>(tmp,Success);
						}
					}
					else {
						while (!t->judge)
						{
							for (int i = 0; i <= t->nowsize; i++)
							{
								if (!(key > t->data[i].k))
								{
									foi.seekg(t->data[i].offset, foi.beg);
									foi.read((char*)(&t), sizeof(interval_node));
									break;
								}
							}
						}
						for (int i = 0; i <= t->nowsize; i++)
						{
							if (!(key > t->data[i].k))
							{
								foi.seekg(t->data[i].offset, foi.beg);
								foi.read((char*)(&tmp.ptr), sizeof(leaf_node));
							}
						}
						tmp.ptr->data[++tmp.ptr->nowsize].first = key;
						tmp.ptr->data[tmp.ptr->nowsize].second = value;
						int i;
						for (i = tmp.ptr->nowsize; i > 0; i--)
						{
							if (tmp.ptr->data[i - 1].first > key)
							{
								tmp.ptr->data[i].first = tmp.ptr->data[i - 1].first;
								tmp.ptr->data[i].second = tmp.ptr->data[i - 1].second;
							}
						}
						tmp.ptr->data[i].first = key;
						tmp.ptr->data[i].second = value;
						if (tmp.ptr->nowsize == sizeofleaf - 1)
						{
							split_leaf(tmp.ptr);
							return pair<iterator,OperationResult>(find(key),Success);
						}
						else {
							tmp.position = i;
							foi.seekp(tmp.ptr->offset, foi.beg);
							foi.write(reinterpret_cast<char*>(&tmp.ptr), sizeof(leaf_node));
							return pair<iterator,OperationResult>(tmp,Success);
						}
					}
				}
			}
		}
		// Erase: Erase the Key-Value
		// Return Success if it is successfully erased
		// Return Fail if the key doesn't exist in the database
		OperationResult erase(const Key& key) {
			// TODO erase function
			return Fail;  // If you can't finish erase part, just remaining here.
		}
		// Overloaded of []
		// Access Specified Element
		// return a reference to the first value that is mapped to a key equivalent to
		// key. Perform an insertion if such key does not exist.
		Value at(const Key& key)
		{
			iterator tmp = find(key);
			if (tmp != end()) return tmp.ptr->data[tmp.position].second;
			throw "DS is too difficult.Fuck it!";
		}
		iterator begin() {
			iterator *tmp;
			tmp->ptr = foi.read(save_data.first_leaf, sizeof(leaf_node));
			tmp->position = 0;
			return tmp;
		}
		const_iterator cbegin() const {
			mutable const_iterator *tmp;
			tmp->ptr = foi.read(save_data.first_leaf, sizeof(leaf_node));
			tmp->position = 0;
			return tmp;
		}
		// Return a iterator to the end(the next element after the last)
		iterator end() {
			iterator tmp;
			foi.seekg(save_data.first_leaf, foi.beg);
			foi.read((char*)(&tmp.ptr), sizeof(leaf_node));
			while (tmp.ptr->next != 0x0)
			{
				foi.seekg(tmp.ptr->next, foi.beg);
				foi.read((char*)(&tmp.ptr), sizeof(leaf_node));
			}
			tmp.position = tmp.ptr->nowsize + 1;
			return tmp;
		}
		const_iterator cend() const {
			mutable const_iterator *tmp;
			tmp->ptr = foi.read(save_data.first_leaf, sizeof(leaf_node));
			while (tmp->ptr->next != 0x0)
			{
				tmp->ptr = foi.read(tmp->ptr->next, sizeof(leaf_node));
			}
			tmp->position = tmp->ptr->nowsize + 1;
			return tmp;
		}
		// Check whether this BTree is empty
		bool empty() const {
			return save_data.root == 0x0;
		}
		// Return the number of <K,V> pairs
		size_t size() const {
			int cnt = 0;
			leaf_node *tmp;
			if (save_data.first_leaf == 0) return 0;
			foi.seekg(save_data.first_leaf, foi.beg);
			foi.read((char*)(&tmp), sizeof(leaf_node));
			cnt += tmp->nowsize + 1;
			while (tmp->next != 0x0)
			{
				foi.seekg(tmp->next, foi.beg);
				foi.read((char*)(&tmp), sizeof(leaf_node));
				cnt += tmp->nowsize + 1;
			}
			return cnt;
		}
		// Clear the BTree
		void clear() {
			foi.clear();
			foi.seekp(0, foi.beg);
			save_data.root = save_data.first_left = 0x0;
			save_data.now_for_change = (long)foi.tellp() +sizeof(save);
			foi.seekp(0, foi.beg);
			foi.write(reinterpret_cast<char*>(&save_data), sizeof(save));
		}
		/**
		 * Returns the number of elements with key
		 *   that compares equivalent to the specified argument,
		 * The default method of check the equivalence is !(a < b || b > a)
		 */
		size_t count(const Key& key) const {
			leaf_node *tmp = foi.read(save_data.first_leaf, sizeof(leaf_node));
			size_t cnt = 0;
			for (int i = 0; i <= tmp->nowsize; i++)
			{
				if (!(tmp->data[i].Key > key || tmp->data[i].Key < key)) cnt++;
			}
			while (tmp->next != 0x0)
			{
				tmp = foi.read(tmp->next, sizeof(leaf_node));
				for (int i = 0; i <= tmp->nowsize; i++)
				{
					if (!(tmp->data[i].Key > key || tmp->data[i].Key < key)) cnt++;
				}
			}
			return cnt;
		}

	};
}  // namespace sjtu
