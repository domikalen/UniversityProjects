#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

#include <algorithm>
#include <deque>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <vector>

#include <cassert>
using namespace std;

using State = unsigned int;
using Symbol = char;

struct MISNFA {
    std::set<State> m_States;
    std::set<Symbol> m_Alphabet;
    std::map<std::pair<State, Symbol>, std::set<State>> m_Transitions;
    std::set<State> m_InitialStates;
    std::set<State> m_FinalStates;
};

struct DFA {
    std::set<State> m_States;
    std::set<Symbol> m_Alphabet;
    std::map<std::pair<State, Symbol>, State> m_Transitions;
    State m_InitialState;
    std::set<State> m_FinalStates;

    bool operator==(const DFA& dfa)
    {
        return std::tie(m_States, m_Alphabet, m_Transitions, m_InitialState, m_FinalStates) == std::tie(dfa.m_States, dfa.m_Alphabet, dfa.m_Transitions, dfa.m_InitialState, dfa.m_FinalStates);
    }
};

#endif
MISNFA can_end(MISNFA nfa, map<pair<State, Symbol>, set<State>>m_Trans_revers){
    MISNFA normal;
    normal.m_Alphabet = nfa.m_Alphabet;
    normal.m_FinalStates = nfa.m_FinalStates;

    queue<State>q;
    set<State>visited;
    for(auto to_push : nfa.m_FinalStates) {
        q.push(to_push);
        visited.insert(to_push);
        normal.m_States.insert(to_push);
        if(nfa.m_InitialStates.find(to_push) != nfa.m_InitialStates.end()){
            normal.m_InitialStates.insert(to_push);
        }
    }

    while(!q.empty()){
        auto curr = q.front();
        q.pop();

        for(auto curr_alph : nfa.m_Alphabet){
            auto it = m_Trans_revers.find(make_pair(curr, curr_alph));
            if(it != m_Trans_revers.end()){
                for(auto curr_normal : it->second){
                    auto it2 = nfa.m_Transitions.find(make_pair(curr_normal, curr_alph));
                    if(it2 != nfa.m_Transitions.end()){
                        normal.m_Transitions[make_pair(curr_normal, curr_alph)] = it2->second;
                        normal.m_States.insert(curr_normal);
                        if(visited.find(curr_normal)==visited.end()){
                            visited.insert(curr_normal);
                            q.push(curr_normal);
                            if(nfa.m_InitialStates.find(curr_normal) != nfa.m_InitialStates.end()){
                                normal.m_InitialStates.insert(curr_normal);
                            }
                        }
                    }
                }
            }
        }

    }

    return normal;
};

MISNFA can_visit(const MISNFA& nfa){
    map<pair<State, Symbol>, set<State>> m_Trans_revers;
    MISNFA normal;
    normal.m_Alphabet = nfa.m_Alphabet;
    normal.m_InitialStates = nfa.m_InitialStates;

    queue<State>q;
    set<State>visited; //maybe need to use set in this case
    for(auto to_push : nfa.m_InitialStates) {
        q.push(to_push);
        visited.insert(to_push);
        normal.m_States.insert(to_push);
        if(nfa.m_FinalStates.find(to_push) != nfa.m_FinalStates.end()){
            normal.m_FinalStates.insert(to_push);
        }
    }

    while(!q.empty()){
        auto curr = q.front();
        q.pop();
        for(auto curr_alph : nfa.m_Alphabet){
            auto it = nfa.m_Transitions.find({curr, curr_alph});
            auto it2 = normal.m_Transitions.find({curr, curr_alph});

            if(it != nfa.m_Transitions.end()){
                if(it2 == normal.m_Transitions.end()){
                    normal.m_Transitions[make_pair(curr, curr_alph)] = set<State>{};
                    for(auto to_push: it->second){
                        normal.m_Transitions[make_pair(curr, curr_alph)].insert(to_push);
                        m_Trans_revers[make_pair(to_push, curr_alph)].insert(curr);
                        if(visited.find(to_push) == visited.end()){
                            visited.insert(to_push);
                            q.push(to_push);
                            normal.m_States.insert(to_push);
                            if(nfa.m_FinalStates.find(to_push) != nfa.m_FinalStates.end()){
                                normal.m_FinalStates.insert(to_push);
                            }
                        }
                    }
                }
            }
        }
    }

    return can_end(normal, m_Trans_revers);
};

DFA determinize(const MISNFA& nfa){
    MISNFA normal = can_visit(nfa);
    map<set<State>, State>new_state;
    DFA result;
    result.m_Alphabet = normal.m_Alphabet;
    queue<set<State>>q;
    set<set<State>>visited;

    int index_state = 0;
    set<State>start;
    for(auto start_state: normal.m_InitialStates){
        start.insert(start_state);
        if(normal.m_FinalStates.find(start_state) != normal.m_FinalStates.end())
        {
            result.m_FinalStates.insert(index_state);
        }
    }
    result.m_InitialState = index_state;
    result.m_States.insert(index_state);
    new_state[start] = index_state;

    q.push(start);
    visited.insert(start);

    while(!q.empty()){
        auto curr = q.front();
        q.pop();
        int curr_index = new_state[curr];
        for(auto alp: normal.m_Alphabet){
            set<State>merge_state;
            bool last_state = false;
            for(auto state: curr){
                auto it = normal.m_Transitions.find(make_pair(state, alp));
                if(it != normal.m_Transitions.end()){
                    merge_state.insert(it->second.begin(), it->second.end());
                }
                if(normal.m_FinalStates.find(state) != normal.m_FinalStates.end()){
                    last_state = true;
                }
            }
            if(!merge_state.empty()){
                if(new_state.find(merge_state) == new_state.end()){
                    index_state++;
                    new_state[merge_state] = index_state;
                    result.m_States.insert(index_state);

                }
                result.m_Transitions[make_pair(curr_index, alp)] = new_state[merge_state];
                if(visited.find(merge_state) == visited.end()){
                    visited.insert(merge_state);
                    q.push(merge_state);
                }
            }
            if(last_state){
                result.m_FinalStates.insert(curr_index);
            }

        }
    }


    return result;
};
