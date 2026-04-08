
#pragma once
#include <iostream>
#include<list>
#include<stack>
#include<queue>
#include <cmath>
using namespace std;


struct TrafficSignal {

    int greentimer;
    int minGreenTime;
    int queueThreshold;
    int pa, pb;
    TrafficSignal() {
        greentimer = 0;
        minGreenTime = 5;    // Default Tg = 10 steps
        queueThreshold = 5;
        pa = 1; pb = 10;// Default threshold
    }

    void Timer(bool state, int time) {
        if (state == true) {
            greentimer = greentimer + time;
        }
    }
    void turnGreen(bool& state) {
        state = true;
        greentimer = 0;
    }
    void turnRed(bool& state) {
        state = false;
        greentimer = 0;
    }

    bool canSwitch(int currentQueue) const {
        return (greentimer >= minGreenTime || currentQueue < queueThreshold);
    }
};

struct RoadDetails {
    float length, max_speed, capacity;
    int currentVehicles;
    int queueCount;
    bool signalState;
    float NonIdealWeight;
    float a, b;
    TrafficSignal light;

    int dischargeCapcity;
    bool operator<( const RoadDetails& other)  const {
        return this->calculateWeight() < other.calculateWeight();
    }

    float calculateWeight() const {
        float best = length / max_speed;
        if (capacity <= 0) return 1000000; // Safety for division
        return best * (1.0f + a * pow((float)currentVehicles / capacity, b));
    }
    RoadDetails(float ab, float bc, float c, float aplha = 0.5, float beta = 4,int discharge=5) :a(aplha), b(beta),dischargeCapcity(discharge) {
        currentVehicles = 0;
        length = ab;
        max_speed = bc;
        capacity = c;
        queueCount = 0;
        signalState = true;

    }
    float DischargeAllowed(int capacity_ofnext_road, int pop_of_next_road) {
        float allowed = queueCount;
        float avail = capacity_ofnext_road - pop_of_next_road;
        if (avail > allowed) {
            if (dischargeCapcity <= allowed) {
                return signalState * dischargeCapcity;
            }
            else {
                return signalState * queueCount;
            }
        }
        else { return avail * signalState; }
    }
    float TimeCal() {
        if (capacity <= 0)
        {
            return bestTime();
        }
        NonIdealWeight = NonIdealtime();
        return NonIdealWeight;
    }

    float bestTime() {
        return length / max_speed;
    }

    float NonIdealtime() {
        if (capacity <= 0)
        {
            return bestTime();
        }
        NonIdealWeight = bestTime();
        NonIdealWeight = NonIdealWeight * (1 + a * pow((float)currentVehicles / capacity, b));
        return NonIdealWeight;
    }
    void vehicleEntersRoad() {
        if (currentVehicles < capacity) {
            currentVehicles++;
            NonIdealtime();
        }
    }
    void vehicleExitsRoad() {
        if (currentVehicles >0) {
            currentVehicles--;
            NonIdealtime();
        }
    }


    void updateVehicles() {
        queueCount++;
    }

    void vehicleJoinsQueue() {
        if (queueCount < capacity) {
            queueCount++;
            NonIdealtime();
        }
    }
    void vehicleExitsQueue() {
        if (queueCount > 0 && signalState) {
            queueCount--;
        }
    }
    void decVehicles() {
        queueCount--;
    }

    float Congestion() {
        if (capacity <= 0)
        {
            return 0;
        }
        return (float)currentVehicles / capacity;
    }

    void Time(int time) {

        light.Timer(signalState, time);
    }

    void changeState() {
        if (signalState) {
            light.turnRed(signalState);
        }
        else {
            light.turnGreen(signalState);
        }
    }
    void change_to_red() {
       
            light.turnRed(signalState);
        
        
    }
    bool switching () {
       return light.canSwitch(queueCount);
    }
    void increment(int time) {
         light.Timer(signalState, time);

    }

    float choosing() {
        float currentCost = (light.pa * queueCount) + (light.pb * pow(Congestion(), 2));
        return currentCost;
    }
};

struct weighted {
    int index;
    RoadDetails weight;
    bool operator==(const weighted& other) const {
        return index == other.index;
    }
};
template <class t>
struct Gnode {
    t vertex;
    list<weighted> Neighbors;
    bool operator==(t v) {
        return vertex == v;
    }


};

template <class t, int size>
class Graph {
    
    Gnode<t> array[size];
    bool directed;
public:
    int Vcount;
    Graph() {
        Vcount = 0;
        directed = false;
    }
    Graph(bool x) {
        Vcount = 0;
        directed = x;
    }

