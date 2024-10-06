

// -------------- Mehmet Egehan Pala 31089 28.10.2023 ---------------------





#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>


using namespace std;

struct flight{
  string from;
  string to;
  int hour;
  int min;
  int price;
  int ID;
  
  flight *next;
  flight *prev;
  
    
    
    flight()
    {}
    flight(string f, string t, int h, int m, int p, int i, flight *nxt, flight *pre)//constructor for the new command
        :from(f), to(t), hour(h), min(m), price(p), ID(i), next(nxt), prev(pre)
    {}
    
    flight(string f, string t, int h, int m, int p)//constructor for the makepair()
        :from(f), to(t), hour(h), min(m), price(p)
    {}
    
    
    bool operator() (flight i,flight j) { // bool operator returns the false true values for the sort function in the below
        if (i.hour == j.hour){
            return (i.min < j.min);}
        else{
            return (i.hour < j.hour);}}
}flight_hours;



struct airline{
  string name;
  int ID;
  airline *next;
  flight *flights;

  
    airline()
    {}
    airline(string n, int i, airline *nxt, flight *fli)//constructor for the new nodes
        :name(n), ID(i), next(nxt), flights(fli)
    {}
    
    airline(string n)//constructor for the makepair()
        :name(n)
    {}
     
  //Hint: You may need to implement multiple constructors
};

struct path {
    vector<flight> flights;
    int totalPrice;

    path() : totalPrice(0) {}// initializing the total price to 0
};

pair<vector<string>, vector<vector<flight>>> read_files(bool input_done){
  
  ///READ FLIGHTS///
  vector<string> airlines;
  vector<vector<flight>> flights(100); //start with a previously sized vector or you get seg fault (for sure)
  
  while(not input_done){
    string airline_name;
    string filename;
    cout << "Enter the name of airline: ";
    cin >> airline_name;
    cout << "Enter the name of airline file: ";
    cin >> filename;
    ifstream input(filename);

    if(filename == "done"){
      input_done = true;
      return make_pair(airlines, flights);
    }
    
    while( input.fail() ){
      cout << "Could not open file: " << filename << endl;
      cout << "Enter the name of airline: ";
      cin >> airline_name;
      cout << "Enter the name of airline file: ";
      cin >> filename;
      input.open(filename);

      if(filename == "done"){
      input_done = true;
      return make_pair(airlines, flights);
      }
    }
    
    airlines.push_back(airline_name);
    
   
    cout << "Processing input file: " << filename <<endl;
    
    int line_no = 0;
    vector<string> lines;
    string line;
    int vector_location = airlines.size() - 1;
    while(getline(input, line)){
      lines.push_back(line);
      line_no += 1;
      if(line_no % 5 == 0){
	flight a_flight(lines[line_no-5], lines[line_no-4], stoi(lines[line_no-3]), stoi(lines[line_no-2]), stoi(lines[line_no-1]));
	flights[vector_location].push_back(a_flight);
      }
    }
  }
  if(input_done){
    cout << "Input files are already read, however you can do manual changes.." << endl;
  }

  return make_pair(airlines, flights);
}


int glob_IDs = 0;

