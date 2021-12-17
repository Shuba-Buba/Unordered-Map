#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <list>


template <class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
private:
    using PairValue = std::pair<const KeyType, ValueType>;
    std::vector<std::list<typename std::list<PairValue>::iterator>> MainTable;
    std::list<PairValue> TableOfValuesAndKeys;

    Hash hasher;
    size_t length = 0;
    int RelocationConstant = 2;

    void reallocation(std::vector<std::list<typename std::list<PairValue>::iterator>>& A, std::list<PairValue> &T2, size_t new_len) {
        std::vector<std::list<iterator>> NewVersion(new_len);
        std::list<PairValue> NewTableOfValuesAndKeys;
        for (auto bucket : A) {
            for (auto to = bucket.begin(); to != bucket.end(); ++to) {
                size_t pos = hasher((*to)->first) % new_len;
                if (!NewVersion[pos].empty()) {
                    auto next = NewVersion[pos].back();
                    auto cur = nex++;
                    NewTableOfValuesAndKeys.insert(next, *(*to));
                    NewVersion[pos].push_back(cur);
                } else {
                    auto cur = NewTableOfValuesAndKeys.end();
                    NewTableOfValuesAndKeys.insert(cur, *(*to));
                    NewVersion[pos].push_back(--cur);
                }
            }
        }
        this->MainTable = std::move(NewVersion);
        this-> TableOfValuesAndKeys = std::move(NewTableOfValuesAndKeys);
    }

public:
    using iterator = typename std::list<PairValue>::iterator;
    using const_iterator = typename std::list<PairValue>::const_iterator;
    void insert(const std::pair<KeyType,ValueType> &A) {
        size_t pos = hasher(A.first) % MainTable.size();
        bool ok = false;
        for (auto to = MainTable[pos].begin(); to != MainTable[pos].end(); ++to) {
            if ((*to)->first == A.first) {
                ok = true;
                break;
            }
        }
        if (!ok) {
            if (MainTable[pos].size()) {
                auto next = MainTable[pos].back();
                auto cur = next++;
                TableOfValuesAndKeys.insert(next, A);
                MainTable[pos].push_back(cur);
            } else {
                auto cur = TableOfValuesAndKeys.end();
                TableOfValuesAndKeys.insert(cur, A);
                --cur;
                MainTable[pos].push_back(cur);
            }
            ++length;
        }
        if (length == MainTable.size()) {
            reallocation(MainTable, TableOfValuesAndKeys, MainTable.size() * RelocationConstant);
        }
    }

    template<class it>
    HashMap(it begin, it end, Hash new_hasher = Hash()) : hasher(new_hasher) {
        MainTable.resize(RelocationConstant);
        while (begin != end) {
            auto new_element = *begin;
            insert(new_element);
            begin++;
        }
    }

    HashMap(const std::initializer_list<std::pair<const KeyType, ValueType>> &A, Hash new_hasher = Hash()) : HashMap(A.begin(), A.end(), new_hasher) {
    }

        HashMap() : HashMap({}, std::hash<KeyType>()) {
        }

        size_t size() const {
            return length;
        }


        bool empty() const {
            return (length == 0);
        }

        Hash hash_function() const {
            return hasher;
        }

        void erase(const KeyType &key) {
            size_t pos = hasher(key) % MainTable.size();
            for (auto iter = MainTable[pos].begin(); iter != MainTable[pos].end(); ++iter) {
                if ((*iter)->first == key) {
                 TableOfValuesAndKeys.erase(*iter);
                    MainTable[pos].erase(iter);
                    --length;
                    break;
                }
            }
            if (length <= MainTable.size() / (RelocationConstant * RelocationConstant)) {
                size_t new_size = length * RelocationConstant;
                if (new_size == 0) {
                   new_size = RelocationConstant;
                }
                reallocation(MainTable, TableOfValuesAndKeys, new_size);
            }
        }
        iterator begin() {
            return TableOfValuesAndKeys.begin();
        }

        iterator end() {
            return TableOfValuesAndKeys.end();
        }

        const_iterator begin() const {
            return TableOfValuesAndKeys.cbegin();
        }

        const_iterator end() const {
           return TableOfValuesAndKeys.end();
        }
    iterator find(const KeyType &key) {
            size_t pos = hasher(key) % MainTable.size();
            for (auto to = MainTable[pos].begin(); to != MainTable[pos].end(); ++to) {
                if ((*to)->first == key) {
                    return *to;
                }
            }
            return end();
        }

        const_iterator find(const KeyType &key) const {
            size_t pos = hasher(key) % MainTable.size();
            for (auto to = MainTable[pos].begin(); to != MainTable[pos].end(); ++to) {
                if ((*to)->first == key) {
                    return *to;
                }
            }
            return end();
        }

        ValueType &operator[](const KeyType &K) {
            auto to = find(K);
            if (to == end()) {
                insert({K, ValueType()});
            }
            to = find(K);
            return to->second;
        }

        const ValueType &at(const KeyType &K) const {
            auto to = find(K);
            if (to == end()) {
               throw std::out_of_range("Shuba_Buba");
            }
            return (*to).second;
        }

        HashMap(const HashMap &other) {
            clear();
            hasher = other.hasher;
            for (const auto &to : other.TableOfValuesAndKeys) {
                insert(to);
            }
        }

        HashMap &operator=(const HashMap &other) {
            if (this == &other) {
                return *this;
            }
            this->clear();
            this->hasher = other.hasher;
            for (const auto &it : other.TableOfValuesAndKeys) {
                this->insert(it);
            }
            return *this;
        }

        HashMap(Hash _) : hasher(_) {
            clear();
        }

        void clear() {
            for (size_t t = 0; t < MainTable.size(); ++t) {
                for (auto &to : MainTable[t]) {
                    to = TableOfValuesAndKeys.end();
                }
            } 
            MainTable.clear();
            TableOfValuesAndKeys.clear();
            MainTable.resize(RelocationConstant);
            length = 0;
        }
};

