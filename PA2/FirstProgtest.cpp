#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <compare>
#include <functional>
#include <stdexcept>
#include <algorithm>
#endif /* __PROGTEST__ */
using namespace std;

class CPerson{
public:
    string name;
    string addr;
    string account;
    int sumIncome;
    int sumExpense;
    CPerson(const std::string & name_, const std::string & addr_, const std::string & account_)
            : name(name_), addr(addr_), account(account_) {
        sumIncome = 0;
        sumExpense = 0;
    }
};

class CIterator
{
public:
    CIterator(const CIterator& other) = default;
    CIterator(vector<CPerson> persons_)
            : persons(std::move(persons_)), position(0) {
    };
    bool                atEnd   () const{
        return position >= persons.size();
    };
    void                next    (){
        if(!atEnd())
            position++;
    };
    const std::string &name() const {
        if (atEnd()) return emptyStr;
        return persons[position].name;
    }

    const std::string &addr() const {
        if (atEnd()) return emptyStr;
        return persons[position].addr;
    }

    const std::string &account() const {
        if (atEnd()) return emptyStr;
        return persons[position].account;
    }
private:
    const vector<CPerson> persons;
    size_t position;
    static inline const string emptyStr = "";
};

class CTaxRegister
{
public:
    bool      birth      ( const std::string    & name,
                           const std::string    & addr,
                           const std::string    & account ){
        CPerson person(name, addr, account);
        auto it = lower_bound(persons.begin(), persons.end(), person, cmpPersons);
        if(it != persons.end() && it->name == name && it->addr == addr)
            return false;

        auto it2 = lower_bound(personsByAccounts.begin(), personsByAccounts.end(), person, cmpAccounts);
        if(it2 != personsByAccounts.end() && it2->account == account)
            return false;

        personsByAccounts.insert(it2, person);
        persons.insert(it,person);
        return true;
    };
    bool      death      ( const std::string    & name,
                           const std::string    & addr ){
        CPerson person(name, addr, "");
        auto it = lower_bound(persons.begin(), persons.end(), person, cmpPersons);
        if(it == persons.end() || it->name != name || it->addr != addr)
            return false;

        auto it2 = lower_bound(personsByAccounts.begin(), personsByAccounts.end(), CPerson("", "", it->account), cmpAccounts);
        personsByAccounts.erase(it2);
        persons.erase(it);
        return true;
    };
    bool      income     ( const std::string    & account,
                           int                    amount ){
        CPerson person("", "", account);
        auto it = lower_bound(personsByAccounts.begin(), personsByAccounts.end(), person, cmpAccounts);
        if(it == persons.end() || it->account != account)
            return false;
        auto it2 = lower_bound(persons.begin(), persons.end(), CPerson(it->name, it->addr, ""), cmpPersons);
        it2->sumIncome += amount;
        it->sumIncome += amount;

        return true;
    };
    bool      income     ( const std::string    & name,
                           const std::string    & addr,
                           int                    amount ){
        CPerson person(name, addr, "");
        auto it = lower_bound(persons.begin(), persons.end(), person, cmpPersons);
        if(it == persons.end() || it->name != name || it->addr != addr)
            return false;

        auto it2 = lower_bound(personsByAccounts.begin(), personsByAccounts.end(), CPerson("", "", it->account), cmpAccounts);
        it2->sumIncome += amount;
        it->sumIncome += amount;
        return true;
    };
    bool      expense    ( const std::string    & account,
                           int                    amount ){
        CPerson person("", "", account);
        auto it = lower_bound(personsByAccounts.begin(), personsByAccounts.end(), person, cmpAccounts);
        if(it == persons.end() || it->account != account)
            return false;

        auto it2 = lower_bound(persons.begin(), persons.end(), CPerson(it->name, it->addr, ""), cmpPersons);
        it2->sumExpense += amount;
        it->sumExpense += amount;
        return true;
    };
    bool      expense    ( const std::string    & name,
                           const std::string    & addr,
                           int                    amount ){
        CPerson person(name, addr, "");
        auto it = lower_bound(persons.begin(), persons.end(), person, cmpPersons);
        if(it == persons.end() || it->name != name || it->addr != addr)
            return false;
        auto it2 = lower_bound(personsByAccounts.begin(), personsByAccounts.end(), CPerson("", "", it->account), cmpAccounts);
        it2->sumExpense += amount;
        it->sumExpense += amount;
        return true;
    };
    bool      audit      ( const std::string    & name,
                           const std::string    & addr,
                           std::string          & account,
                           int                  & sumIncome,
                           int                  & sumExpense ) const{
        CPerson person(name, addr, "");
        auto it = lower_bound(persons.begin(), persons.end(), person, cmpPersons);
        if(it == persons.end() || it->name != name || it->addr != addr)
            return false;
        account = it->account;
        sumIncome = it->sumIncome;
        sumExpense = it->sumExpense;
        return true;
    };
    CIterator listByName () const{
        return CIterator(persons);
    };

