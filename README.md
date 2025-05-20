# Flight Route Optimization
## Features

- Display all available airports with their codes and names
- Display all available flight routes with distance, duration, and cost
- Find the optimal route between two airports using Dijkstra's algorithm
- Add new airports to the network
- Add new flight routes between existing airports
- Data persistence: saves all airports and routes to a file
- Automatically loads previously saved data on startup
- User-friendly menu-driven interface

## How to Use
 Main Menu Options:
   ```
   === Flight Route Optimization System ===
   1. Find optimal route between airports
   2. Display all airports
   3. Display all routes
   4. Add a new route
   5. Add a new airport
   6. Exit
   ```

## Detailed Instructions

### Finding Optimal Routes
1. Select option 1 from the main menu
2. Enter the source airport code (e.g., DEL for Delhi)
3. Enter the destination airport code (e.g., BOM for Mumbai)
4. The program will display the optimal route, total distance, duration, and cost

### Viewing Airports
1. Select option 2 from the main menu
2. The program will display all available airports with their codes and names

### Viewing Routes
1. Select option 3 from the main menu
2. The program will display all available routes with distance, duration, and cost

### Adding a New Route
1. Select option 4 from the main menu
2. Enter the source airport code (must be an existing airport)
3. Enter the destination airport code (must be an existing airport)
4. Enter the distance between the airports (in units)
5. Enter the flight duration (in minutes)
6. Enter the cost (in units)
7. The route will be added to the network and saved automatically when you exit

### Adding a New Airport
1. Select option 5 from the main menu
2. Enter a unique 2-3 letter airport code (e.g., JAI for Jaipur)
3. Enter the full name of the airport
4. The airport will be added to the network and saved automatically when you exit

### Data Persistence
- All changes (new airports and routes) are automatically saved to "flight_network.dat" when you exit
- When you restart the program, it will load all previously saved data
- If no data file exists, the program will start with the default network

## Airport Codes

The program includes the following Indian airports:
- DEL: Indira Gandhi International Airport (Delhi)
- BOM: Chhatrapati Shivaji International Airport (Mumbai)
- MAA: Chennai International Airport
- BLR: Kempegowda International Airport (Bangalore)
- HYD: Rajiv Gandhi International Airport (Hyderabad)
- CCU: Netaji Subhas Chandra Bose International Airport (Kolkata)
- COK: Cochin International Airport

## Implementation Details

- Uses adjacency list representation for the flight network
- Implements Dijkstra's algorithm with a min-heap for efficient path finding
- Considers multiple factors: distance, duration, and cost
- Stores data in a CSV-like format in "flight_network.dat"
- Provides data persistence between program runs

## Example Usage

### Finding the optimal route from Delhi to Mumbai:
1. Select option 1 from the main menu
2. Enter "DEL" as the source airport code
3. Enter "BOM" as the destination airport code
4. The program will display:
   ```
   Optimal route from DEL to BOM:
   Path: DEL -> BOM
   Total Distance: 1148 units
   Total Duration: 125 minutes
   Total Cost: 7500 units
   ```

### Adding a new airport (Jaipur):
1. Select option 5 from the main menu
2. Enter "JAI" as the airport code
3. Enter "Jaipur International Airport" as the airport name
4. The airport will be added to the network

### Adding a new route (Delhi to Jaipur):
1. Select option 4 from the main menu
2. Enter "DEL" as the source airport code
3. Enter "JAI" as the destination airport code
4. Enter "300" as the distance
5. Enter "45" as the duration
6. Enter "3000" as the cost
7. The route will be added to the network

## Data File Format

The program stores all data in a file named "flight_network.dat" in the following format:

```
[number of airports]
[airport1_code],[airport1_name]
[airport2_code],[airport2_name]
...
[source_code],[destination_code],[distance],[duration],[cost]
[source_code],[destination_code],[distance],[duration],[cost]
...
```

Example:
```
3
DEL,Indira Gandhi International Airport
BOM,Chhatrapati Shivaji International Airport
JAI,Jaipur International Airport
DEL,BOM,1148,125,7500
BOM,DEL,1148,130,7800
DEL,JAI,300,45,3000
```

This file is automatically created and updated by the program. You don't need to modify it manually.
