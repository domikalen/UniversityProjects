#ifndef __PROGTEST__
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <utility>
#include <vector>

using namespace std;
using Symbol = char;
using Word = std::vector<Symbol>;

struct Grammar {
    std::set<Symbol> m_Nonterminals;
    std::set<Symbol> m_Terminals;
    std::vector<std::pair<Symbol, std::vector<Symbol>>> m_Rules;
    Symbol m_InitialSymbol;
};
#endif

struct Tras {
    Tras(Symbol next1, vector<size_t> next2, std::vector<Symbol> next3) : path(next1, next2), list(std::move(next3)) {}

    std::pair<Symbol, vector<size_t>> path;
    std::vector<Symbol> list;
};

struct Kostyl{
    pair<pair<size_t, size_t>, pair<size_t, size_t>>combination;
};

vector<Tras> Kartes_soucin(const map<Symbol, vector<size_t>>&first, const map<Symbol, vector<size_t>>&second, const vector<pair<vector<Symbol>, pair<Symbol, size_t>>>&Rules_index){
    vector<Tras>res;
    for(auto left: first){
        for(auto right: second){
            vector<Symbol>cmp = {left.first, right.first};
            for(const auto& to_res_off: Rules_index){
                if (to_res_off.first == cmp) {
                    vector<size_t>path;
                    path.push_back(to_res_off.second.second);
                    path.insert(path.end(), left.second.begin(), left.second.end());
                    path.insert(path.end(), right.second.begin(), right.second.end());
                    res.emplace_back(to_res_off.second.first, path, cmp);
                }
            }
        }
    }
    return res;
}

std::vector<size_t>recursive(map<pair<size_t, size_t>, map<Symbol, vector<size_t>>>&table, size_t start, size_t finish, const vector<pair<vector<Symbol>, pair<Symbol, size_t>>>&Rules_index, Symbol nacalo){
    vector<Tras>res;
    vector<size_t>to_ret;
    for(size_t ll = 1; ll <= finish; ll++){
        size_t left = 1, right = ll + 1;
        for(size_t rr = start - 1; rr > 0; rr--){
            res = Kartes_soucin(table[make_pair(ll, left)], table[make_pair(right, rr)], Rules_index);
            for(auto elem: res){
                table[make_pair(ll, start)][elem.path.first] = elem.path.second;
            }
            left++;
            right++;
        }

    }

    if(finish != 1 && finish != 0){
        to_ret = recursive(table, start+1,finish-1, Rules_index, nacalo);
    }
    else {
        if(finish ==0)
            start--;
        //        for(auto srt: table[make_pair(1,start)][nacalo]){
        //                to_ret = srt;
        //        }
        to_ret = table[make_pair(1,start)][nacalo];
    }
    return to_ret;
}

std::vector<size_t> trace(const Grammar &grammar, const Word &word){

    size_t i = 1;
    set<pair<Symbol, size_t>>column;
    for(auto alph: word){
        column.insert(make_pair(alph, i));
        i++;
    }
    i--;
    map<pair<size_t, size_t>, map<Symbol, vector<size_t>>>table;
    vector<pair<vector<Symbol>, pair<Symbol, size_t>>>Rules_index;
    size_t index = 0;
    for(auto next :grammar.m_Rules){
        Rules_index.emplace_back(next.second, make_pair(next.first, index));
        index++;
    }
    bool flag = false;
    if(word.empty()){
        for(auto edg: Rules_index){
            if(edg.second.first == grammar.m_InitialSymbol&&edg.first.empty()){
                flag = true;
                vector<size_t>ret{edg.second.second};
                return ret;
            }
        }
        if(!flag){
            return {};
        }
    }
    for(auto coordinate: column){
        for(auto next :Rules_index){
            if(next.first.size() == 1){
                if(coordinate.first == next.first[0]){
                    table[make_pair(coordinate.second, 1)][next.second.first] = vector<size_t>{next.second.second};
                }
            }
        }
    }

    return recursive(table, 2, i-1, Rules_index, grammar.m_InitialSymbol);
};

