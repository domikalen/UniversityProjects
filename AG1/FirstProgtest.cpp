#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <string>
#include <vector>
#include <array>
#include <iterator>
#include <set>
#include <list>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <compare>
#include <queue>
#include <stack>
#include <deque>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <stdexcept>
#include <condition_variable>
#include <pthread.h>
#include <semaphore.h>
#include "progtest_solver.h"
#include "sample_tester.h"
using namespace std;
#endif /* __PROGTEST__ */
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
class COptimizer;

class CCasePack{
public:
    CCasePack(size_t id, AProblemPack problems){
        id_pack = id;
        pack = problems;
        m_index_min = 0;
        m_index_cnt = 0;
        maxmin = problems->m_ProblemsMin.size();
        maxcnt = problems->m_ProblemsCnt.size();
    };

    void nextMinIndex() {
        if(m_index_min >= pack->m_ProblemsMin.size()) {
            return;
        }
        m_index_min++;
    }

    void nextCntIndex() {
        if(m_index_cnt >= pack->m_ProblemsCnt.size()) {
            return;
        }
        m_index_cnt++;
    }
    bool is_Solved(){
        return ((m_index_cnt >= pack->m_ProblemsCnt.size()) && (m_index_min >= pack->m_ProblemsMin.size()));
    }
    size_t id_pack;
    size_t m_index_min;
    size_t m_index_cnt;
    size_t maxmin;
    size_t maxcnt;
    AProblemPack pack;
};

class CMyPolygon{
public:
    CMyPolygon(size_t id_company, size_t id_pack, APolygon pol, bool type){
        m_id_company = id_company;
        m_id_pack = id_pack;
        problem = pol;
        m_type_min = type;
    }
    size_t m_id_company;
    size_t m_id_pack;
    APolygon problem;
    bool m_type_min;
};

using AMyPolygon = shared_ptr<CMyPolygon>;
class CMap{
public:
    map<size_t, shared_ptr<CCasePack>>m_problems;
    mutex m_Mtx;
    void add_problems(size_t id, AProblemPack to_solve){
        unique_lock<mutex> lock(m_Mtx);
        m_problems[id]= make_shared<CCasePack>(id, to_solve);
    }

    void nextIndex(AMyPolygon polygon) {
        unique_lock<mutex> lock(m_Mtx);
        if(polygon->m_type_min) {
            m_problems[polygon->m_id_pack]->nextMinIndex();
        }
        else {
            m_problems[polygon->m_id_pack]->nextCntIndex();
        }

    }

    bool solved(size_t id) {
        unique_lock<mutex> lock(m_Mtx);
        return m_problems[id]->is_Solved();
    }

    AProblemPack solvedPackMapa(size_t id) {
        unique_lock<mutex> lock(m_Mtx);
        AProblemPack result = m_problems[id]->pack;
        m_problems.erase(id);
        return result;
    }
};

class CCaseFront{
public:
    mutex m_Mtx;
    queue<AMyPolygon>q;

    AMyPolygon get_front(){
        unique_lock<mutex>lock(m_Mtx);
        return q.front();
    }

    void add_front(AMyPolygon next){
        unique_lock<mutex>lock(m_Mtx);
        q.push(next);
    }