    void bfs(int levels[size]) {
        if (Vcount <= 0) { return; }
        int visited[size] = { 0 };
        queue<int> adjacent;
        int starting = getIndex(array[0].vertex);
        adjacent.push(starting);
        visited[starting] = 1;

        levels[starting] = 0;

        while (!adjacent.empty()) {
            int top = adjacent.front();
            cout << array[top].vertex << "\t";
            adjacent.pop();

            typename list<weighted>::iterator temp = array[top].Neighbors.begin();
            while (temp != array[top].Neighbors.end()) {
                int neighborIndex = temp->index;
                if (visited[neighborIndex] != 1) {
                    visited[neighborIndex] = 1;
                    adjacent.push(neighborIndex);
                    levels[neighborIndex] = levels[top] + 1;
                }
                temp++;
            }
        }
        
        cout << endl;
    }


    void PrintLevels() {
        int levels[size];
        for (int i = 0; i < size; i++) {
            levels[i] = -1;
        }
        bfs(levels);
        int max = 0;
        
        for (int i = 0; i < Vcount; i++) {
            if (max < levels[i]) {
                max = levels[i];
            }
        }

        for (int i = 0; i <= max; i++) {
            cout << "\nLevel = " << i << endl;
            for (int j = 0; j < Vcount; j++) {
                if (levels[j] == i) {
                    cout << "\t" << array[j].vertex << "\t";
                }
            }
        }
    }


    void insertVertex(t vertex) {
        if (Vcount < size) {
            array[Vcount].vertex = vertex;
            Vcount++;
        }
        else { return; }
    }


        void makeEdge(int a, int b, float len, float speed, float cap) {
        RoadDetails road(len, speed, cap);
        if (!directed) {
            if (a < size && b < size && a >= 0 && b >= 0) {
                weighted tempA = { b, road };
                weighted tempB = { a, road };
                array[a].Neighbors.push_back(tempA);
                array[b].Neighbors.push_back(tempB);
            }
        }
        else {
            if (a < size && b < size && a >= 0 && b >= 0) {
                weighted tempA = { b, road };
                array[a].Neighbors.push_back(tempA);
            }
        }
    }
    int getIndex(t label) {
        for (int i = 0; i < Vcount; i++) {
            if (array[i].vertex == label) return i;
        }
        return -1; // Not found
    }

    int getVertex() {
        return Vcount;
    }



    bool isEmpty() {
        return Vcount <= 0;
    }



    int No_of_edges_btw_2_vertices(t data, t data2) {
        int a = getIndex(data);
        int b = getIndex(data2);
        int links = 0;
        if (a == -1 || b == -1) { return 0; }

        typename list<weighted>::iterator temp = array[a].Neighbors.begin();
        typename list<weighted>::iterator temp1 = array[b].Neighbors.begin();
        while (temp != array[a].Neighbors.end()) {
            if (temp->index == b) {
                links++;
            }
            temp++;
        }
        while (temp1 != array[b].Neighbors.end()) {
            if (temp1->index == a) {
                links++;
            }
            temp1++;
        }if (!directed) {
            return links / 2;
        }
        else {
            return links;
        }
    }
    void DeleteEdge(t data1, t data2) {
        int a = getIndex(data1);
        int b = getIndex(data2);
        if (a == -1 || b == -1) return;


        weighted B;
        B.index = b;
        weighted A; 
        A.index = a;

        if (!directed) {
            array[a].Neighbors.remove(B);
            array[b].Neighbors.remove(A);
        }
        else {
            array[a].Neighbors.remove(B);
        }

        cout << "\t\t\t\t\t\tDeleted\n";
    }
    void DeleteVertex(t data) {
        int index = getIndex(data);
        if (index == -1) return;
        weighted d = { index, 0 };

        for (int i = 0; i < Vcount; i++) {
            array[i].Neighbors.remove(d);
        }

        for (int i = index; i < Vcount - 1; i++) {
            array[i] = array[i + 1];
        }

        Vcount--;

        for (int i = 0; i < Vcount; i++) {
            typename list<weighted>::iterator it = array[i].Neighbors.begin();
            while (it != array[i].Neighbors.end()) {
                if (it->index > index) {
                    it->index = it->index - 1;
                }
                it++;
            }
        }
    }

    void dfs() {
        stack<int> holder;
        bool visited[size] = { false };
        t data = array[0].vertex;
        int start = getIndex(data);
        holder.push(start);
        visited[start] = true;

        while (!holder.empty()) {
            int index = holder.top();
            cout << array[index].vertex << "\t";
            holder.pop();
            typename list<weighted>::iterator temp = array[index].Neighbors.begin();
            while (temp != array[index].Neighbors.end()) {
                if (visited[temp->index] == false) {
                    holder.push(temp->index);
                    visited[temp->index] = true;
                }
                temp++;
            }

        }
    }