airline* make_linked_list_structure(vector<string> airlines, vector<vector<flight>> flights){
    
    size_t number_of_airlines = airlines.size();
    int start_IDs = 0;
    
    //this part of the program is for to store the IDs
    for(int i = 0; i < number_of_airlines; i++)
    {
        if (i >= 1)
        {
            start_IDs = start_IDs + flights[i - 1].size();//calculating the size of the previous airline vector and adding it up to start_IDs
        }
        
        for (int j = 0; j < flights[i].size(); j++)
        {
            flights[i][j].ID = start_IDs + j;//calculates the size of the previous airline vector and sums up with the j to calculate the ID number
            glob_IDs++;
        }
        
    }
    
    for (int i = 0; i < number_of_airlines; i++)
    {
        //sorting the vector of struckts by hours and minutes with the help of the decleration in the struckt flight
        sort(flights[i].begin(), flights[i].end(), flight_hours);
    }
    
    // creating a head for the linkedlist
    airline* head = new airline(airlines[0], 0, nullptr, nullptr);
    airline* currentHeadPtr = head;
    
    for (int i = 0; i < number_of_airlines; i++)
    {
        
        airline* temp_airline = nullptr;// creating a temporarily pointer to use on creating a linked list
        
        if (i == 0) // if i == 0 then it means we are in the first airline so making the temp, head
        {
            temp_airline = head;
        }
        else
        {
            temp_airline = new airline(airlines[i], i, nullptr, nullptr); // if i != 0 then we are in the next airlines (can create memory in heap)
            currentHeadPtr -> next = temp_airline; //connecting head next to temp_airline
            currentHeadPtr = temp_airline;// initializing the currentHead pointer to temporarily variable so that can use it in the next addition
        }
        
        
        flight* temp = nullptr;
        for (int j = 0; j < flights[i].size(); j++)
        {
            
            //creating a new flights to add to the airlines
            flight* ptr = new flight(flights[i][j].from, flights[i][j].to, flights[i][j].hour, flights[i][j].min, flights[i][j].price, flights[i][j].ID, nullptr, nullptr);
            
            if (currentHeadPtr -> flights == nullptr){ // if it is the firs flight we directly add it to the airline
                currentHeadPtr -> flights = ptr;
                ptr -> prev = nullptr;
            }
            
            /*since it is the second flight to be added we could keep the first flight by accessing from the currentHeadPtr (to point towards the
             previous one) */
            else if (j == 1 )
            {
                temp = currentHeadPtr -> flights;
                ptr -> prev = temp;
                temp -> next = ptr;
                temp = ptr;
            }
            else
            {//this part is the regular linked list addititon
                temp -> next = ptr;
                ptr -> prev = temp;
                temp = ptr;
            }
        }
    }
    return head;
}


void pathFinderRecursion(airline* head, const string& new_from, const string& new_to, int maxTransfers, const path& current_Path, vector<path>& foundPaths)
{
    if (maxTransfers == -1) {
        return; // Maximum transfers reached should stop recursion (base case)
    }

    airline* currentHeadPtr = head;
    while (currentHeadPtr != nullptr)
    {
        flight* currentFlight = currentHeadPtr->flights;
        while (currentFlight != nullptr)
        {
            if (currentFlight->from == new_from)
            {
                //storing the transferPrice and the flights in a vector
                path new_path = current_Path;
                new_path.flights.push_back(*currentFlight);
                new_path.totalPrice += currentFlight->price;
                
                if (currentFlight->to == new_to)
                {
                    // if the from and to matches add it to the vector
                    foundPaths.push_back(new_path);
                }
                else
                {   // ıf not matches
                    // Continue searching for flights from the same location (from becomes the previous one's to).
                    // keep decreasing the maxTransfer so that it does not do a recursive iteration infinitely
                    pathFinderRecursion(head, currentFlight->to, new_to, maxTransfers - 1, new_path, foundPaths);
                }
            }
            currentFlight = currentFlight->next;
        }
        currentHeadPtr = currentHeadPtr->next;
    }
}

bool sortByPrice(path i, path j)//sort function for the totalPrice sort
{
    return (i.totalPrice < j.totalPrice);
}



