#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <fstream>
#include <pthread.h>
#include <unistd.h>
#include <set>
#include <cmath>
#include <queue>
#include <map>
#include <unordered_map>
#include <signal.h>
#include <execinfo.h>
#include <assert.h>
#include <queue>
#include <algorithm>
#include <functional>
using namespace std;

#define NUM_NODES 37700
#define NUM_READ_POST_THREADS 2
#define NUM_PUSH_UPDATE_THREADS 2

struct Action{
    int user_id;
    int action_id;
    string action_type;
    time_t action_time;
};
map<int, map<int, int>> commonNeighbours;
// int commonNeighbours[NUM_NODES][NUM_NODES];
set<int> feed_added;

enum comparatorType {chronological, priority_based};
comparatorType get_rand_comp(){
    return (comparatorType)(rand()%2);
}
class ParentComp{
    public:
    virtual bool operator()(const Action& a, const Action& b) = 0;
};

class chronologicalComp : public ParentComp{
public:  
    bool operator()(const Action& a, const Action& b) {
        return a.action_time < b.action_time;
    }
};

class priorityComp : public ParentComp{
    int node_id;
public:
    priorityComp(int v){
        node_id = v;
    }
    bool operator()(const Action& a, const Action& b) {
        return commonNeighbours[node_id][a.user_id] < commonNeighbours[node_id][b.user_id];
    }
};
ParentComp* get_comparator(comparatorType comparator,int node_id){
    if(comparator == chronological){
        return new chronologicalComp();
    }
    else if (comparator == priority_based){
        return new priorityComp(node_id);
    }
}
class Node{
    public:
    queue<Action> wall_queue;
    ParentComp* comparator_;
    std::priority_queue<Action, vector<Action>, std::function<bool(Action&, Action&)>> feed_queue{
    [this](Action& a, Action& b) { return comparator_->operator()(a, b); }
    };
    int action_cntr[3]; // 0:post, 1:comment, 2:like
    // priority_queue<Action, vector<Action>, ParentComp*> feed_queue;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    Node(int node_id,comparatorType comparator): comparator_(get_comparator(comparator,node_id)){
        action_cntr[0] = 0;
        action_cntr[1] = 0;
        action_cntr[2] = 0;
    }
   
};

Node* nodes[NUM_NODES];
vector<int> adj[NUM_NODES];

queue<Action> action_queue; // shared b/w userSimulator and pushUpdate
pthread_mutex_t action_queue_mutex = PTHREAD_MUTEX_INITIALIZER; // locks action_queue
pthread_mutex_t feed_queue_mutex = PTHREAD_MUTEX_INITIALIZER; // locks feed_queue
// pthread_cond_t user_simulator_cv = PTHREAD_COND_INITIALIZER; // userSimulator waits on this
pthread_cond_t push_update_cv = PTHREAD_COND_INITIALIZER;   // pushUpdate waits on this
pthread_cond_t read_post_cv = PTHREAD_COND_INITIALIZER; // readPost waits on this





void create_graph(string path){
    ifstream file(path);
    if(!file.is_open()) {
        cout << "Error opening file" << endl;
        exit(1);
    }
    int x=0, y=0;
    char dummy[10], c; // c consumes the comma
    file >> dummy;
    while(file >> x >> c >> y) {
        adj[x].push_back(y);
        adj[y].push_back(x);

    }
    file.close();
    for(int i=0; i<NUM_NODES; i++){\
        // nodes[i] = new Node(i, get_rand_comp());
        nodes[i] = new Node(i, chronological);
    }
}

void getCommonNeighbours(){
    // for every pair of neighbouring nodes in the graph,
    // find the number of common neighbours
    // takes 1m26s 
    for(int i=0; i<NUM_NODES; i++){
        for(auto nei: adj[i]){
            int val = 0;
            if(commonNeighbours[i][nei] == 0 && commonNeighbours[nei][i] == 0){
                for(auto n1: adj[i]){
                    for(auto n2: adj[nei]){
                        if(n1 == n2) val++;
                    }
                }
            }
            else
                val = max(commonNeighbours[i][nei], commonNeighbours[nei][i]);
            commonNeighbours[i][nei] = val;
            commonNeighbours[nei][i] = val;

            // cout << val;
        }
        // cout << endl;
    }
}

