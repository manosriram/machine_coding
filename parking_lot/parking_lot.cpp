#include <chrono>
#include <iostream>
#include <map>
#include <math.h>
#include <unistd.h>
using namespace std;

enum parking_spot_type { large, compact, handicapped };
enum vehicle_type { car, bike, van, truck };
static int ticket_counter, spot_counter;
class Vehicle {
public:
  string license_no;
  vehicle_type type;
};

class Car : public Vehicle {
public:
  Car(string car_license_no) {
    license_no = car_license_no;
    type = car;
  }
};
class Bike : public Vehicle {
public:
  Bike(string bike_license_no) {
    license_no = bike_license_no;
    type = bike;
  }
};
class Van : public Vehicle {
public:
  Van(string van_license_no) {
    license_no = van_license_no;
    type = van;
  }
};
class Truck : public Vehicle {
public:
  Truck(string truck_license_no) {
    license_no = truck_license_no;
    type = truck;
  }
};

class ParkingSpot {
public:
  int id;
  bool is_available;
  parking_spot_type spot_type;

  Vehicle *vehicle_at_spot;

  ParkingSpot() {
    id = spot_counter++;
    is_available = true;
  }

  void unassign_parking_spot() {
    is_available = true;
    vehicle_at_spot = nullptr;
  }

  // ~ParkingSpot() { delete vehicle_at_spot; }
};

class HandicappedParkingSpot : public ParkingSpot {
public:
  HandicappedParkingSpot() { spot_type = handicapped; }
};

class CompactParkingSpot : public ParkingSpot {
public:
  CompactParkingSpot() { spot_type = compact; }
};

class LargeParkingSpot : public ParkingSpot {
public:
  LargeParkingSpot() { spot_type = large; }
};

class ParkingTicket {
public:
  int id;
  int fee;
  bool is_valid;
  std::chrono::system_clock::time_point entry, exit;

  ParkingSpot *assigned_spot;

  ParkingTicket(ParkingSpot *spot) {
    id = ticket_counter++;
    assigned_spot = spot;
    assigned_spot->is_available = false;
    entry = std::chrono::system_clock::now();
    is_valid = true;
  }

  void printReceipt() {
    exit = std::chrono::system_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(exit - entry)
            .count();
    int hours = (float)((duration / (1000 * 60 * 60)) % 24);

    cout << "id: " << id << endl;
    // cout << "entry at: " << entry.max() << endl;
    // cout << "exit at: " << exit.max() << endl;
    cout << "duration: " << hours << endl;
    cout << "fee: " << 100 * ceil(hours) << endl;
  }
};

class ParkingFloor {
public:
  map<int, ParkingSpot *> floor_parking_spots;
  map<string, ParkingTicket *> floor_parking_tickets;
  int total_no_of_spots;

  ParkingFloor(int no_of_compact_spots, int no_of_handicapped_spots,
               int no_of_large_spots) {

    total_no_of_spots =
        no_of_compact_spots + no_of_handicapped_spots + no_of_large_spots;

    int total = 1, t;
    for (t = 1; t <= no_of_compact_spots; ++t) {
      floor_parking_spots[total++] = new CompactParkingSpot();
    }

    for (t = 1; t <= no_of_handicapped_spots; ++t) {
      floor_parking_spots[total++] = new HandicappedParkingSpot();
    }

    for (t = 1; t <= no_of_large_spots; ++t) {
      floor_parking_spots[total++] = new LargeParkingSpot();
    }
  }

  ParkingSpot *getAvailableSpot(parking_spot_type spot_type) {
    for (int t = 1; t <= total_no_of_spots; ++t) {
      if (floor_parking_spots[t]->is_available &&
          floor_parking_spots[t]->spot_type == spot_type)
        return floor_parking_spots[t];
    }
    return nullptr;
  }

  ParkingTicket *assignTicketToVehicle(Vehicle *v) {
    if (floor_parking_tickets[v->license_no]) {
      cout << "vehicle already present in the parking lot" << endl;
      return nullptr;
    }
    parking_spot_type p;

    if (v->type == car || v->type == van || v->type == truck)
      p = large;
    else if (v->type == bike)
      p = compact;
    else
      p = handicapped;

    cout << "ty = " << p << endl;
    ParkingSpot *parking_spot = getAvailableSpot(p);

    ParkingTicket *ticket = new ParkingTicket(parking_spot);

    floor_parking_spots[parking_spot->id] = parking_spot;
    floor_parking_tickets[v->license_no] = ticket;

    return ticket;
  }

  void exitVehicle(ParkingTicket *ticket) {
    floor_parking_spots[ticket->assigned_spot->id]->unassign_parking_spot();
  }
};

class DisplayBoard : public ParkingFloor {
public:
  DisplayBoard() : ParkingFloor(10, 10, 10) {}

  void displayFloorStats(ParkingFloor *floor) {
    int free_compact = 10, free_large = 10, free_handicapped = 10;
    floor_parking_spots = floor->floor_parking_spots;

    for (auto x : floor_parking_spots) {
      if (!x.second->is_available) {
        if (x.second->spot_type == large)
          free_large--;
        else if (x.second->spot_type == compact)
          free_compact--;
        else if (x.second->spot_type == handicapped)
          free_handicapped--;
      }
    }

    cout << "Free Compact Spots: " << free_compact << endl
         << "Free Handicapped Spots: " << free_handicapped << endl
         << "Free Large Spots: " << free_large << endl;
  }
};

class ParkingLot {
public:
  map<int, ParkingFloor *> parking_floors;

  ParkingLot(int floors) {
    for (int t = 1; t <= floors; ++t) {
      parking_floors[t] = new ParkingFloor(10, 10, 10);
    }
  }
};

class Exit : public ParkingFloor {
public:
  Exit() : ParkingFloor(10, 10, 10) {}

  void validateTicket(ParkingFloor *floor, Vehicle *v) {
    // floor->floor_parking_spots[
    ParkingTicket *ticket = floor->floor_parking_tickets[v->license_no];

    int no_of_hours = 1;
    ticket->fee = 100 * no_of_hours;

    ticket->printReceipt();
  }
};

class ParkingLotFactory {
public:
  ParkingLotFactory(int floors) {
    ParkingLot *lot = new ParkingLot(floors);
    DisplayBoard *d = new DisplayBoard();

    auto floor_one = lot->parking_floors[1];

    cout << "car ap31abcd entry" << endl;
    Vehicle *v1 = new Car("ap31abcd");
    auto t1 = floor_one->assignTicketToVehicle(v1);

    cout << "car ap31xyzz entry" << endl;
    Vehicle *v2 = new Car("ap31xyzz");
    auto t2 = floor_one->assignTicketToVehicle(v2);

    d->displayFloorStats(floor_one);
    // return;
    cout << "car ap31xyzz exit" << endl;
    floor_one->exitVehicle(t1);

    d->displayFloorStats(floor_one);

    cout << "bike ap33abcb entry" << endl;
    Vehicle *v3 = new Bike("ap31abcb");
    auto t3 = floor_one->assignTicketToVehicle(v3);

    cout << "car ap31abcd exit" << endl;
    floor_one->exitVehicle(t2);

    d->displayFloorStats(floor_one);

    cout << "bike ap31abcb exit" << endl;
    floor_one->exitVehicle(t3);
  }
};

int main() { ParkingLotFactory *f = new ParkingLotFactory(3); }