int pathfinder(airline* head){
    
    vector<path> foundPaths;
    int max_transfers = 0;
    string new_from, new_to;
    cout << "Where are you now? "; cin >> new_from;
    cout << "Where do you want to go? "; cin >> new_to;
    cout << "Maximum number of transfers: " ; cin >> max_transfers;

    
    path currPath; // Initial empty route
    pathFinderRecursion(head, new_from, new_to, max_transfers, currPath, foundPaths);//calling recursive function


    sort(foundPaths.begin(), foundPaths.end(), sortByPrice);//sorting the foundPaths vector by totalPrice
    
    cout << "##Best price path##" << endl;
    
    if (foundPaths.empty()) 
    {
        cout << "No path found between: " << new_from << " and " << new_to << endl;
    } 
    else
    {
        // The first route is the cheapest since we sorted it by totalPrice
        cout << "Cheapest Route:" << endl;
        for (int i = 0; i < foundPaths[0].flights.size(); ++i) {
            
            //the first path will be the cheapest one since we have already sorted by totalPrice
            cout << "[" << foundPaths[0].flights[i].ID << "|" << foundPaths[0].flights[i].from << "->" << foundPaths[0].flights[i].to << "|" << foundPaths[0].flights[i].hour << ":" << foundPaths[0].flights[i].min << "|" << foundPaths[0].flights[i].price << "]";
            
            if (i < foundPaths[0].flights.size() - 1) {
                cout << "->";
            }
        }
        
        cout << " $" << "TOTAL PRICE: " << foundPaths[0].totalPrice << endl;
    }
    
    return 0;
}
    




void delete_linked_list(airline* &head){

    airline *currentHeadPtr = head;
    flight *current_flight = nullptr;
    flight *temp_flight = nullptr;
    
    while(head)
    {
        head->flights = current_flight;
        //deleting the flights
        while(current_flight)
        {
            //initializing the temp pointer to the second(next) flight so that we can delete the first
            temp_flight = current_flight->next;
            head->flights = temp_flight;
            
            //initializing the pointer's prev pointer to nullptr so that there will be no connection that can cause an error
            temp_flight->prev = nullptr;
            
            delete current_flight;
            current_flight = temp_flight;//setting current_flight to the next flight to be deleted
            
        }
        
        currentHeadPtr = head->next; //keeping the head next in a pointer
        delete head;//deleting the head
        head = currentHeadPtr;//setting the head again so that we can delete the next airline when it is empty
    }

}

void print_all(airline *head)
{
    airline *currentHeadPtr = head;
    flight *current_flight = nullptr;
    
    if (head == nullptr)//if head is nullptr then it is empty
    {
        cout << "List is empty" << endl;
    }
    
    else
    {
        while(currentHeadPtr != nullptr)
        {
            cout << "##################" << endl;
            cout << "### AIRLINE ID: " << currentHeadPtr -> ID <<" ###" << endl;
            cout << "Name: " << currentHeadPtr -> name << endl;
            cout << "Flights: " << endl;
            
            current_flight = currentHeadPtr -> flights;
            
            while(current_flight != nullptr)
            {
                //printing the info's of the flights one by one in a double while loop
                
                cout << "#[" << current_flight->ID << "|" << current_flight->from << "->" << current_flight->to << "|" << current_flight->hour << ":" << current_flight->min << "|" << current_flight->price << "TRY]#";
                current_flight = current_flight->next;
            }
            currentHeadPtr = currentHeadPtr->next;
            
        }
        cout << endl;
    }
}



