#include <iostream>
#include <queue>
#include <functional>
using namespace std;
// Define the abstract comparator class
class AbstractComparator {
public:
    virtual bool IsLessThan(int a, int b) const = 0;
};

// Define a concrete comparator subclass that compares based on chronological order
class ChronologicalComparator : public AbstractComparator {
public:
    bool IsLessThan(int a, int b) const override {
        return a < b;
    }
};

// Define a concrete comparator subclass that compares based on priority
class PriorityBasedComparator : public AbstractComparator {
public:
    bool IsLessThan(int a, int b) const override {
        return a > b;
    }
};

// Define the Node class with a priority queue that uses an abstract comparator
class Node {
private:
AbstractComparator* comparator_;
std::priority_queue<int, std::vector<int>, std::function<bool(int, int)>> priority_queue_{
    [this](int a, int b) { return comparator_->IsLessThan(a, b); }
};
public:
    Node(AbstractComparator* comparator) : comparator_(comparator) {}

    void AddValue(int value) {
        priority_queue_.push(value);
    }

    int GetTopValue() {
        int top_value = priority_queue_.top();
        priority_queue_.pop();
        return top_value;
    }


};

int main() {
    ChronologicalComparator chronological_comparator;
    PriorityBasedComparator priority_based_comparator;

    Node chronological_node(&chronological_comparator);
    Node priority_based_node(&priority_based_comparator);

    chronological_node.AddValue(2);
    chronological_node.AddValue(1);
    int top_chronological = chronological_node.GetTopValue();
    cout << top_chronological << endl;
    // 'top_chronological' should be 1 because we're using a chronological comparator

    priority_based_node.AddValue(2);
    priority_based_node.AddValue(1);
    int top_priority_based = priority_based_node.GetTopValue();
    cout << top_priority_based << endl;
    // 'top_priority_based' should be 2 because we're using a priority-based comparator

    return 0;
}