#ifndef __PROGTEST__
int main()
{
    Grammar g0{
            {'A', 'B', 'C', 'S'},
            {'a', 'b'},
            {
                    {'S', {'A', 'B'}},
                    {'S', {'B', 'C'}},
                    {'A', {'B', 'A'}},
                    {'A', {'a'}},
                    {'B', {'C', 'C'}},
                    {'B', {'b'}},
                    {'C', {'A', 'B'}},
                    {'C', {'a'}},
            },
            'S'};

    assert(trace(g0, {'b', 'a', 'a', 'b', 'a'}) == std::vector<size_t>({0, 2, 5, 3, 4, 6, 3, 5, 7}));
    assert(trace(g0, {'b'}) == std::vector<size_t>({}));
    assert(trace(g0, {'a'}) == std::vector<size_t>({}));
    assert(trace(g0, {}) == std::vector<size_t>({}));
    assert(trace(g0, {'a', 'a', 'a', 'a', 'a'}) == std::vector<size_t>({1, 4, 6, 3, 4, 7, 7, 7, 7}));
    assert(trace(g0, {'a', 'b'}) == std::vector<size_t>({0, 3, 5}));
    assert(trace(g0, {'b', 'a'}) == std::vector<size_t>({1, 5, 7}));
    assert(trace(g0, {'c', 'a'}) == std::vector<size_t>({}));

    Grammar g1{
            {'A', 'B'},
            {'x', 'y'},
            {
                    {'A', {}},
                    {'A', {'x'}},
                    {'B', {'x'}},
                    {'A', {'B', 'B'}},
                    {'B', {'B', 'B'}},
            },
            'A'};

    assert(trace(g1, {}) == std::vector<size_t>({0}));
    assert(trace(g1, {'x'}) == std::vector<size_t>({1}));
    assert(trace(g1, {'x', 'x'}) == std::vector<size_t>({3, 2, 2}));
    assert(trace(g1, {'x', 'x', 'x'}) == std::vector<size_t>({3, 4, 2, 2, 2}));
    assert(trace(g1, {'x', 'x', 'x', 'x'}) == std::vector<size_t>({3, 4, 4, 2, 2, 2, 2}));
    assert(trace(g1, {'x', 'x', 'x', 'x', 'x'}) == std::vector<size_t>({3, 4, 4, 4, 2, 2, 2, 2, 2}));
    assert(trace(g1, {'x', 'x', 'x', 'x', 'x', 'x'}) == std::vector<size_t>({3, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2}));
    assert(trace(g1, {'x', 'x', 'x', 'x', 'x', 'x', 'x'}) == std::vector<size_t>({3, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2}));
    assert(trace(g1, {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'}) == std::vector<size_t>({3, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2}));
    assert(trace(g1, {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'}) == std::vector<size_t>({3, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2}));

    Grammar g2{
            {'A', 'B'},
            {'x', 'y'},
            {
                    {'A', {'x'}},
                    {'B', {'x'}},
                    {'A', {'B', 'B'}},
                    {'B', {'B', 'B'}},
            },
            'B'};

    assert(trace(g2, {}) == std::vector<size_t>({}));
    assert(trace(g2, {'x'}) == std::vector<size_t>({1}));
    assert(trace(g2, {'x', 'x'}) == std::vector<size_t>({3, 1, 1}));
    assert(trace(g2, {'x', 'x', 'x'}) == std::vector<size_t>({3, 3, 1, 1, 1}));

    Grammar g3{
            {'A', 'B', 'C', 'D', 'E', 'S'},
            {'a', 'b'},
            {
                    {'S', {'A', 'B'}},
                    {'S', {'S', 'S'}},
                    {'S', {'a'}},
                    {'A', {'B', 'S'}},
                    {'A', {'C', 'D'}},
                    {'A', {'b'}},
                    {'B', {'D', 'D'}},
                    {'B', {'b'}},
                    {'C', {'D', 'E'}},
                    {'C', {'b'}},
                    {'C', {'a'}},
                    {'D', {'a'}},
                    {'E', {'S', 'S'}},
            },
            'S'};

    assert(trace(g3, {}) == std::vector<size_t>({}));
    assert(trace(g3, {'b'}) == std::vector<size_t>({}));
    assert(trace(g3, {'a', 'b', 'a', 'a', 'b'}) == std::vector<size_t>({1, 2, 0, 3, 7, 1, 2, 2, 7}));
    assert(trace(g3, {'a', 'b', 'a', 'a', 'b', 'a', 'b', 'a', 'b', 'a', 'a'}) == std::vector<size_t>({1, 1, 0, 4, 8, 11, 12, 0, 5, 6, 11, 11, 0, 4, 9, 11, 7, 11, 7, 2, 2}));
}
#endif