void* userSimulator(void* arg) {
    int sleep_time = 4; // in seconds
    ofstream log_file;
    log_file.open("sns.log", ios::out | ios::app);
    if(!log_file.is_open()) {
        cout << "Error opening file" << endl;
        pthread_exit(NULL);
    }
    log_file << "User Simulator started" << endl;
    while(1){
        log_file << "Selected 100 random nodes" << endl;
        set<int> selected_nodes;
        while(selected_nodes.size() < 100){
            int node_id = rand()%NUM_NODES; // random number between 1 and 37700
            selected_nodes.insert(node_id);
        }
        for(auto node_id: selected_nodes){
            log_file<< "Node " << node_id << endl;
            int degree = adj[node_id].size();
            int num_actions = ceil(log2(degree));
            log_file << "Degree: " << degree << " Num actions: " << num_actions << endl;
            for(int i=0; i<num_actions; i++){
                int action_type = rand()%3;
                Action action;
                action.user_id = node_id;
                nodes[node_id]->action_cntr[action_type]++;
                action.action_id = nodes[node_id]->action_cntr[action_type];
                action.action_time = time(NULL);
                if(action_type == 0){
                    action.action_type = "post";
                }else if(action_type == 1){
                    action.action_type = "comment";
                }else{
                    action.action_type = "like";
                }
                log_file << "Action type: " << action.action_type << " Action id: " << action.action_id << endl;
                pthread_mutex_lock(&action_queue_mutex); // lock action_queue
                // cout<<"entering critical section of userSimulator"<<endl;
                action_queue.push(action);         // push action to action_queue
                pthread_cond_signal(&push_update_cv);   // signal pushUpdate
                // cout<<"exiting critical section of userSimulator"<<endl;
                pthread_mutex_unlock(&action_queue_mutex);  // unlock action_queue
                // nodes[node_id].wall_queue.push_back(action);
                nodes[node_id]->wall_queue.push(action);
            }
            log_file << endl;
        }
        sleep(sleep_time);
    }  
    log_file.close();
    pthread_exit(NULL); 
}

void* pushUpdate(void* arg) {
    ofstream log_file;
    log_file.open("sns.log", ios::out | ios::app);
    if(!log_file.is_open()) {
        cout << "Error opening file" << endl;
        pthread_exit(NULL);
    }
    log_file << "Push Update started" << endl;
    while(1){
        pthread_mutex_lock(&action_queue_mutex); // lock action_queue
        // cout<<"entering critical section of pushUpdate"<<endl;
        while(action_queue.empty()){
            pthread_cond_wait(&push_update_cv, &action_queue_mutex); // wait for signal from userSimulator
        }
        // cout<<"Size of action_queue: "<<action_queue.size()<<" accessed by pushUpdate thread";
        log_file<<"Push update: action_queue not empty"<<endl;
        Action action = action_queue.front(); // get action from action_queue
        action_queue.pop(); // remove action from action_queue
        cout<<"exiting critical section of pushUpdate"<<endl;
        pthread_mutex_unlock(&action_queue_mutex); // unlock action_queue


        for(auto node_id: adj[action.user_id]){
            pthread_mutex_lock(&feed_queue_mutex); // lock node
            feed_added.insert(node_id);
            pthread_mutex_unlock(&feed_queue_mutex);
            pthread_mutex_lock(&nodes[node_id]->mutex); // lock node
            nodes[node_id]->feed_queue.push(action); // push action to feed_queue
            pthread_cond_signal(&read_post_cv); // signal readPost
            // pthread_cond_signal(&push_update_cv); 
            pthread_mutex_unlock(&nodes[node_id]->mutex);
            cout<<"exiting critical section of pushUpdate for node "<<node_id<<endl;
            log_file<<"Push update: action pushed to node "<<node_id<<endl;
        }
    }   
    log_file.close();
    pthread_exit(NULL);
}


