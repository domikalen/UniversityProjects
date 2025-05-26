#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <climits>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */
// uncomment if your code implements initializer lists
#define EXTENDED_SYNTAX
class CRangeList;
class CRange
{
public:
    long long lo;
    long long hi;
    CRange(long long lo, long long hi){
        if(lo > hi){
            throw logic_error("Invalid range");
        }
        this->lo = lo;
        this->hi = hi;
    }
    CRangeList operator + (const CRange &range2) const;
    CRangeList operator - (const CRange &range2)const;
    friend ostream &operator << (ostream & out, const CRange &range){
        auto lol = out.flags();
        out.unsetf(lol);
        if(range.lo == range.hi){
            out << range.hi;
        }
        else
            out << '<' << range.lo << ".." << range.hi << '>';
        out.setf(lol);
        return out;
    }
private:
};
class CRangeList
{
    friend class CRange;
public:
//constructor
    CRangeList(){};
    CRangeList(const initializer_list<CRange>& list) {
        for (auto it = list.begin(); it != list.end(); ++it) {
            *this += *it;
        }
    }
    vector<CRange>::iterator begin() {
        return m_Ranges.begin();
    }

    vector<CRange>::iterator end() {
        return m_Ranges.end();
    }

// = operator
    CRangeList &operator = (const CRange &range){
        m_Ranges.clear();
        m_Ranges.push_back(range);
        return *this;
    }
    CRangeList &operator = (const CRangeList &list){
        if(this != &list) {
            m_Ranges.clear();
            copy(list.m_Ranges.begin(), list.m_Ranges.end(), back_inserter(m_Ranges));
        }
        return *this;
    }
// += operator
    CRangeList &operator += (const CRange &range){
        if(m_Ranges.empty()) {
            m_Ranges.push_back(range);
            return *this;
        }
        auto Iter_start = lower_bound(m_Ranges.begin(), m_Ranges.end(), range, cmpRange);
        CRangeList result;
        copy(m_Ranges.begin(), Iter_start, back_inserter(result.m_Ranges));
        long long int poz = Iter_start - m_Ranges.begin();
        if(poz == 0){
            poz = 1;
            result.m_Ranges.push_back(range);
        }
        m_Ranges.insert(Iter_start, range);
        int size = (int) m_Ranges.size();
        for (int i = poz; i < size; i++) {
            CRange& curr = m_Ranges[i];
            CRange& prev = result.m_Ranges.back();

            if (curr.lo != LLONG_MIN && curr.lo - 1 <= prev.hi){
                prev.hi = max(prev.hi, curr.hi);
            }
            else if(curr.lo == LLONG_MIN && curr.lo <= prev.hi){
                prev.hi = max(prev.hi, curr.hi);
            }
            else {
                result.m_Ranges.push_back(curr);
            }
        }
        m_Ranges.clear();
        copy(result.m_Ranges.begin(), result.m_Ranges.end(), back_inserter(m_Ranges));
        return *this;
    }

    CRangeList &operator += (const CRangeList &list){
        if(m_Ranges.empty()){
            *this = list;
            return *this;
        }
        int size = (int) list.m_Ranges.size();
        for(int i = 0; i < size; i++){
            *this += list.m_Ranges[i];
        }
        return *this;
    }
    CRangeList operator + (const CRange &range) const{
        CRangeList newList = *this;
        newList += range;
        return newList;
    }
// -= operator
    CRangeList& operator-=(const CRange& range) {
        vector<CRange> new_ranges;
        new_ranges.reserve(m_Ranges.size());
        auto Iterator = lower_bound(m_Ranges.begin(), m_Ranges.end(), range, cmpRange);
        copy(m_Ranges.begin(), prev(Iterator), back_inserter(new_ranges));

        long long int poz = Iterator - m_Ranges.begin();
        auto it = Iterator - 1;
        if(poz == 0)
            it = Iterator;

        for (; it != m_Ranges.end(); ++it) {
            auto& current = *it;
            if (range.lo <= current.lo && range.hi >= current.hi) {
                continue;
            }
            else if (range.lo > current.hi || range.hi < current.lo) {
                new_ranges.emplace_back(current);
            }
            else {
                if (range.lo > current.lo) {
                    new_ranges.emplace_back(current.lo, range.lo - 1);
                }
                if (range.hi < current.hi) {
                    new_ranges.emplace_back(range.hi + 1, current.hi);
                }
            }
        }
        m_Ranges = std::move(new_ranges);
        return *this;
    }

    CRangeList &operator -= (const CRangeList &list){
        if(m_Ranges.empty()){
            return *this;
        }
        int num_ranges = (int) list.m_Ranges.size();
        for (int i = 0; i < num_ranges; i++) {
            (*this) -= list.m_Ranges[i];
        }
        return *this;
    }
    CRangeList operator - (const CRange &range) const{
        CRangeList result = *this;
        result -= range;
        return result;
    }

// == operator
    bool operator == (const CRangeList &list) const {
        if (m_Ranges.size() != list.m_Ranges.size()) {
            return false;
        }
        return std::equal(m_Ranges.begin(), m_Ranges.end(), list.m_Ranges.begin(),
                          [](const CRange& a, const CRange& b) {
                              return a.lo == b.lo && a.hi == b.hi;
                          });
    }
// != operator
    bool operator != (const CRangeList &list) const{
        return !(*this == list);
    }
// includes function
    bool includes (const long long number) const{
        if(m_Ranges.empty())
            return false;
        auto Iterator = lower_bound(m_Ranges.begin(), m_Ranges.end(), CRange(number,number), cmpRange);

        if(Iterator != m_Ranges.begin() && number != Iterator->lo)
            --Iterator;
        bool sosi = number <= Iterator->hi &&number >= Iterator->lo;
        return sosi;
    }
    bool includes (const CRange &range) const{
        if(m_Ranges.empty())
            return false;
        auto Iterator = lower_bound(m_Ranges.begin(), m_Ranges.end(), range, cmpRange);

        if(Iterator != m_Ranges.begin() && range.lo != Iterator->lo)
            --Iterator;
        bool sosi = range.hi <= Iterator->hi &&range.lo >= Iterator->lo;
        return sosi;
    }
// << operator
    friend ostream &operator << (ostream & out, const CRangeList &list){
        auto lol = out.flags();
        out.unsetf(lol);
        out << '{';
        int size = list.m_Ranges.size();
        int i = 0;
        while(i < size){
            out << list.m_Ranges[i];
            if(i != size - 1){
                out << ',';
            }
            i++;
        }
        out << '}';
        out.setf(lol);
        return out;
    }

private:
    vector<CRange>m_Ranges;
    static bool cmpRange(const CRange &range, const CRange &range2){
        return range.lo < range2.lo;
    }
};

CRangeList CRange::operator + (const CRange &range2) const{
    CRangeList list;
    list = (*this);
    list += range2;
    return list;
}
CRangeList CRange::operator - (const CRange &range2) const{
    CRangeList list;
    list = (*this);
    list -= range2;
    return list;
}
