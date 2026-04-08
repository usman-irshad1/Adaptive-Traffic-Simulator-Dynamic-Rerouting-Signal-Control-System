// Project_DSA.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<list>
#include<string>
#include"Header1.h"
using namespace std;



template <class t>
struct vehicle {
    int id;
    t dest;
    t source;
    t current;
    list<t> path;
    int state;
    float timespent, timeRemaining;
    vehicle() {
        id = 0;
        dest = t();
        source = t();
        current = t();
        state = -1;
        timespent = timeRemaining = 0;
        //if 0 waiting 1 running 2 arrived;
    }
    vehicle(int i, t s, t d) {
        id = i;
        dest = d;
        source = s;
        current = s;
        state = 0;
        timespent = timeRemaining = 0;
        //if 0 waiting 1 running 2 arrived;
    }
};
template <class t,int size>
class Manager {
    list<vehicle<t>> array_of_vehicles;
    Graph<t, size>* map; //making it a pointerr so any change in map is refrencted here as well
public:
    Manager(Graph<t, size>* m) : map(m) {}

    void entrance() {
        int mov[size];
        for (int i = 0; i < size; i++) {
            mov[i] = 0;
        }
        typename list<vehicle<t>> ::iterator car = array_of_vehicles.begin();
        while (car != array_of_vehicles.end()) {
            if (car->state != 0) {
                car++;
                continue;
            }

            if (car->path.size() < 2) {
                car->state = 2; // Nowhere to go
                car++;
                continue;
            }
            t u = car->path.front();
            auto it_p = car->path.begin();
            advance(it_p, 1);
            t v = *it_p;
            int index = map->getIndex(u);
            RoadDetails& road = map->getEdgeDetails(u, v);
            float discharge=road.DischargeAllowed(road.capacity, road.currentVehicles);
            if (road.signalState && mov[index]<discharge) {
                mov[index]++;
                road.vehicleEntersRoad();
                car->timeRemaining = road.NonIdealtime();
                car->current = u;
                car->state = 1;
            }
            else {
                road.vehicleJoinsQueue();
                car->state = 0;
            }
            car++;
        }
    }


    void entraingfromQueetoEdge() {

        typename list<vehicle<t>> ::iterator car = array_of_vehicles.begin();
        int mov[size];
        for (int i = 0; i < size; i++) {
            mov[i] = 0;
        }
        while (car != array_of_vehicles.end()) {
            if (car->path.size() < 2) {
                car->state = 2;
                car++;
                continue;
            }
            if (car->state == 0) {
                t u = car->path.front();
                auto it_p = car->path.begin();
                advance(it_p, 1);
                t v = *it_p;
                int index = map->getIndex(u);
                RoadDetails& road = map->getEdgeDetails(u, v);
                float discharge = road.DischargeAllowed(road.capacity, road.currentVehicles);
                if (road.signalState && mov[index] < discharge) {
                    mov[index]++;
                    road.vehicleExitsQueue();
                    road.vehicleEntersRoad();
                    car->timeRemaining = road.NonIdealtime();
                    car->current = u;
                    car->state = 1;
                }
            }
            car++;

        }
    }

    void reached() {
        typename list<vehicle<t>> ::iterator car = array_of_vehicles.begin();
        while (car != array_of_vehicles.end()) {
            if (car->state == 2) { car++; continue; }
            if (car->state == 1) {

                if (car->path.size() == 2) {

                    if (car->timeRemaining <= 0) {
                        car->state = 2;
                        t u = car->path.front();
                        auto it_p = car->path.begin();
                        advance(it_p, 1);
                        t v = *it_p;
                        RoadDetails& r = map->getEdgeDetails(u, v);
                        r.vehicleExitsRoad();
                        car->current = v;
                        car->timeRemaining = 0;
                        car->timespent += car->timeRemaining;
                        cout << ">> Vehicle " << car->id << " has ARRIVED at " << v << endl;
                    }

                }
            }
            car++;
        }

    }