    void pop_front(){
        unique_lock<mutex>lock(m_Mtx);
        q.pop();
    }
    bool empty(){
        unique_lock<mutex>lock(m_Mtx);
        return q.empty();
    }
};
class CMinSolver{
public:
    CMinSolver(){
        m_Solver_Min = createProgtestMinSolver();
    }
    void add_Polygon(AMyPolygon problem) {

        m_Solver_Min->addPolygon(problem->problem);

        m_min_problems.push_back(problem);
    }
    bool free_Capacity(){
        return m_Solver_Min->hasFreeCapacity();
    }
    vector<AMyPolygon> filledVector() {
        return m_min_problems;
    }
    AProgtestSolver getSolver() {
        return m_Solver_Min;
    }
    void newSolver() {
        m_Solver_Min = createProgtestMinSolver();
        m_min_problems.clear();
    }
    vector<AMyPolygon>m_min_problems;
    AProgtestSolver m_Solver_Min;
};
class CCntSolver{
public:
    CCntSolver(){
        m_Solver_Cnt = createProgtestCntSolver();
    }
    void add_Polygon(AMyPolygon problem){
        m_Solver_Cnt->addPolygon(problem->problem);
        m_cnt_problems.push_back(problem);
    }
    bool free_Capacity(){
        return m_Solver_Cnt->hasFreeCapacity();
    }
    vector<AMyPolygon> filledVector() {
        return m_cnt_problems;
    }
    AProgtestSolver getSolver() {
        return m_Solver_Cnt;
    }
    void newSolver() {
        m_Solver_Cnt = createProgtestCntSolver();
        m_cnt_problems.clear();
    }
    vector<AMyPolygon>m_cnt_problems;
    AProgtestSolver m_Solver_Cnt;
};
//class CCaseSolver{
//public:
//    CCaseSolver() {
//        m_Solver_Min = createProgtestMinSolver();
//        m_Solver_Cnt = createProgtestCntSolver();
//    }
//
//    bool add_Polygon(AMyPolygon problem) {
//        if(problem->m_type_min) {
//            if(problem->problem.get()) {
//                m_Solver_Min->addPolygon(problem->problem);
//            }
//            m_min_problems.push_back(problem);
//            return m_Solver_Min->hasFreeCapacity();
//        }
//        else {
//            if(problem->problem.get()) {
//                m_Solver_Cnt->addPolygon(problem->problem);
//            }
//            m_cnt_problems.push_back(problem);
//            return m_Solver_Cnt->hasFreeCapacity();
//        }
//
//    }
//    vector<AMyPolygon> filledVector() {
//        if(!m_Solver_Min->hasFreeCapacity()) {
//            return m_min_problems;
//        }
//        else {
//            return m_cnt_problems;
//        }
//    }
//    AProgtestSolver getSolver() {
//        if(!m_Solver_Min->hasFreeCapacity()) {
//            return m_Solver_Min;
//        }
//        else {
//            return m_Solver_Cnt;
//        }
//    }
//
//    void newSolver() {
//        if(!m_Solver_Min->hasFreeCapacity()) {
//            m_Solver_Min = createProgtestMinSolver();
//            m_min_problems.clear();
//        }
//        else {
//            m_Solver_Cnt = createProgtestCntSolver();
//            m_cnt_problems.clear();
//        }
//    }
//    bool hasSolvers(){
//        return !m_min_problems.empty() || !m_cnt_problems.empty();
//    }
//
//    pair<AProgtestSolver, vector<AMyPolygon>> getLastSolver() {
//        if(!m_min_problems.empty()) {
//            auto result = make_pair(m_Solver_Min, m_min_problems);
//            m_min_problems.clear();
//            return result;
//        }
//        else {
//            auto result = make_pair(m_Solver_Cnt, m_cnt_problems);
//            m_cnt_problems.clear();
//            return result;
//        }
//    }
//private:
//    AProgtestSolver m_Solver_Min;
//    AProgtestSolver m_Solver_Cnt;
//    vector<AMyPolygon>m_min_problems;
//    vector<AMyPolygon>m_cnt_problems;
//};

class CCaseCompany{
public:
    CCaseCompany(size_t id, ACompany companies, atomic<bool> & stop, condition_variable & worker_cv)
            : m_id_company(id), m_id_pack(0), m_companies(std::move(companies)), m_stop(stop), m_worker_cv(worker_cv){}

    void start_Workers (CCaseFront & opt ){
        m_receive = thread ( &CCaseCompany::receiver, this, ref(opt) );
        m_return = thread ( &CCaseCompany::returner, this );
    }

    void receiver (CCaseFront &opt){
        while(true) {
            AProblemPack curr = m_companies->waitForPack();
            if(!curr.get()){
                break;
            }
            m_map.add_problems(m_id_pack, curr);
            for(auto next : curr->m_ProblemsMin){
                opt.add_front(make_shared<CMyPolygon>(m_id_company, m_id_pack, next, true));
            }
            for(auto next : curr->m_ProblemsCnt){
                opt.add_front(make_shared<CMyPolygon>(m_id_company, m_id_pack, next, false));
            }
            if(curr->m_ProblemsMin.empty() && curr->m_ProblemsCnt.empty()) {
                unique_lock<mutex>lock(m_Mtx);
                m_queue.push(m_id_pack);
                q_CV.notify_one();
            }
            m_id_pack++;
            m_worker_cv.notify_one();
        }
    }

    void returner (){
        while(true) {
            unique_lock<mutex> lock(m_Mtx);
            q_CV.wait(lock, [&](){return ((!m_queue.empty() && m_queue.top() == m_id_packReturn) || m_stop);});
            if(m_queue.empty() && m_stop) {
                break;
            }
            size_t curr_id = m_queue.top();
            m_queue.pop();
            m_id_packReturn++;
            auto result = m_map.solvedPackMapa(curr_id);
            m_companies->solvedPack( result);
        }
    }

    void to_Solve(AMyPolygon polygon){
        m_map.nextIndex(polygon);
        if(m_map.solved(polygon->m_id_pack)){
            size_t ind = polygon->m_id_pack;
            auto temp = m_map.m_problems[ind];
            unique_lock<mutex>lock(m_Mtx);
            m_queue.push(polygon->m_id_pack);
            q_CV.notify_one();
        }
    }
    size_t m_id_company;
    size_t m_id_pack;
    size_t m_id_packReturn;
    CMap m_map;
    ACompany m_companies;
    thread m_return;
    thread m_receive;
    mutex m_Mtx;
    condition_variable q_CV;
    priority_queue<size_t, vector<size_t>, greater<size_t>>m_queue;

    atomic<bool> & m_stop;
    condition_variable & m_worker_cv;
};

using ACaseCompany = shared_ptr<CCaseCompany>;

