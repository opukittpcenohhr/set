#include "set.h"

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include <string>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <fstream>
#include <cassert>
#include <cstring>
#include <unordered_set>
#include <unordered_map>
#include <numeric>
#include <ctime>
#include <bitset>
#include <complex>
#include <chrono>
#include <random>
#include <functional>

using namespace std;

mt19937 rng(10);

void insert(vector<int> &a, int val) {
    if (find(a.begin(), a.end(), val) == a.end()) {
        a.push_back(val);
        sort(a.begin(), a.end());
    }
}

void erase(vector<int> &a, int val) {
    if (find(a.begin(), a.end(), val) != a.end()) {
        a.erase(find(a.begin(), a.end(), val));
    }
}

void print(vector<int> v) {
    for (auto x : v) {
        cout << x << ' ';
    }
    cout << endl;
}

void check_all() {
    const int IT = 1000;
    const int C = 50;

    Set<int> my;
    vector<int> ok;
    vector<int> have(C);
    for (int iit = 0; iit < IT; iit++) {
        int v = rng() % 5;
        if (v == 0 || v == 1) {
            int val = rng() % C;
            if (have[val]) {
                my.erase(val);
                erase(ok, val);
            } else {
                my.insert(val);
                insert(ok, val);
            }
            have[val] ^= 1;
        } else if (v == 2) {
            int val = rng() % C;
            auto it = my.lower_bound(val);
            auto oit = lower_bound(ok.begin(), ok.end(), val);
            while (oit != ok.end()) {
                assert(*it == *oit);
                it++;
                oit++;
            }
            assert(it == my.end());
            for (int i = 0; i < (int)ok.size(); i++) {
                it--;
                oit--;
                assert(*it == *oit);
            }
            assert(it == my.begin());
            assert(oit == ok.begin());
        } else if (v == 3) {
            assert(my.size() == ok.size());
            assert(my.empty() == ok.empty());
        } else {
            Set<int> tmp = my;
            my = Set<int>(tmp);
        }
        if (ok != my.print()) {
            print(ok);
            print(my.print());
            exit(0);
        }
    }
}

signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);

    for (int it = 0; it < 100000; it++) {
        check_all();
    }
}
