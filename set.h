#include <set>
#include <vector>
#include <array>
#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std;

template<class ValueType>
class Set {
public:
    class Node {
    public:
        array<int, 3> sons;
        int p;

        int sz;
        ValueType Lvalue = ValueType();
        ValueType Rvalue = ValueType();

        Node() {
            fill(sons.begin(), sons.end(), -1);
            p = -1;
            sz = 0;
            Lvalue = ValueType();
            Rvalue = ValueType();
        }

        bool leaf() const {
            int l = 1;
            for (auto t : sons) {
                l &= (t == -1);
            }
            return l;
        }

        int count_sons() const {
            int l = 0;
            for (auto t : sons) {
                if (t != -1) {
                    l++;
                }
            }
            return l;
        }

        int next_son(int v) const {
            for (int i = 0; i < 2; i++) {
                if (sons[i] == v) {
                    assert(sons[i + 1] != -1);
                    return sons[i + 1];
                }
            }
            assert(false);
            return -1;
        }

        int prev_son(int v) const {
            for (int i = 1; i < 3; i++) {
                if (sons[i] == v) {
                    assert(sons[i - 1] != -1);
                    return sons[i - 1];
                }
            }
            assert(false);
            return -1;
        }

        int first_son() const {
            for (auto t : sons) {
                if (t != -1) {
                    return t;
                }
            }
            assert(false);
            return -1;
        }

        int last_son() const {
            int l = -1;
            for (auto t : sons) {
                if (t != -1) {
                    l = t;
                }
            }
            assert(l != -1);
            return l;
        }

        bool contains(ValueType value) const {
            return !(value < Lvalue) && !(Rvalue < value);
        }
    };

    vector<Node> nodes;

    void pull(int v) {
        if (!nodes[v].leaf()) {
            nodes[v].sz = 0;
            int was = 0;
            nodes[v].Lvalue = ValueType();
            nodes[v].Rvalue = ValueType();
            for (auto t : nodes[v].sons) {
                if (t != -1) {
                    nodes[t].p = v;
                    nodes[v].sz += nodes[t].sz;
                    if (!was || nodes[t].Lvalue < nodes[v].Lvalue) {
                        nodes[v].Lvalue = nodes[t].Lvalue;
                    }
                    if (!was || nodes[v].Rvalue < nodes[t].Rvalue) {
                        nodes[v].Rvalue = nodes[t].Rvalue;
                    }
                    was = 1;
                }
            }
        } else {
            nodes[v].sz = 1;
        }
    }

    void pull_all(int v) {
        pull(v);
        if (nodes[v].p != -1) {
            pull_all(nodes[v].p);
        }
    }

    void sort_sons(int v) {
        sort(nodes[v].sons.begin(), nodes[v].sons.end(), [&](int i, int j) {
            if (i == j) {
                return false;
            }
            if (i == -1) {
                return false;
            }
            if (j == -1) {
                return true;
            }
            return nodes[i].Lvalue < nodes[j].Lvalue;
        });
    }

    void sort_nodes(vector<int> &ind) {
        sort(ind.begin(), ind.end(), [&](int i, int j) {
            return nodes[i].Lvalue < nodes[j].Lvalue;
        });
    }

    int new_node() {
        int res = nodes.size();
        nodes.push_back(Node());
        return res;
    }

    int new_node(ValueType value) {
        int res = nodes.size();
        nodes.push_back(Node());
        nodes[res].Lvalue = value;
        nodes[res].Rvalue = value;
        nodes[res].sz = 1;
        return res;
    }

    void FixInsert(int v, int nw_node) {
        for (int i = 0; i < 3; i++) {
            if (nodes[v].sons[i] == -1) {
                nodes[v].sons[i] = nw_node;
                sort_sons(v);
                pull_all(v);
                return;
            }
        }
        vector<int> nn;
        for (int i = 0; i < 3; i++) {
            nn.push_back(nodes[v].sons[i]);
        }
        nn.push_back(nw_node);
        sort_nodes(nn);
        if (nodes[v].p == -1) {
            int F = new_node();
            int S = new_node();
            nodes[F].sons[0] = nn[0];
            nodes[F].sons[1] = nn[1];
            nodes[F].sons[2] = -1;
            nodes[S].sons[0] = nn[2];
            nodes[S].sons[1] = nn[3];
            nodes[S].sons[2] = -1;
            nodes[v].sons[0] = F;
            nodes[v].sons[1] = S;
            nodes[v].sons[2] = -1;
            pull(F);
            pull(S);
            pull(v);
        } else {
            int nw = new_node();
            nodes[v].sons[0] = nn[0];
            nodes[v].sons[1] = nn[1];
            nodes[v].sons[2] = -1;
            nodes[nw].sons[0] = nn[2];
            nodes[nw].sons[1] = nn[3];
            nodes[nw].sons[2] = -1;
            pull(v);
            pull(nw);
            FixInsert(nodes[v].p, nw);
        }
    }

