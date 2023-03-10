#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>
#include <set>
#include <cmath>
#include <queue>
#include <map>
#include <queue>
#include <algorithm>
#include <functional>
#include <sstream>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

using namespace std;

#define NUM_NODES 37700
#define NUM_READ_POST_THREADS 10
#define NUM_PUSH_UPDATE_THREADS 25
#define SLEEP_TIME 4

struct Action{
    int user_id;
    int action_id;
    string action_type;
    time_t action_time;
};

// number of common neighbours for each edge
map<int, map<int, int>> commonNeighbours;
// stores node IDs for which feed queues have been updated
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
ParentComp* get_comparator(comparatorType comparator, int node_id){
    if(comparator == chronological){
        return new chronologicalComp();
    }
    else if (comparator == priority_based){
        return new priorityComp(node_id);
    }
    return NULL;
}
class Node{
    public:
    std::queue<Action> wall_queue;
    ParentComp* comparator_;
    std::priority_queue<Action, vector<Action>, std::function<bool(Action&, Action&)>> feed_queue{
    [this](Action& a, Action& b) { return comparator_->operator()(a, b); }
    };
    int action_cntr[3]; // 0:post, 1:comment, 2:like
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    Node(int node_id, comparatorType comparator): comparator_(get_comparator(comparator,node_id)){
        action_cntr[0] = 0;
        action_cntr[1] = 0;
        action_cntr[2] = 0;
    }
};

// Graph definition
Node* nodes[NUM_NODES];
vector<int> adj[NUM_NODES];

queue<Action> action_queue; // shared b/w userSimulator and pushUpdate
pthread_mutex_t action_queue_mutex = PTHREAD_MUTEX_INITIALIZER; // locks action_queue
pthread_mutex_t feed_queue_mutex = PTHREAD_MUTEX_INITIALIZER; // locks feed_queue
pthread_mutex_t log_file_mutex = PTHREAD_MUTEX_INITIALIZER; // locks writes to log_file
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
    for(int i=0; i<NUM_NODES; i++){
        // nodes[i] = new Node(i, get_rand_comp());
        nodes[i] = new Node(i, chronological);
    }
}

void getCommonNeighbours(){
    /* for every pair of neighbouring nodes in the graph,
        find the number of common neighbours
    */
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
        }
    }
}

void* userSimulator(void* arg) {
    stringstream temp_log;
    // temp_log << "----------User Simulator----------\n";
    while(1){ 
        temp_log << "Selected 100 random nodes\n";
        set<int> selected_nodes;
        while(selected_nodes.size() < 100){
            int node_id = rand()%NUM_NODES + 1; // random number between 1 and 37700
            selected_nodes.insert(node_id);
        }
        for(auto node_id: selected_nodes){
            temp_log << "Node: " << node_id << ", ";
            int degree = adj[node_id].size();
            int num_actions = 10*(1 + ceil(log2(degree)));
            temp_log << "Degree: " << degree << ", Num actions: " << num_actions << "\n";
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
                temp_log << "Action type: " << action.action_type << " Action id: " << action.action_id << "\n";
                pthread_mutex_lock(&action_queue_mutex);    // lock action_queue
                action_queue.push(action);                  // push action to action_queue
                pthread_cond_signal(&push_update_cv);       // signal pushUpdate
                pthread_mutex_unlock(&action_queue_mutex);  // unlock action_queue

                nodes[node_id]->wall_queue.push(action);
            }
            temp_log << "\n";
        }
        pthread_mutex_lock(&log_file_mutex);
        ofstream log_file;
        log_file.open("sns.log", ios::out | ios::app);
        log_file << temp_log.rdbuf();
        if(!log_file.is_open()) {
            cout << "Error opening file" << endl;
            pthread_exit(NULL);
        }
        log_file.close();
        pthread_mutex_unlock(&log_file_mutex);
        temp_log.clear();

        sleep(SLEEP_TIME);
    }  
    pthread_exit(NULL); 
}