void* readPost(void* arg) {
    // detect changes in feed_queue and print them
    ofstream log_file;
    log_file.open("sns.log", ios::out | ios::app);
    if(!log_file.is_open()) {
        cout << "Error opening file" << endl;
        pthread_exit(NULL);
    }
    log_file << "Read Post started" << endl;
    while(1){
        pthread_mutex_lock(&feed_queue_mutex); // lock feed_queue for all nodes
        cout<<"entering critical section of readPost by thread "<<pthread_self()<<endl;
        while(feed_added.empty()){
            pthread_cond_wait(&read_post_cv, &feed_queue_mutex); // wait for signal from pushUpdate
        }
        log_file<<"Read post: feed_added not empty"<<endl;
        auto it = feed_added.begin();
        int node_id = *it;
        feed_added.erase(it);
        cout<<"exiting critical section of readPost by thread "<<pthread_self()<<endl;
        pthread_mutex_unlock(&feed_queue_mutex); // unlock feed_queue for all nodes
        cout<<"thread "<<pthread_self()<<" reading feed_queue of node "<<node_id<<endl;
        while(1){
            Action action;
            pthread_mutex_lock(&nodes[node_id]->mutex); // lock node
            if(!nodes[node_id]->feed_queue.empty()){
                action = nodes[node_id]->feed_queue.top(); // get action from feed_queue
                // it.second.pop(); // remove action from feed_queue
                nodes[node_id]->feed_queue.pop();// remove action from feed_queue
            }
            else{
                pthread_mutex_unlock(&nodes[node_id]->mutex); // unlock node
                break;
            }
            pthread_mutex_unlock(&nodes[node_id]->mutex); // unlock node
            string time_str = ctime(&action.action_time);
            time_str.pop_back();
            string log_gen = "I read action number " + to_string(action.action_id) + " of type " + action.action_type + " by user " + to_string(action.user_id) + " at time " + time_str;
            cout<<log_gen<<endl;
            log_file<<log_gen<<endl;
        }
        
    }
    log_file.close();
    pthread_exit(NULL);
}



void sigsegv_handler(int sig){
    // print stack trace with line numbers
    void *array[10];
    size_t size;
    char **strings;
    size_t i;
    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);
    printf("Obtained %zd stack frames. Stack trace:\n", size);
    for (i = 0; i < size; i++)
        printf("%s\n", strings[i]);
    free(strings);
    exit(1);
}

int main(){
    // signal(SIGSEGV, sigsegv_handler);
    srand(time(NULL));
    create_graph("musae_git_edges.csv");
    cout<<"Graph created"<<endl;
    // getCommonNeighbours();
    pthread_t userSimulatorThread;

    pthread_create(&userSimulatorThread, NULL, userSimulator, NULL);
    vector<pthread_t> readPostThreads;
    for(int i=0; i<NUM_READ_POST_THREADS; i++){
        pthread_t readPostThread;
        pthread_create(&readPostThread, NULL, readPost, NULL);
        readPostThreads.push_back(readPostThread);
    }

    vector<pthread_t> pushUpdateThreads;
    for(int i=0; i<NUM_PUSH_UPDATE_THREADS; i++){
        pthread_t pushUpdateThread;
        pthread_create(&pushUpdateThread, NULL, pushUpdate, NULL);
        pushUpdateThreads.push_back(pushUpdateThread);
    }

    pthread_join(userSimulatorThread, NULL);
    for(int i=0; i<NUM_READ_POST_THREADS; i++){
        pthread_join(readPostThreads[i], NULL);
    }
    for(int i=0; i<NUM_PUSH_UPDATE_THREADS; i++){
        pthread_join(pushUpdateThreads[i], NULL);
    }

    pthread_mutex_destroy(&action_queue_mutex);
    pthread_cond_destroy(&push_update_cv);
    pthread_cond_destroy(&read_post_cv);
    pthread_mutex_destroy(&feed_queue_mutex);

    // for(int i=0; i<NUM_NODES; i++){
    //     pthread_mutex_destroy(&nodes[i].mutex);
    // }

    return 0;
}