    void FixErase(int v) {
        vector<int> gr;
        for (auto t : nodes[v].sons) {
            if (t == -1) {
                continue;
            }
            for (auto k : nodes[t].sons) {
                if (k != -1) {
                    gr.push_back(k);
                }
            }
        }
        if (gr.size() == 3) {
            if (nodes[v].p == -1) {
                nodes[v].sons[0] = gr[0];
                nodes[v].sons[1] = gr[1];
                nodes[v].sons[2] = gr[2];
                pull(v);
                return;
            } else {
                int nw = new_node();
                nodes[nw].sons[0] = gr[0];
                nodes[nw].sons[1] = gr[1];
                nodes[nw].sons[2] = gr[2];
                nodes[v].sons[0] = nw;
                nodes[v].sons[1] = -1;
                nodes[v].sons[2] = -1;
                pull(nw);
                pull(v);
                FixErase(nodes[v].p);
                return;
            }
        } else {
            int count_sons = 2;
            if (gr.size() >= 6) {
                count_sons = 3;
            }
            vector<int> count_grandsons(count_sons, 2);
            int left = gr.size() - count_sons * 2;
            for (int i = 0; i < left; ++i) {
                ++count_grandsons[i];
            }
            nodes[v].sons[0] = -1;
            nodes[v].sons[1] = -1;
            nodes[v].sons[2] = -1;
            int pnt = 0;
            for (int i = 0; i < count_sons; i++) {
                int son = new_node();
                for (int j = 0; j < count_grandsons[i]; j++) {
                    nodes[son].sons[j] = gr[pnt++];
                }
                pull(son);
                nodes[v].sons[i] = son;
            }
            pull_all(v);
            return;
        }
    }

    void print(int v, vector<int> &ans) {
        if (v == -1) {
            return;
        }
        if (nodes[v].leaf()) {
            ans.push_back(nodes[v].Lvalue);
        }
        for (auto t : nodes[v].sons) {
            print(t, ans);
        }
    }

    int Find(int v, ValueType value) const {
        if (v == -1) {
            return -1;
        }
        if (nodes[v].leaf()) {
            if (nodes[v].contains(value)) {
                return v;
            } else {
                return -1;
            }
        } else {
            for (auto t : nodes[v].sons) {
                if (t != -1 && nodes[t].contains(value)) {
                    return Find(t, value);
                }
            }
            return -1;
        }
    }

    int FindInsert(int v, ValueType value) {
        if (nodes[v].leaf()) {
            return v;
        } else {
            for (auto t : nodes[v].sons) {
                if (t == -1) continue;
                if (nodes[t].Rvalue < value) {
                    continue;
                } else {
                    return FindInsert(t, value);
                }
            }
            return FindInsert(nodes[v].last_son(), value);
        }
    }

    int LowerBound(int v, ValueType value) const {
        if (nodes[v].leaf()) {
            if (!(nodes[v].Lvalue < value)) {
                return v;
            } else {
                return -1;
            }
        } else {
            for (auto t : nodes[v].sons) {
                if (t == -1) continue;
                if (nodes[t].Rvalue < value) {
                    continue;
                } else {
                    return LowerBound(t, value);
                }
            }
            return -1;
        }
    }

    int GoLeft(int v) const {
        while (!nodes[v].leaf()) {
            v = nodes[v].first_son();
        }
        return v;
    }

    int GoRight(int v) const {
        while (!nodes[v].leaf()) {
            v = nodes[v].last_son();
        }
        return v;
    }

    int FindNext(int v) const {
        while (nodes[v].p != -1) {
            int p = nodes[v].p;
            if (nodes[p].last_son() == v) {
                v = p;
            } else {
                int to = nodes[p].next_son(v);
                return GoLeft(to);
            }
        }
        return -1;
    }

    int FindPrev(int v) const {
        while (nodes[v].p != -1) {
            int p = nodes[v].p;
            if (nodes[p].first_son() == v) {
                v = nodes[v].p;
            } else {
                int to = nodes[p].prev_son(v);
                return GoRight(to);
            }
        }
        return -1;
    }