    void ShortestPath() {
        typename list <vehicle<t>> ::iterator temp = array_of_vehicles.begin();
        while (temp != array_of_vehicles.end()) {
            if (temp->state == -1) {
                temp++;
            }
            else if (temp->state == 2) {
                temp++;
            }
            else {
                temp->path = map->shortest_Path_btw2_vericex_returing_list(temp->current, temp->dest);
                temp++;
            }
        }
    }
    void addVehicle(int id, t source, t destination) {
        vehicle<t> newCar(id, source, destination);
        newCar.path = map->shortest_Path_btw2_vericex_returing_list(source, destination);
            array_of_vehicles.push_back(newCar);
      
    }

    void arrivalAtIntersection() {
        ShortestPath();
        typename list <vehicle<t>> ::iterator car = array_of_vehicles.begin();
        int mov[size];
        for (int i = 0; i < size; i++) {
            mov[i] = 0;
        }

        while (car != array_of_vehicles.end()) {
            if (car->state == 1 && car->timeRemaining <= 0 && car->path.size() > 2) {
                t u = car->path.front();
                int index = map->getIndex(u);
                auto it = car->path.begin();
                advance(it, 1);
                t v = *it;
                RoadDetails& oldRoad = map->getEdgeDetails(u, v);
                oldRoad.vehicleExitsRoad();
                car->path.pop_front();
                car->current = v;

                t next_u = car->path.front();
                auto it2 = car->path.begin();
                advance(it2, 1);
                t next_v = *it2;
                RoadDetails& road = map->getEdgeDetails(next_u, next_v);
                float discharge = road.DischargeAllowed(road.capacity, road.currentVehicles);
                if (!road.signalState) {
                    road.vehicleJoinsQueue();
                    car->current = v;
                    car->state = 0;
                }

                else {
                    if (road.currentVehicles < road.capacity && mov[index] < discharge) {
                        mov[index]++;
                        road.vehicleEntersRoad();
                        car->timeRemaining = road.NonIdealtime();
                        car->state = 1;
                    }
                    else {
                        road.vehicleJoinsQueue();
                        car->current = v;
                        car->state = 0;

                    }
                }
               
            }
            car++;
        }
    }



    void updateSignals() {

        for (int i = 0; i < size; i++) {

            list<RoadDetails*> edges;

            edges = map->getEdges(map->getVertexAt(i), edges);
            if (edges.empty()) {
                continue;
            }
            RoadDetails* currentGreen = nullptr;
            RoadDetails* worstCandidate = nullptr;
            float max = -1.0f;
            typename  list<RoadDetails*>::iterator temp = edges.begin();
            while (temp != edges.end()) {

                (*temp)->increment(1);

                if ((*temp)->signalState == true) {
                    currentGreen = *temp;
                }
                if ((*temp)->choosing() > max) {
                    max = (*temp)->choosing();
                    worstCandidate = *temp;
                }
                temp++;
            }

            if (worstCandidate != nullptr) {
                if (currentGreen == nullptr) {
                    worstCandidate->changeState(); // Turn green
                }
                else if (currentGreen != worstCandidate) {
                   
                    if (currentGreen->currentVehicles == 0 || currentGreen->switching()) {
                        currentGreen->change_to_red();
                        worstCandidate->changeState();
                    }
                }
            
            }
        }
    }
    bool printPerformanceMetrics(int time, float average) {
        float totalActualTime = 0;
        float totalIdealTime = 0;
        float arrivedCount = 0;

        for (const auto& car : array_of_vehicles) {
            if (car.state == 2) {
                totalActualTime += car.timespent;
                arrivedCount++;
            }
        }
        cout << "\n--- PERFORMANCE METRICS REPORT ---" << endl;
        if (arrivedCount > 0) {
            cout << "Average Travel Time: " << (totalActualTime / arrivedCount) << " units" << endl;
        }


        cout << "Throughput: " << arrivedCount / time << " vehicles total" << endl;
        cout << "Congestion " << average << endl;
        if (average == 0) { return true; }
        else { return false; }
        cout << "----------------------------------" << endl;
    }