    bool edgeExist(t data1, t data2) {
        int a = getIndex(data1);
        int b = getIndex(data2);
        if (a == -1 || b == -1) { return false; }

        typename list<weighted>::iterator temp = array[a].Neighbors.begin();
        while (temp != array[a].Neighbors.end()) {
            if (b == temp->index) {
                return true;
            }
            temp++;
        }
        return false;
    }

    int No_Of_Edges() {
        int edges = 0;
        for (int i = 0; i < Vcount; i++) {
            typename list<weighted>::iterator temp = array[i].Neighbors.begin();
            while (temp != array[i].Neighbors.end()) {
                edges++;
                temp++;
            }
        }
        if (!directed) {
            return edges / 2;
        }
        else {
            return edges;
        }
    }



    Graph minimumSpanningtree() {
        Graph<t, size> result;
        for (int i = 0; i < Vcount; i++) {
            result.insertVertex(array[i].vertex);
        }
        bool visited[size] = { false };
        visited[0] = true;
        for (int edges = 0; edges < Vcount - 1; edges++) {

            float minimum = 100000;
            int start = -1;
            int end = -1;

            for (int i = 0; i < Vcount; i++) {
                if (visited[i] == true) {

                    typename list<weighted>::iterator temp = array[i].Neighbors.begin();
                    while (temp != array[i].Neighbors.end()) {

                        if (temp->weight.calculateWeight() < minimum && visited[temp->index] == false) {
                            minimum = temp->weight.calculateWeight();
                            start = i;
                            end = temp->index;
                        }
                        temp++;
                    }
                }
            }

            if (end != -1) {
                visited[end] = true;

                RoadDetails* w = nullptr;

                typename list<weighted>::iterator temp = array[start].Neighbors.begin();
                while (temp != array[start].Neighbors.end()) {
                    if (temp->index == end) {
                        w = &(temp->weight);
                        break;
                    }
                    temp++;
                }

                if (w != nullptr) {
                    result.makeEdge(start, end, w->length, w->max_speed, w->capacity);
                }

                cout << "Inserting link btw " << start << "\t" << end
                    << "\t for a weight of " << minimum << endl;
            }
        }

        return result;
    }


    void Shortest_Link_btw_two_vertices(t data, t data1) {
        int a = getIndex(data);
        int b = getIndex(data1);
        bool direct = false;
        typename list<weighted>::iterator temp = array[a].Neighbors.begin();
        int min = 100000;
        int index = -1;
        while (temp != array[a].Neighbors.end()) {
            if (b == temp->index) {
                if (temp->weight < min) {
                    min = temp->weight;
                    index = temp->index;
                    direct = true;
                }

            }
            temp++;
        }
        if (direct) {
            cout << "The minimum distance between the vertes" << data << "\t" << data1 << "\t" << "is \t" << min;
        }
        else {
            cout << "No direct link";
        }
    }

    list<t> shortest_Path_btw2_vericex_returing_list(t data, t data2) {
        list <t> temp;
        int a = getIndex(data);//starting index
        int b = getIndex(data2);//target
        float distance[size];
        bool visited[size];
        int indexes[size];
        for (int i = 0; i < size; i++) {

            distance[i] = 1000000000;
            visited[i] = false;
            indexes[i] = -1;
        }


        distance[a] = 0;
        for (int i = 0; i < Vcount; i++) {
            int nextNode = -1;
            float min = 1000000000;
            for (int j = 0; j < Vcount; j++) {
                if (distance[j] < min && visited[j] == false) {
                    min = distance[j];
                    nextNode = j;
                }
            }
            if (nextNode == -1) {
                break;
            }

            else {
                visited[nextNode] = true;
            }

            for (auto it = array[nextNode].Neighbors.begin(); it != array[nextNode].Neighbors.end(); ++it) {
                int v = it->index;
                float weight = it->weight.NonIdealtime();

                if (!visited[v] && distance[nextNode] + weight < distance[v]) {
                    distance[v] = distance[nextNode] + weight;
                    indexes[v] = nextNode;
                }
            }

        }
        if (distance[b] == 1000000000) {
            
            return temp;
        }

        int current = b;
        int path[size];
        int count = 0;
        while (current != -1) {
            path[count] = current;
            count++;
            current = indexes[current];
        }

        for (int i = count - 1; i >= 0; i--) {
            int nodeIndex = path[i];
            temp.push_back(array[nodeIndex].vertex);
        }
        return temp;
    }