void add_flight_with_input(airline *head)
{
    airline *currentHeadPtr = head;
    flight *current_flight = nullptr;
    
    string airline_name;
    string new_from;
    string new_to;
    int new_hour = 0;
    int new_min = 0;
    int new_price = 0;
   
    cout << "Adding manual entry: " << endl;
    cout << "AIRLINE: "; cin >> airline_name;
    cout << "FROM: "; cin >> new_from;
    cout << "TO: "; cin >> new_to;
    cout << "HOUR: "; cin >> new_hour;
    cout << "MIN: "; cin >> new_min;
    cout << "PRICE: "; cin >> new_price;
    
    
    //calculating the total id of the flights in the linkedlist
    flight *temp = nullptr; //creating temp pointer for the id count
    int counter = 0;
    int airline_counter = 0;
    while (currentHeadPtr != nullptr)
    {
        temp = currentHeadPtr->flights;
        while(temp != nullptr)
        {
            counter++;
            temp = temp->next;
        }
        currentHeadPtr = currentHeadPtr->next;
        airline_counter++;
    }
    currentHeadPtr = head;// initializing the current_airline to head again to use it in the adding phase
    
    //creating a new node for the addition proccess
    flight* new_flight = new flight(new_from, new_to, new_hour, new_min, new_price, counter,  nullptr, nullptr);
    
    
    airline *prev_air = head;
    //finding the linked list for the given airline name
    while(currentHeadPtr && airline_name != currentHeadPtr->name)
    {
        currentHeadPtr = currentHeadPtr->next;
    }
    //finding the last airline so that we can add new airline to its' next
    while (prev_air->next != nullptr)
    {
        prev_air = prev_air->next;
    }
    
    //if there is no match with the user enterance we create a new one and add it to the linked list
    if (currentHeadPtr == nullptr)
    {
        airline *temp = new airline(airline_name, airline_counter, nullptr, nullptr);
        
        prev_air->next = temp;
        
        temp = nullptr;
        
        prev_air->next->flights = new_flight;
        
    }
    
    else
    {
        //initializing the current_flight to the first flight of the airline
        current_flight = currentHeadPtr->flights;
        
        //if the first flights hour is bigger than the one in the list make the new flight the first one
        if (current_flight->hour > new_hour)
        {
            currentHeadPtr->flights = new_flight;// making the new flight the first light in the list
            new_flight->next = current_flight;//making the new flights next to the previous first flight
            current_flight->prev = new_flight;//making the previous first flight's prev the new flight
            new_flight->prev = nullptr;
        }
        else{
            //adding the flight as sorted by hour
            while(current_flight->next && new_hour > current_flight->next->hour)
            {
                current_flight = current_flight->next;
            }
            
            //if the new flight will become the last one we implement this
            if (current_flight->next == nullptr)
            {
                current_flight->next = new_flight;
                new_flight->prev = current_flight;
                new_flight->next = nullptr;
            }
            else
            {
                //if the new flight to be added is in the middle we implement this
                //current flight is one step behind then the position to be inserted
                new_flight->next = current_flight->next;//new flights next will be the current flights next
                current_flight->next->prev = new_flight;//the node ahead from the currnet flight's previous will become new flight
                current_flight->next = new_flight;//current flight's next will become the new flight (inserted one)
                new_flight->prev = current_flight; // and new fligths previous will become the current flight
            }
        }
    }
    cout << "Flight ID " << glob_IDs + 1 << " is added to the list."<< endl;
    glob_IDs++;
}