void* pushUpdate(void* arg) {
    stringstream temp_log;
    // temp_log << "Push Update started\n";
    while(1){
        pthread_mutex_lock(&action_queue_mutex); // lock action_queue
        while(action_queue.empty()){
            pthread_cond_wait(&push_update_cv, &action_queue_mutex); // wait for signal from userSimulator
        }
        assert(!action_queue.empty());

        temp_log << "Push update: action_queue not empty\n";
        Action action = action_queue.front(); // get action from action_queue
        action_queue.pop(); // remove action from action_queue
        // cout<<"exiting critical section of pushUpdate"<<endl;
        pthread_mutex_unlock(&action_queue_mutex); // unlock action_queue

        for(auto node_id: adj[action.user_id]){
            pthread_mutex_lock(&feed_queue_mutex); // lock node
            feed_added.insert(node_id);
            pthread_mutex_unlock(&feed_queue_mutex);

            pthread_mutex_lock(&nodes[node_id]->mutex); // lock node
            nodes[node_id]->feed_queue.push(action); // push action to feed_queue
            pthread_cond_signal(&read_post_cv); // signal readPost
            pthread_mutex_unlock(&nodes[node_id]->mutex);

            // cout<<"exiting critical section of pushUpdate for node "<<node_id<<endl;
            temp_log << "Push update: action pushed to node " << node_id << "\n";
        }
        temp_log << "\n";

        pthread_mutex_lock(&log_file_mutex);
        ofstream log_file;
        log_file.open("sns.log", ios::out | ios::app);
        log_file << temp_log.rdbuf();
        if(!log_file.is_open()) {
            cout << "Error opening file" << endl;
            pthread_exit(NULL);
        }
        log_file.close();
        pthread_mutex_unlock(&log_file_mutex);
        temp_log.clear();
    }   
    pthread_exit(NULL);
}

void* readPost(void* arg) {
    // detect changes in feed_queue and print them
    stringstream temp_log;
    
    // temp_log << "Read Post started\n";
    while(1){
        pthread_mutex_lock(&feed_queue_mutex); // lock feed_queue for all nodes
        cout<<"entering critical section of readPost by thread " << pthread_self() << endl;
        while(feed_added.empty()){
            pthread_cond_wait(&read_post_cv, &feed_queue_mutex); // wait for signal from pushUpdate
        }
        assert(!feed_added.empty());
        temp_log << "Read post: feed_added not empty\n";
        auto it = feed_added.begin();
        int node_id = *it;
        feed_added.erase(it);
        cout<<"exiting critical section of readPost by thread " << pthread_self() << endl;
        pthread_mutex_unlock(&feed_queue_mutex); // unlock feed_queue for all nodes

        cout<<"thread "<<pthread_self()<<" reading feed_queue of node "<<node_id<<endl;
        while(1){
            Action action;
            pthread_mutex_lock(&nodes[node_id]->mutex); // lock node
            if(!nodes[node_id]->feed_queue.empty()){
                action = nodes[node_id]->feed_queue.top(); // get action from feed_queue
                nodes[node_id]->feed_queue.pop();// remove action from feed_queue
            }
            else{
                pthread_mutex_unlock(&nodes[node_id]->mutex); // unlock node
                break;
            }
            pthread_mutex_unlock(&nodes[node_id]->mutex); // unlock node
            string time_str = ctime(&action.action_time);
            time_str.pop_back();
            temp_log << "Node ID: " << node_id << " reads action number " << action.action_id << " of type " << action.action_type << " by user " << action.user_id << " at time " << time_str << "\n";
        }

        pthread_mutex_lock(&log_file_mutex);
        ofstream log_file;
        log_file.open("sns.log", ios::out | ios::app);
        log_file << temp_log.rdbuf();
        if(!log_file.is_open()) {
            cout << "Error opening file" << endl;
            pthread_exit(NULL);
        }
        log_file.close();
        pthread_mutex_unlock(&log_file_mutex);
        temp_log.clear();
    }
    pthread_exit(NULL);
}

int main(){
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

    pthread_mutex_destroy(&log_file_mutex);
    pthread_mutex_destroy(&action_queue_mutex);
    pthread_cond_destroy(&push_update_cv);
    pthread_cond_destroy(&read_post_cv);
    pthread_mutex_destroy(&feed_queue_mutex);

    return 0;
}