    static bool cmpPersons(const CPerson &first, const CPerson &second){
        if(first.name == second.name)
            return first.addr < second.addr;
        return first.name < second.name;
    }
    static bool cmpAccounts(const CPerson &first, const CPerson &second) {
        return first.account < second.account;
    }
    vector<CPerson>persons;
    vector<CPerson>personsByAccounts;
private:
};

#ifndef __PROGTEST__
int main ()
{
    std::string acct;
    int    sumIncome, sumExpense;
    CTaxRegister b0;
    assert ( b0 . birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
    assert ( b0 . birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
    assert ( b0 . birth ( "Peter Hacker", "Main Street 17", "634oddT" ) );
    assert ( b0 . birth ( "John Smith", "Main Street 17", "Z343rwZ" ) );
    assert ( !b0 . birth ( "John Smith", "Main Street 17", "Z343rwZ" ) );

    assert ( b0 . income ( "Xuj5#94", 1000 ) );
    assert ( b0 . income ( "634oddT", 2000 ) );
    assert ( b0 . income ( "123/456/789", 3000 ) );
    assert ( b0 . income ( "634oddT", 4000 ) );
    assert ( b0 . income ( "Peter Hacker", "Main Street 17", 2000 ) );
    assert ( b0 . expense ( "Jane Hacker", "Main Street 17", 2000 ) );
    assert ( b0 . expense ( "John Smith", "Main Street 17", 500 ) );
    assert ( b0 . expense ( "Jane Hacker", "Main Street 17", 1000 ) );
    assert ( b0 . expense ( "Xuj5#94", 1300 ) );
    assert ( b0 . audit ( "John Smith", "Oak Road 23", acct, sumIncome, sumExpense ) );
    assert ( acct == "123/456/789" );
    assert ( sumIncome == 3000 );
    assert ( sumExpense == 0 );
    assert ( b0 . audit ( "Jane Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
    assert ( acct == "Xuj5#94" );
    assert ( sumIncome == 1000 );
    assert ( sumExpense == 4300 );
    assert ( b0 . audit ( "Peter Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
    assert ( acct == "634oddT" );
    assert ( sumIncome == 8000 );
    assert ( sumExpense == 0 );
    assert ( b0 . audit ( "John Smith", "Main Street 17", acct, sumIncome, sumExpense ) );
    assert ( acct == "Z343rwZ" );
    assert ( sumIncome == 0 );
    assert ( sumExpense == 500 );
    CIterator it = b0 . listByName ();

    assert ( ! it . atEnd ()
             && it . name () == "Jane Hacker"
             && it . addr () == "Main Street 17"
             && it . account () == "Xuj5#94" );
    it . next ();
    assert ( ! it . atEnd ()
             && it . name () == "John Smith"
             && it . addr () == "Main Street 17"
             && it . account () == "Z343rwZ" );
    it . next ();
    assert ( ! it . atEnd ()
             && it . name () == "John Smith"
             && it . addr () == "Oak Road 23"
             && it . account () == "123/456/789" );
    it . next ();
    assert ( ! it . atEnd ()
             && it . name () == "Peter Hacker"
             && it . addr () == "Main Street 17"
             && it . account () == "634oddT" );
    it . next ();
    assert ( it . atEnd () );

    assert ( b0 . death ( "John Smith", "Main Street 17" ) );

    CTaxRegister b1;
    assert ( b1 . birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
    assert ( b1 . birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
    assert ( !b1 . income ( "634oddT", 4000 ) );
    assert ( !b1 . expense ( "John Smith", "Main Street 18", 500 ) );
    assert ( !b1 . audit ( "John Nowak", "Second Street 23", acct, sumIncome, sumExpense ) );
    assert ( !b1 . death ( "Peter Nowak", "5-th Avenue" ) );
    assert ( !b1 . birth ( "Jane Hacker", "Main Street 17", "4et689A" ) );

    assert ( !b1 . birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
    assert ( b1 . death ( "Jane Hacker", "Main Street 17" ) );
    assert ( b1 . birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
    assert ( b1 . audit ( "Joe Hacker", "Elm Street 23", acct, sumIncome, sumExpense ) );
    assert ( acct == "Xuj5#94" );
    assert ( sumIncome == 0 );
    assert ( sumExpense == 0 );
    assert ( !b1 . birth ( "Joe Hacker", "Elm Street 23", "AAj5#94" ) );

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