class COptimizer
{
public:
    COptimizer() : m_id_company(0), m_Stop(false), m_receive_stop(false) {}

    /*---------------POHUI----------------*/
    static bool                        usingProgtestSolver                     ( void )
    {
        return true;
    }
    static void                        checkAlgorithmMin                       ( APolygon                              p )
    {
        // dummy implementation if usingProgtestSolver() returns true
    }
    static void                        checkAlgorithmCnt                       ( APolygon                              p )
    {
        // dummy implementation if usingProgtestSolver() returns true
    }
    /*---------------POHUI----------------*/

    void                               start                                   ( int                                   threadCount ) {

        for ( int i = 0; i < threadCount; i++ )
            m_threads.emplace_back ( &COptimizer::worker, this );

        for ( const auto & company : m_companies )
            company->start_Workers( buffer );
    }
    void                               stop                                    (                                                   ) {

        for ( auto & company : m_companies ) {
            company->m_receive.join();
        }
        m_receive_stop = true;
        workers_CV.notify_one();
        for ( auto & worker : m_threads )
            worker.join();

        m_Stop = true;
        for ( auto & company : m_companies ) {
            company->q_CV.notify_one();
            company->m_return.join();
        }
    }
    void                                addCompany                              ( const ACompany& company                           ) {
        m_companies.emplace_back ( std::make_shared<CCaseCompany> (m_id_company ,company, m_Stop, workers_CV ) );
        m_id_company++;
    }
    size_t m_id_company;
    atomic<bool> m_Stop;
    atomic<bool> m_receive_stop;
    vector<std::thread>m_threads;
    CCaseFront buffer;
    CMinSolver solver_Min;
    CCntSolver solver_Cnt;
    mutex m_Mtx;
    condition_variable workers_CV;
private:
    vector<ACaseCompany> m_companies;
    void worker () {
        while(true){
            unique_lock<mutex>lock(m_Mtx);
            workers_CV.wait(lock, [&](){return !buffer.empty() || m_receive_stop;});
            if(buffer.empty() && m_receive_stop && (!solver_Min.m_min_problems.empty() || !solver_Cnt.m_cnt_problems.empty())) {
                if(!solver_Min.m_min_problems.empty()){
                    auto oldSolver = solver_Min.m_Solver_Min;
                    auto minProblem = solver_Min.m_min_problems;
                    solver_Min.newSolver();
                    lock.unlock();
                    oldSolver->solve();
                    for(auto polygon : minProblem) {
                        m_companies[polygon->m_id_company]->to_Solve(polygon);
                    }
                    continue;
                }
                else if(!solver_Cnt.m_cnt_problems.empty()) {
                    auto oldSolver = solver_Cnt.m_Solver_Cnt;
                    auto cntProblem = solver_Cnt.m_cnt_problems;
                    solver_Cnt.newSolver();
                    lock.unlock();
                    oldSolver->solve();
                    for(auto polygon : cntProblem) {
                        m_companies[polygon->m_id_company]->to_Solve(polygon);
                    }
                    continue;
                }
            }
            else if(buffer.empty() && m_receive_stop && solver_Cnt.m_cnt_problems.empty() && solver_Min.m_min_problems.empty()) {
                workers_CV.notify_one();
                break;
            }
            while(!buffer.empty()){
                AMyPolygon curr = buffer.get_front();
                buffer.pop_front();

                if(curr->m_type_min) {
                    solver_Min.add_Polygon(curr);
                    if(!solver_Min.free_Capacity()) {
                        auto oldSolver = solver_Min.m_Solver_Min;
                        auto minProblem = solver_Min.m_min_problems;
                        solver_Min.newSolver();
                        workers_CV.notify_one();
                        lock.unlock();
                        oldSolver->solve();
                        for(auto polygon : minProblem) {
                            m_companies[polygon->m_id_company]->to_Solve(polygon);
                        }
                        break;
                    }
                }
                else {
                    solver_Cnt.add_Polygon(curr);
                    if(!solver_Cnt.free_Capacity()) {
                        auto oldSolver = solver_Cnt.m_Solver_Cnt;
                        auto cntProblem = solver_Cnt.m_cnt_problems;
                        solver_Cnt.newSolver();
                        workers_CV.notify_one();
                        lock.unlock();
                        oldSolver->solve();
                        for (auto polygon: cntProblem) {
                            m_companies[polygon->m_id_company]->to_Solve(polygon);
                        }
                        break;
                    }
                }

            }
        }
    }
};

// TODO: COptimizer implementation goes here
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __PROGTEST__
int                                    main                                    ( void )
{
    COptimizer optimizer;
    ACompanyTest  company = std::make_shared<CCompanyTest> ();
    optimizer . addCompany ( company );
    optimizer . start ( 4 );
    optimizer . stop  ();
    if ( ! company -> allProcessed () )
        throw std::logic_error ( "(some) problems were not correctly processsed" );
    return 0;
}
#endif /* __PROGTEST__ */