    vector<int> print() {
        vector<int> ans;
        print(root, ans);
        return ans;
    }

    void insert(const ValueType& value) {
        if (Find(root, value) != -1) {
            return;
        }
        if (root == -1) {
            root = new_node(value);
            return;
        }
        int v = FindInsert(root, value);
        if (nodes[v].p != -1) {
            FixInsert(nodes[v].p, new_node(value));
        } else {
            int nroot = new_node();
            nodes[nroot].sons[0] = root;
            nodes[nroot].sons[1] = new_node(value);
            nodes[nroot].sons[2] = -1;
            sort_sons(nroot);
            pull(nroot);
            root = nroot;
        }
    }

    void erase(const ValueType& value) {
        if (Find(root, value) == -1) {
            return;
        }
        int v = Find(root, value);
        if (nodes[v].p == -1) {
            clear();
            return;
        } else {
            int p = nodes[v].p;
            for (int i = 0; i < 3; i++) {
                if (nodes[p].sons[i] == v) {
                    nodes[p].sons[i] = -1;
                }
            }
            sort_sons(p);
            pull(p);
            if (nodes[p].p != -1) {
                FixErase(nodes[p].p);
            } else {
                int cnt = nodes[p].count_sons();
                if (cnt > 1) {
                    return;
                } else {
                    int nd = nodes[p].last_son();
                    root = new_node(nodes[nd].Lvalue);
                    pull(root);
                }
            }
        }
    }

    class iterator {
    public:
        const Set* set;
        int node;

        iterator(): set(nullptr), node(-1) {}

        iterator(const Set *set_, int node_): set(set_), node(node_) {}

        iterator& operator=(const iterator& other) {
            set = other.set;
            node = other.node;
            return *this;
        }

        iterator(const iterator& other): set(other.set), node(other.node) {}

        const ValueType& operator*() const {
            if (node == -1) assert(false);
            return set->nodes[node].Lvalue;
        }

        const ValueType* operator->() const {
            if (node == -1) assert(false);
            return &set->nodes[node].Lvalue;
        }

        bool operator==(const iterator& other) const {
            return set == other.set && node == other.node;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

        iterator operator++(int) {
            if (node == -1) {
                assert(false);
            }
            iterator tmp = *this;
            int nw = set->FindNext(node);
            node = nw;
            return tmp;
        }

        iterator& operator++() {
            if (node == -1) {
                assert(false);
            }
            int nw = set->FindNext(node);
            node = nw;
            return *this;
        }

        iterator operator--(int) {
            int nw;
            if (node == -1) {
                nw = set->GoRight(set->root);
            } else {
                nw = set->FindPrev(node);
            }
            iterator tmp = *this;
            node = nw;
            return tmp;
        }

        iterator& operator--() {
            int nw;
            if (node == -1) {
                nw = set->GoRight(set->root);
            } else {
                nw = set->FindPrev(node);
            }
            node = nw;
            return *this;
        }
    };

    size_t size() const {
        if (root == -1) {
            return 0;
        } else {
            return nodes[root].sz;
        }
    }

    bool empty() const {
        return size() == static_cast<size_t>(0);
    }

    iterator begin() const {
        if (empty()) {
            return end();
        }
        return iterator(this, GoLeft(root));
    }

    iterator end() const {
        auto res = iterator(this, -1);
        return res;
    }

    iterator find(const ValueType& value) const {
        if (empty()) {
            return end();
        }
        return iterator(this, Find(root, value));
    }

    iterator lower_bound(const ValueType& value) const {
        if (empty()) {
            return end();
        }
        int node = LowerBound(root, value);
        return iterator(this, node);
    }

    Set() {
        clear();
    }

    Set(const Set &other) {
        clear();
        root = other.root;
        nodes = other.nodes;
    }

    template<typename Iterator>
    Set(Iterator first, Iterator last) {
        clear();
        while (first != last) {
            insert(*first);
            first++;
        }
    }

    Set(std::initializer_list<ValueType> elems) {
        clear();
        for (auto x : elems) {
            insert(x);
        }
    }

    ~Set() {
        clear();
        return;
    }

    void clear() {
        root = -1;
        nodes.clear();
    }

    Set& operator=(const Set &other) {
        if (&other == this) {
            return *this;
        }
        clear();
        root = other.root;
        nodes = other.nodes;
        return *this;
    }
private:
    int root = -1;
};