    void physics() {
        float x=1;
        int total_time=0;
        cout << "time step = 6 minutes ticks per loop";
        for (int i = 0; i < 500; i++) {
            total_time++;
            updateSignals();

            
            for (auto& car : array_of_vehicles) {
                if (car.state == 1) {
                    car.timeRemaining -= .10f;
                    car.timespent += .10f;
                }
                else if (car.state == 0) {
                    car.timespent += .10f; 
                }
            }
            if (i % 10==0) {
             bool check= printPerformanceMetrics(total_time, x);
            
            if (check) {
                cout << "All cars have reached destination\n";
                break;
            }
            }
            if (i % 50==0) {
                if (x == 0) {
                    break;
                }
                printNetwork();
                
            }

            
            reached();                  
            arrivalAtIntersection();
            entraingfromQueetoEdge();   
            entrance(); 
            checkBrokenPath();
             x= map->AverageRush();

            if (i %50==0) {
                
                cout << "\n--- DEBUG PROBE ---" << endl;
                for (auto& car : array_of_vehicles) {
                    cout << "Car " << car.id << " | State: " << car.state
                        << " | Current Node: " << car.current
                        << " | Time Rem: " << car.timeRemaining << endl;
                   
                }
            }
            
        }
        printPerformanceMetrics(total_time, x);
        
    }

    void printNetwork() {

        map->printGrapgh();
    }

    void PrintAllConnetedCityFromAcity() {
        map->PrintLevels();
    }

    void checkBrokenPath() {
        typename list<vehicle<t>>::iterator car = array_of_vehicles.begin();
        while (car != array_of_vehicles.end()) {
            if (car->path.empty()) {
            cout << "Removing Vehicle " << car->id << ": No viable path to " << car->dest << endl;
            if (car->state == 1 && car->path.size() >= 2) {
                typename list<t>::iterator temp=car->path.begin();
                t i= *temp;
                temp++;
                t j = *temp;
                RoadDetails& d = map->getEdgeDetails(i, j);
                d.vehicleExitsRoad();
            }
            car = array_of_vehicles.erase(car);
            }
            else {
                car++;
            }
        }
    }
};


int main() {
    Graph<string, 100> pakistanMap(true);

    pakistanMap.insertVertex("Karachi");    // Hub South
    pakistanMap.insertVertex("Sukkur");     // Junction
    pakistanMap.insertVertex("Quetta");     // West Route
    pakistanMap.insertVertex("DG Khan");    // Central Link
    pakistanMap.insertVertex("Multan");     // South-Central Hub
    pakistanMap.insertVertex("Lahore");     // East Hub
    pakistanMap.insertVertex("Islamabad");  // Hub North

    int KHI = pakistanMap.getIndex("Karachi");
    int SUK = pakistanMap.getIndex("Sukkur");
    int QUE = pakistanMap.getIndex("Quetta");
    int DGK = pakistanMap.getIndex("DG Khan");
    int MUL = pakistanMap.getIndex("Multan");
    int LHR = pakistanMap.getIndex("Lahore");
    int ISB = pakistanMap.getIndex("Islamabad");
    pakistanMap.makeEdge(KHI, SUK, 450, 100, 10);
    pakistanMap.makeEdge(SUK, MUL, 390, 120, 8);
    pakistanMap.makeEdge(MUL, LHR, 330, 110, 8);
    pakistanMap.makeEdge(LHR, ISB, 370, 120, 10);
    pakistanMap.makeEdge(KHI, QUE, 680, 80, 5);
    pakistanMap.makeEdge(QUE, DGK, 350, 70, 4);
    pakistanMap.makeEdge(DGK, ISB, 500, 90, 6);
    pakistanMap.makeEdge(SUK, QUE, 400, 85, 4);  // Sukkur to Quetta link
    pakistanMap.makeEdge(DGK, MUL, 100, 90, 5);  // DGK to Multan link
    pakistanMap.makeEdge(MUL, DGK, 100, 90, 5);  // Multan to DGK link
    pakistanMap.makeEdge(LHR, MUL, 330, 110, 8); // Southbound East
    pakistanMap.makeEdge(ISB, DGK, 500, 90, 6);  // Southbound West

    Manager<string, 100> cityManager(&pakistanMap);

    cityManager.addVehicle(1, "Karachi", "Islamabad"); // Has 2 main paths
    cityManager.addVehicle(2, "Karachi", "Islamabad");
    cityManager.addVehicle(3, "Quetta", "Lahore");    // Must cross from Left to Right
    cityManager.addVehicle(4, "Multan", "Quetta");    // Must cross from Right to Left
    cityManager.addVehicle(5, "Karachi", "Lahore");

    cout << "--------------------------------------------------\n";
    cityManager.PrintAllConnetedCityFromAcity();
    cityManager.physics();

    return 0;
}