    void shortest_Path_btw2_vericex(t data, t data2) {

        int a = getIndex(data);//starting index
        int b = getIndex(data2);//target
        float distance[size];
        bool visited[size];
        int indexes[size];
        for (int i = 0; i < size; i++) {

            distance[i] = 1000000000;
            visited[i] = false;
            indexes[i] = -1;
        }


        distance[a] = 0;
        for (int i = 0; i < Vcount; i++) {
            int nextNode = -1;
            float min = 1000000000;
            for (int j = 0; j < Vcount; j++) {
                if (distance[j] < min && visited[j] == false) {
                    min = distance[j];
                    nextNode = j;
                }
            }
            if (nextNode == -1) {
                cout << "Path does not exist"; break;
            }
      
            else {
                visited[nextNode] = true;
            }

            for (auto it = array[nextNode].Neighbors.begin(); it != array[nextNode].Neighbors.end(); ++it) {
                int v = it->index;
                float weight = it->weight.NonIdealtime();

                if (!visited[v] && distance[nextNode] + weight < distance[v]) {
                    distance[v] = distance[nextNode] + weight;
                    indexes[v] = nextNode;
                }
            }

        }
        if (distance[b] == 1000000000) {
            cout << "No path exists\n";
            
        }

        int current = b;
        int path[size];
        int count = 0;
        while (current != -1) {
            path[count] = current;
            count++;
            current = indexes[current];
        }
        for (int i = count - 1; i >= 0; i--) {
            int nodeIndex = path[i];
            cout << array[nodeIndex].vertex << "\t";
        }
        cout << "\nTotal Travel Cost: " << distance[b] << " units" << endl;
    }

    bool searchVertex(t data) {
        return getIndex(data) != -1;
    }

    int getDegree(t data) {
        int a = getIndex(data);
        int degree = 0;
        typename list<weighted> ::iterator temp = array[a].Neighbors.begin();
        if (a == -1) { cout << "\nVertex does not exist \n"; return -1; }
        degree = array[a].Neighbors.size();
        cout << "\nThe degree of the vertex " << data << " is " << degree << endl;
        return degree;
    }

    void Type() {
        if (directed) {
            cout << "\nThe graph is Directed\n";
        }
        else {
            cout << "\nThe graph is Undirected\n";
        }
    }

    void display_edge_of_index(string s) {
        int index = getIndex(s);
        if (index == -1) { cout << "\nCity not found.\n"; return; } // Safety
        typename list<weighted>::iterator temp = array[index].Neighbors.begin();
        cout << "\nThe vertex ";
        cout << array[index].vertex << " has links to \n";
        while (temp != array[index].Neighbors.end()) {
            cout << array[temp->index].vertex << "\t";
            cout<<"length = "<<temp->weight.length<<endl;
            cout << "Current Vehicles = " << temp->weight.currentVehicles << endl;
            cout << "Signal State = " << temp->weight.signalState << endl;
            cout << "Congestion = " << temp->weight.Congestion() << endl;
            temp++;
        }

    }
    void Incoming(t target) {
        if (!directed) { return; }
        int targetIdx = getIndex(target);
        if (targetIdx == -1) return;

        cout << "Incoming flights to " << target << ":" << endl;
        for (int i = 0; i < Vcount; i++) {
            typename list<weighted>::iterator temp = array[i].Neighbors.begin();
            while (temp != array[i].Neighbors.end()) {
                if (temp->index == targetIdx) {
                    cout << array[i].vertex << "\t";
                }
                temp++;
            }

        }
    }

    RoadDetails& getEdgeDetails(t data, t data1) {
        int a = getIndex(data);
        int b = getIndex(data1);

        typename list<weighted>::iterator temp = array[a].Neighbors.begin();
        while (temp != array[a].Neighbors.end()) {
            if (temp->index == b) {
                return temp->weight;
            }
            temp++;
        }
        throw std::runtime_error("Edge not found");
    }


    list<RoadDetails*> getEdges(t data, list<RoadDetails*> edges) {
        
        int index = getIndex(data);
        if (index == -1) { return edges; }
        for (int i = 0; i < Vcount; i++) {
            typename list<weighted> ::iterator temp = array[i].Neighbors.begin();
            while (temp!=array[i].Neighbors.end()) {
                if (temp->index == index) {
                    edges.push_back(&(temp->weight));
                }
                temp++;
            }
            
        }
        return edges;
    }

    float AverageRush() {
        float sum = 0;
        int count = 0;
        
        for (int i = 0; i < Vcount; i++) {
            typename list<weighted> ::iterator temp = array[i].Neighbors.begin();
            while (temp != array[i].Neighbors.end()) {
                
               sum+= temp->weight.Congestion();
               count++;
               temp++;
            }

        }
        return sum / count;
    }

    
    t getVertexAt(int i) { return array[i].vertex; }



    void printGrapgh() {
        for (int i = 0; i < Vcount; i++) {
            display_edge_of_index(array[i].vertex);
            cout << endl;
        }
    }

    
};



