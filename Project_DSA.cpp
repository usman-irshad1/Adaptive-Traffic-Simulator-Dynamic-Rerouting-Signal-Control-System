// Project_DSA.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<list>
#include<string>
#include"Header1.h"
#include<utility>
#include<fstream>
using namespace std;



template <class t>
struct vehicle {
    int id;
    t dest;
    t source;
    t current;
    list<t> path;
	pair<t, t> currentRoad; 
    int state;
    float timespent, timeRemaining;
    vehicle() {
        id = 0;
        dest = t();
        source = t();
        current = t();
        state = -1;
        timespent = timeRemaining = 0;
		currentRoad.first = currentRoad.second = t();
        //if 0 waiting 1 running 2 arrived;
    }
    vehicle(int i, t s, t d) {
        id = i;
        dest = d;
        source = s;
        current = s;
        state = 0;
        timespent = timeRemaining = 0;
        currentRoad.first = currentRoad.second = t();
        //if 0 waiting 1 running 2 arrived;
    }
};
template <class t,int size>
class Manager {
    float totalArrivedCount ;
    float totalTime;
    list<vehicle<t>> array_of_vehicles;
    Graph<t, size>* map; //making it a pointerr so any change in map is refrencted here as well
public:
    Manager(Graph<t, size>* m) : map(m) {
        totalArrivedCount = 0; 
        totalTime= 0;
    }

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
                car->currentRoad.first = u;
                car->currentRoad.second = v;
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
                    car->currentRoad.first = u;
                    car->currentRoad.second = v;
                }
            }
            car++;

        }
    }

    void reached() {
        typename list<vehicle<t>> ::iterator car = array_of_vehicles.begin();
        while (car != array_of_vehicles.end()) {
            bool deleted = false;
            if (car->state == 2) {
                this->totalArrivedCount++;
                this->totalTime += car->timespent;
                car = array_of_vehicles.erase(car);
                deleted = true;
                 
                continue;
            }
            if (car->state == 1) {

                if (car->path.size() == 2) {

                    if (car->timeRemaining <= 0) {
                        this->totalArrivedCount++;
                        this->totalTime += car->timespent;
                        car->state = 2;
                        t u = car->path.front();
                        auto it_p = car->path.begin();
                        it_p++;
                        t v = *it_p;
                        RoadDetails& r = map->getEdgeDetails(u, v);
                        r.vehicleExitsRoad();
                        car->currentRoad.first = t();
                        car->currentRoad.second = t();
                        car->current = v;
                        car->timeRemaining = 0;
                        Car_timing_file(car->timespent, car->id);
                        car = array_of_vehicles.erase(car);
                        deleted = true;
                        
                    }

                }
            }
            if (!deleted) {
                car++;
            }
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
                it2++;
                t next_v = *it2;
                RoadDetails& road = map->getEdgeDetails(next_u, next_v);
                float discharge = road.DischargeAllowed(road.capacity, road.currentVehicles);
                if (!road.signalState) {
                    road.vehicleJoinsQueue();
                    car->current = v;
                    car->state = 0;
                    car->currentRoad.first = t();
                    car->currentRoad.second = t();
                }

                else {
                    if (road.currentVehicles < road.capacity && mov[index] < discharge) {
                        mov[index]++;
                        road.vehicleEntersRoad();
                        car->timeRemaining = road.NonIdealtime();
                        car->state = 1;
                        car->currentRoad.first = next_u;   
                        car->currentRoad.second = next_v;
                    }
                    else {
                        road.vehicleJoinsQueue();
                        car->current = v;
                        car->state = 0;
                        car->currentRoad.first = t();
                        car->currentRoad.second = t();

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
            int maxWaitingQueue = 0;
            typename  list<RoadDetails*>::iterator temp = edges.begin();
            while (temp != edges.end()) {
                
                (*temp)->light.Timer((*temp)->signalState, 0.10f);

                if ((*temp)->signalState == true) {
                    currentGreen = *temp;
                }

                float totalCost = (*temp)->choosing() + (*temp)->light.starvationCost();

                if (totalCost > max) {
                    max = totalCost;
                    worstCandidate = *temp;
                }
                if ((*temp)->queueCount > maxWaitingQueue) {
                    maxWaitingQueue = (*temp)->queueCount;
                }
                temp++; 
            }

            if (worstCandidate != nullptr) {
                if (currentGreen == nullptr) {
                    worstCandidate->changeState(); // Turn green
                }
                else if (currentGreen != worstCandidate) {
                   
                    if (currentGreen->currentVehicles == 0 || currentGreen->light.canSwitch(currentGreen->queueCount, maxWaitingQueue)) {
                        currentGreen->change_to_red();
                        worstCandidate->changeState();
                    }
                }
            
            }
        }
    }
    bool printPerformanceMetrics(float tickTime, float averageRush) {
        
        ofstream outfile("performance_metrics.txt", ios::app);
        if (outfile.is_open()) {
            outfile << "\n--- PERFORMANCE METRICS REPORT ---" << endl;

            if (this->totalArrivedCount > 0) {
                // ACTUAL Average = (Sum of all car times) / (Total number of cars)
                float actualAvgTravelTime = (float)this->totalTime / this->totalArrivedCount;

                // System Throughput = (Cars Arrived) / (Simulation Ticks Elapsed)
                float throughput = (float)this->totalArrivedCount / tickTime;

                outfile << "Total Vehicles Arrived: " << totalArrivedCount << endl;
                outfile << "Average Travel Time: " << actualAvgTravelTime << " ticks" << endl;
                outfile << "System Throughput: " << throughput << " vehicles/tick" << endl;
            }
            else {
                outfile << "No vehicles have finished yet." << endl;
            }

            outfile << "Total System Cost: " << map->total_System_Cost() << endl;
            outfile << "Average Rush Level: " << averageRush << endl;
            outfile << "----------------------------------" << endl;

            return (averageRush == 0);
            
        }
        else {
            cerr << "Unable to open performance_metrics.txt for writing." << endl;
            return false;
        }
    }

    void Car_timing_file(float time, int id) {
    
		ofstream outfile("car_timings.txt", ios::app);
        if (outfile.is_open()) {
            outfile << "Vehicle " << id << " arrived at destination in " << time << " ticks." << endl;
        }
        else {
            cerr << "Unable to open car_timings.txt for writing." << endl;
		}
    
    }
    void clearMetricsFile() {
        ofstream outfile("performance_metrics.txt", ios::trunc); // trunc deletes everything
        if (outfile.is_open()) {
            outfile << "--- NEW SIMULATION SESSION ---" << endl;
            outfile.close();
        }
        ofstream outfile1("car_timings.txt", ios::trunc); // trunc deletes everything
        if (outfile1.is_open()) {
            outfile1 << "--- NEW SIMULATION SESSION ---" << endl;
            outfile1.close();
        }
        ofstream outfile2("map.txt", ios::trunc); // trunc deletes everything
        if (outfile2.is_open()) {
            outfile2 << "--- NEW SIMULATION SESSION ---" << endl;
            outfile2.close();
        }

    }

    void printNetwork() {
        ofstream outfile("map.txt", ios::app);
            if (outfile.is_open()){ outfile<<map->printGrapgh(); }
        
    }

    void PrintAllConnetedCityFromAcity() {
        map->PrintLevels();
    }

    void checkBrokenPath() {
        typename list<vehicle<t>>::iterator car = array_of_vehicles.begin();
        while (car != array_of_vehicles.end()) {
            if (car->path.empty()) {
                
            cout << "Removing Vehicle " << car->id << ": No viable path to " << car->dest << endl;
            if (car->state ==1 ) {
                RoadDetails& d = map->getEdgeDetails(car->currentRoad.first, car->currentRoad.second);
                d.vehicleExitsRoad();
            }
            car = array_of_vehicles.erase(car);
            }
            else {
                car++;
            }
        }
    }

    void physics() {
        float x = 1;
        int total_time = 0;
        cout << "time step = 6 minutes ticks per loop" << endl;

        while (array_of_vehicles.size() > 0) {
            total_time++;
            updateSignals();

  
            for (auto& car : array_of_vehicles) {
                // If even one car hasn't reached State 2, we keep running

                if (car.state == 1) {
                    car.timeRemaining -= .10f;
                    car.timespent += .10f;
                }
                else if (car.state == 0) {

                    car.timespent += .10f;
                   
                }

                if (car.timeRemaining < 0) {
                    car.timeRemaining = 0;
                }

            }

            if (total_time % 10 == 0) {
                printPerformanceMetrics(total_time, x);
            }

            // Core Physics Functions
            reached();
            arrivalAtIntersection();
            entraingfromQueetoEdge();
            entrance();
            checkBrokenPath();

            x = map->AverageRush();


  
            if (total_time > 20000) {
                cout << "Simulation timed out after 20,000 ticks." << endl;
                break;
            }
        }

        cout << "\n--- FINAL SIMULATION REPORT ---" << endl;
        printPerformanceMetrics(total_time, x);
        cout << "All reachable cars have reached their destinations." << endl;
		cout << this->array_of_vehicles.size() << " left in the system (unreachable or still en route)." << endl;
    }
};


template <class t, int size>
class Simulator {
    Graph<t, size>* pakistanMap;
    

public:
    Manager<t, size>* cityManager;
    Simulator() {

        pakistanMap = new Graph<t, size>(true); // Directed
        cityManager = new Manager<t, size>(pakistanMap);
    }

    void setupNetwork() {
        pakistanMap->insertVertex("Karachi");
        pakistanMap->insertVertex("Sukkur");
        pakistanMap->insertVertex("Quetta");
        pakistanMap->insertVertex("DG Khan");
        pakistanMap->insertVertex("Multan");
        pakistanMap->insertVertex("Lahore");
        pakistanMap->insertVertex("Islamabad");

        int KHI = pakistanMap->getIndex("Karachi");
        int SUK = pakistanMap->getIndex("Sukkur");
        int QUE = pakistanMap->getIndex("Quetta");
        int DGK = pakistanMap->getIndex("DG Khan");
        int MUL = pakistanMap->getIndex("Multan");
        int LHR = pakistanMap->getIndex("Lahore");
        int ISB = pakistanMap->getIndex("Islamabad");

        pakistanMap->makeEdge(KHI, SUK, 450, 120, 60, 0.15, 5.0);
        pakistanMap->makeEdge(MUL, LHR, 330, 120, 50, 0.15, 5.0);
        pakistanMap->makeEdge(LHR, ISB, 370, 120, 50, 0.15, 5.0);

      
        pakistanMap->makeEdge(SUK, MUL, 390, 80, 25, 0.7, 4.0);
        pakistanMap->makeEdge(DGK, MUL, 100, 60, 20, 0.8, 4.0); 
        pakistanMap->makeEdge(MUL, DGK, 100, 60, 20, 0.8, 4.0);

        pakistanMap->makeEdge(ISB, LHR, 370, 120, 50, 0.15, 5.0);
        pakistanMap->makeEdge(LHR, MUL, 330, 110, 50, 0.15, 5.0);
        pakistanMap->makeEdge(MUL, SUK, 390, 100, 40, 0.15, 5.0);
        pakistanMap->makeEdge(SUK, KHI, 450, 100, 60, 0.15, 5.0);

        pakistanMap->makeEdge(KHI, QUE, 680, 80, 25, 0.4, 3.0);
        pakistanMap->makeEdge(QUE, KHI, 680, 80, 25, 0.4, 3.0);

        pakistanMap->makeEdge(QUE, DGK, 350, 70, 20, 0.5, 4.0);
        pakistanMap->makeEdge(DGK, QUE, 350, 70, 20, 0.5, 4.0);

        pakistanMap->makeEdge(DGK, ISB, 500, 90, 20, 0.5, 5.0);
        pakistanMap->makeEdge(ISB, DGK, 500, 90, 20, 0.5, 5.0);

        // Optional: Sukkur to Quetta link
        pakistanMap->makeEdge(SUK, QUE, 400, 85, 15, 0.5, 4.0);
        pakistanMap->makeEdge(QUE, SUK, 400, 85, 15, 0.5, 4.0);
    }

    void addMassiveTraffic() {
        string cities[] = {"Karachi", "Sukkur", "Quetta", "DG Khan", "Multan", "Lahore", "Islamabad"};
        
        
        for (int i = 1; i <= 1000; i++) {
            string start = cities[i % 4]; 
            string end = cities[4 + (i % 3)]; 
            
            if (start != end) {
                cityManager->addVehicle(i, start, end);
            }
        }
        cityManager->printNetwork();
    }

    void run() {
        cout << "--- STARTING HEAVY TRAFFIC SIMULATION (1000 VEHICLES) ---\n";
        cityManager->physics();
    }

    ~Simulator() {
        delete cityManager;
        delete pakistanMap;
    }
    void clear() {
        cityManager->clearMetricsFile();

    }
};


int main() {
    
    Simulator<string, 100> gods_eye;
    gods_eye.clear();
	gods_eye.setupNetwork();
    gods_eye.addMassiveTraffic();
    gods_eye.run();

}