void remove_flight_with_input(airline *head)
{
    
    airline *currentHeadPtr = head;
    flight *current_flight = nullptr;
    
    int flight_ID = 0;
    
    cout << "Flight id to remove: ";cin >> flight_ID;
    cout << "Removing flight id: " << flight_ID << endl;
    

    //finding the node to be deleted
    while (currentHeadPtr != nullptr)
    {
        current_flight = currentHeadPtr->flights;
        
        //Iterate the loop while the current flight is not nullptr and current flight's ID is not matching with the input
        while(current_flight != nullptr && current_flight->ID != flight_ID)
        {
            current_flight = current_flight->next;
        }
        if(current_flight == nullptr)//check if the current flight is null ptr, if so check the next airline
        {
            currentHeadPtr = currentHeadPtr->next;
        }
        else//if current flight is not null ptr then the loop ended because current ptr is pointing to toBeDeleted so break the loop
        {
            break;
        }
    }
    
    //creating a pointer to point toBeDeleted
    flight *toBeDeleted = current_flight;
    //current airline pointer is on the airline that stores the toBeDeleted
    current_flight = nullptr;
    
    current_flight = currentHeadPtr->flights;//making the current fligt the first element of that airline flights
    
    if(toBeDeleted == current_flight)//checking if the first flight is the toBeDeleted
    {
        current_flight = current_flight->next;//finding the next flight so that we can delete the previous
        
        currentHeadPtr->flights = current_flight;// setting the first flight as the second
        current_flight->prev = nullptr;
        delete toBeDeleted;//deleting the toBeDeleted
        
        
        //if toBeDeleted is the only flight in that airline we delete the airline as well
        airline *curr_airlinePtr = head;
        if (current_flight == nullptr)
        {
            while(curr_airlinePtr->next != currentHeadPtr)
            {
                curr_airlinePtr = curr_airlinePtr->next;
            }
            if(currentHeadPtr->next == nullptr)
            {
                curr_airlinePtr->next = nullptr;
                delete currentHeadPtr;
                currentHeadPtr = nullptr;
            }
            else
            {
                curr_airlinePtr->next = currentHeadPtr->next;
                delete currentHeadPtr;
                currentHeadPtr = nullptr;
            }
        }
    }
    
    else
    {
        while(current_flight->next && current_flight->next != toBeDeleted)
        {
            current_flight = current_flight->next;
        }//after while loop we are keeping the current flight just before the toBeDeleted
        if(toBeDeleted->next == nullptr)
        {
            //if toBeDeleted is the last flight in the list we implement this part
            current_flight->next = nullptr;
            delete toBeDeleted;
            toBeDeleted = nullptr;
        }
        else
        {
            //if toBeDeleted is in the middle we implement this part
            current_flight->next = toBeDeleted->next;
            toBeDeleted->next->prev = current_flight;
            delete toBeDeleted;
            toBeDeleted = nullptr;
        }
    }
}





void printMainMenu() {
  cout << endl;
  cout <<"I***********************************************I"<<endl
       <<"I               0 - DELETE DATA                 I"<<endl
       <<"I               1 - READ FILES                  I"<<endl
       <<"I               2 - PRINT ALL FLIGHTS           I"<<endl
       <<"I               3 - ADD FLIGHT                  I"<<endl
       <<"I               4 - REMOVE FLIGHT               I"<<endl
       <<"I               5 - PATH FINDER                 I"<<endl
       <<"I               6 - EXIT                        I"<<endl
       <<"I***********************************************I"<<endl
       <<">>";
  cout << endl;
}  



void processMainMenu() {

  pair<vector<string>, vector<vector<flight>>> lines_flights;
  airline* head;
  bool input_done = false;
  char input;
  do{
    printMainMenu();
    cout << "Please enter your option " << endl;
    cin >> input;
    switch (input) {
    case '0':
      cout << "Commented out functionalities are going to be implemented" << endl;
      delete_linked_list(head);
      cout << "Data is deleted!" << endl;
      input_done = false;
      break;
    case '1':
      if(not input_done){
	lines_flights = read_files(input_done);
	head = make_linked_list_structure(lines_flights.first, lines_flights.second);
	cout << "Files are read.." << endl;
      }
      else{
	cout << "Files are already read.." << endl;
      }
      input_done = true;
      break;
    case '2':
      cout << "Commented out functionalities are going to be implemented" << endl;
      print_all(head);
      break;
    case '3':
      cout << "Commented out functionalities are going to be implemented" << endl;
      add_flight_with_input(head);
      break;
    case '4':
      cout << "Commented out functionalities are going to be implemented" << endl;
      remove_flight_with_input(head);
      break;
    case '5':
      cout << "Commented out functionalities are going to be implemented" << endl;
      pathfinder(head);
      break;
    case '6':
      cout << "Exiting.." << endl;
      exit(0);
    default:
      cout << "Invalid option: please enter again" << endl;
    }
  } while(true);
}
//


int main(){
  
  processMainMenu();
  //Comply with the provided structs and specified linked list structure for a seamless grading
  
  return 0;
  
}



  
