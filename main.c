#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#define INF INT_MAX

typedef struct {
    char code[4];
    char name[50];
} Airport;

typedef struct {
    int src;
    int dest;
    int distance;
    int duration;
    int cost;
} Route;

typedef struct {
    Airport airports[100];
    int numAirports;
    struct AdjListNode* adjList[100];
} FlightNetwork;

typedef struct AdjListNode {
    int dest;
    int distance;
    int duration;
    int cost;
    struct AdjListNode* next;
} AdjListNode;

typedef struct {
    int airport;
    int distance;
    int totalDuration;
    int totalCost;
} HeapNode;

typedef struct {
    int capacity;
    int size;
    int *pos;
    HeapNode** array;
} MinHeap;

// Forward declarations
void freeNetwork(FlightNetwork* network);

AdjListNode* createAdjListNode(int dest, int distance, int duration, int cost) {
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->distance = distance;
    newNode->duration = duration;
    newNode->cost = cost;
    newNode->next = NULL;
    return newNode;
}

FlightNetwork* createFlightNetwork() {
    FlightNetwork* network = (FlightNetwork*)malloc(sizeof(FlightNetwork));
    network->numAirports = 0;

    for (int i = 0; i < 100; i++) {
        network->adjList[i] = NULL;
    }

    return network;
}

int addAirport(FlightNetwork* network, const char* code, const char* name) {
    if (network->numAirports >= 100) {
        printf("Error: Maximum number of airports reached\n");
        return -1;
    }

    for (int i = 0; i < network->numAirports; i++) {
        if (strcmp(network->airports[i].code, code) == 0) {
            printf("Airport %s already exists\n", code);
            return i;
        }
    }

    int index = network->numAirports;
    strcpy(network->airports[index].code, code);
    strcpy(network->airports[index].name, name);

    network->numAirports++;
    return index;
}

int findAirportIndex(FlightNetwork* network, const char* code) {
    for (int i = 0; i < network->numAirports; i++) {
        if (strcmp(network->airports[i].code, code) == 0) {
            return i;
        }
    }
    return -1;
}

void addRoute(FlightNetwork* network, const char* srcCode, const char* destCode,
              int distance, int duration, int cost) {
    int srcIndex = findAirportIndex(network, srcCode);
    int destIndex = findAirportIndex(network, destCode);

    if (srcIndex == -1 || destIndex == -1) {
        printf("Error: One or both airports not found\n");
        return;
    }

    AdjListNode* newNode = createAdjListNode(destIndex, distance, duration, cost);
    newNode->next = network->adjList[srcIndex];
    network->adjList[srcIndex] = newNode;
}

MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->pos = (int*)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (HeapNode**)malloc(capacity * sizeof(HeapNode*));
    return minHeap;
}

HeapNode* createHeapNode(int v, int dist, int duration, int cost) {
    HeapNode* node = (HeapNode*)malloc(sizeof(HeapNode));
    node->airport = v;
    node->distance = dist;
    node->totalDuration = duration;
    node->totalCost = cost;
    return node;
}

void swapHeapNodes(HeapNode** a, HeapNode** b) {
    HeapNode* temp = *a;
    *a = *b;
    *b = temp;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < minHeap->size &&
        minHeap->array[left]->distance < minHeap->array[smallest]->distance)
        smallest = left;

    if (right < minHeap->size &&
        minHeap->array[right]->distance < minHeap->array[smallest]->distance)
        smallest = right;

    if (smallest != idx) {
        HeapNode* smallestNode = minHeap->array[smallest];
        HeapNode* idxNode = minHeap->array[idx];

        minHeap->pos[smallestNode->airport] = idx;
        minHeap->pos[idxNode->airport] = smallest;

        swapHeapNodes(&minHeap->array[smallest], &minHeap->array[idx]);

        minHeapify(minHeap, smallest);
    }
}

int isEmpty(MinHeap* minHeap) {
    return minHeap->size == 0;
}

HeapNode* extractMin(MinHeap* minHeap) {
    if (isEmpty(minHeap))
        return NULL;

    HeapNode* root = minHeap->array[0];

    HeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    minHeap->pos[root->airport] = minHeap->size - 1;
    minHeap->pos[lastNode->airport] = 0;

    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

void decreaseKey(MinHeap* minHeap, int v, int dist, int duration, int cost) {
    int i = minHeap->pos[v];

    minHeap->array[i]->distance = dist;
    minHeap->array[i]->totalDuration = duration;
    minHeap->array[i]->totalCost = cost;

    while (i && minHeap->array[i]->distance < minHeap->array[(i - 1) / 2]->distance) {
        minHeap->pos[minHeap->array[i]->airport] = (i - 1) / 2;
        minHeap->pos[minHeap->array[(i - 1) / 2]->airport] = i;
        swapHeapNodes(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

        i = (i - 1) / 2;
    }
}

bool isInMinHeap(MinHeap* minHeap, int v) {
    if (minHeap->pos[v] < minHeap->size)
        return true;
    return false;
}

void printPath(int* prev, int dest, FlightNetwork* network) {
    if (prev[dest] == -1) {
        printf("%s", network->airports[dest].code);
        return;
    }

    printPath(prev, prev[dest], network);
    printf(" -> %s", network->airports[dest].code);
}

void dijkstra(FlightNetwork* network, int src, int dest) {
    int V = network->numAirports;

    int* dist = (int*)malloc(V * sizeof(int));
    int* duration = (int*)malloc(V * sizeof(int));
    int* cost = (int*)malloc(V * sizeof(int));

    int* prev = (int*)malloc(V * sizeof(int));

    MinHeap* minHeap = createMinHeap(V);

    for (int v = 0; v < V; v++) {
        dist[v] = INF;
        duration[v] = INF;
        cost[v] = INF;
        prev[v] = -1;
        minHeap->array[v] = createHeapNode(v, dist[v], duration[v], cost[v]);
        minHeap->pos[v] = v;
    }

    dist[src] = 0;
    duration[src] = 0;
    cost[src] = 0;
    decreaseKey(minHeap, src, dist[src], duration[src], cost[src]);

    minHeap->size = V;

    while (!isEmpty(minHeap)) {
        HeapNode* minNode = extractMin(minHeap);
        int u = minNode->airport;

        if (u == dest)
            break;

        AdjListNode* pCrawl = network->adjList[u];
        while (pCrawl != NULL) {
            int v = pCrawl->dest;

            if (isInMinHeap(minHeap, v) && dist[u] != INF &&
                pCrawl->distance + dist[u] < dist[v]) {

                dist[v] = dist[u] + pCrawl->distance;
                duration[v] = duration[u] + pCrawl->duration;
                cost[v] = cost[u] + pCrawl->cost;

                prev[v] = u;

                decreaseKey(minHeap, v, dist[v], duration[v], cost[v]);
            }
            pCrawl = pCrawl->next;
        }

        free(minNode);
    }

    if (dist[dest] == INF) {
        printf("No path exists from %s to %s\n",
               network->airports[src].code, network->airports[dest].code);
    } else {
        printf("Optimal route from %s to %s:\n",
               network->airports[src].code, network->airports[dest].code);
        printf("Path: ");
        printPath(prev, dest, network);
        printf("\nTotal Distance: %d units\n", dist[dest]);
        printf("Total Duration: %d minutes\n", duration[dest]);
        printf("Total Cost: %d units\n", cost[dest]);
    }

    free(dist);
    free(duration);
    free(cost);
    free(prev);
    for (int i = 0; i < minHeap->capacity; i++) {
        if (i < minHeap->size && minHeap->array[i]) {
            free(minHeap->array[i]);
        }
    }
    free(minHeap->array);
    free(minHeap->pos);
    free(minHeap);
}

void findMinCostRoute(FlightNetwork* network, int src, int dest) {
    printf("Finding minimum cost route...\n");
}

void displayAllAirports(FlightNetwork* network) {
    printf("\n=== Available Airports ===\n");
    printf("%-5s %-50s\n", "Code", "Name");
    printf("----------------------------------------\n");

    for (int i = 0; i < network->numAirports; i++) {
        printf("%-5s %-50s\n",
               network->airports[i].code,
               network->airports[i].name);
    }
    printf("\n");
}

// Helper function to convert a string to uppercase
void toUpperCase(char* str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = str[i] - 32;
        }
    }
}

void displayAllRoutes(FlightNetwork* network) {
    printf("\n=== Available Routes ===\n");
    printf("%-5s %-5s %-10s %-10s %-10s\n",
           "From", "To", "Distance", "Duration", "Cost");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < network->numAirports; i++) {
        AdjListNode* current = network->adjList[i];

        while (current) {
            printf("%-5s %-5s %-10d %-10d %-10d\n",
                   network->airports[i].code,
                   network->airports[current->dest].code,
                   current->distance,
                   current->duration,
                   current->cost);

            current = current->next;
        }
    }
    printf("\n");
}

// Save the flight network data to a file
void saveNetworkToFile(FlightNetwork* network, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    // Write the number of airports
    fprintf(file, "%d\n", network->numAirports);

    // Write airport data
    for (int i = 0; i < network->numAirports; i++) {
        fprintf(file, "%s,%s\n",
                network->airports[i].code,
                network->airports[i].name);
    }

    // Write route data
    for (int i = 0; i < network->numAirports; i++) {
        AdjListNode* current = network->adjList[i];
        while (current) {
            fprintf(file, "%s,%s,%d,%d,%d\n",
                   network->airports[i].code,
                   network->airports[current->dest].code,
                   current->distance,
                   current->duration,
                   current->cost);

            current = current->next;
        }
    }

    fclose(file);
    printf("Network data saved to %s\n", filename);
}

// Load the flight network data from a file
FlightNetwork* loadNetworkFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("No existing data file found. Starting with default network.\n");
        return NULL;
    }

    FlightNetwork* network = createFlightNetwork();

    // Read the number of airports
    int numAirports;
    if (fscanf(file, "%d\n", &numAirports) != 1) {
        printf("Error reading number of airports from file\n");
        freeNetwork(network);
        fclose(file);
        return NULL;
    }

    // Read airport data
    for (int i = 0; i < numAirports; i++) {
        char code[4], name[100];
        char line[150];

        if (fgets(line, sizeof(line), file) == NULL) {
            printf("Error reading airport data from file\n");
            freeNetwork(network);
            fclose(file);
            return NULL;
        }

        // Parse the line to get code and name
        char* token = strtok(line, ",");
        if (token == NULL) {
            printf("Error parsing airport code\n");
            continue;
        }
        strcpy(code, token);

        token = strtok(NULL, "\n");
        if (token == NULL) {
            printf("Error parsing airport name\n");
            continue;
        }
        strcpy(name, token);

        // Add the airport to the network
        addAirport(network, code, name);
    }

    // Read route data
    char line[150];
    while (fgets(line, sizeof(line), file) != NULL) {
        char srcCode[4], destCode[4];
        int distance, duration, cost;

        // Parse the line to get route data
        char* token = strtok(line, ",");
        if (token == NULL) {
            printf("Error parsing source airport code\n");
            continue;
        }
        strcpy(srcCode, token);

        token = strtok(NULL, ",");
        if (token == NULL) {
            printf("Error parsing destination airport code\n");
            continue;
        }
        strcpy(destCode, token);

        token = strtok(NULL, ",");
        if (token == NULL || sscanf(token, "%d", &distance) != 1) {
            printf("Error parsing distance\n");
            continue;
        }

        token = strtok(NULL, ",");
        if (token == NULL || sscanf(token, "%d", &duration) != 1) {
            printf("Error parsing duration\n");
            continue;
        }

        token = strtok(NULL, "\n");
        if (token == NULL || sscanf(token, "%d", &cost) != 1) {
            printf("Error parsing cost\n");
            continue;
        }

        // Add the route to the network
        addRoute(network, srcCode, destCode, distance, duration, cost);
    }

    fclose(file);
    printf("Network data loaded from %s\n", filename);
    return network;
}

void freeNetwork(FlightNetwork* network) {
    for (int i = 0; i < network->numAirports; i++) {
        AdjListNode* current = network->adjList[i];
        while (current) {
            AdjListNode* next = current->next;
            free(current);
            current = next;
        }
    }

    free(network);
}

int main() {
    // Define the data file name
    const char* dataFile = "flight_network.dat";

    // Try to load network from file
    FlightNetwork* network = loadNetworkFromFile(dataFile);

    // If loading failed, create a default network
    if (network == NULL) {
        network = createFlightNetwork();

        // Add default airports
        addAirport(network, "DEL", "Indira Gandhi International Airport");
        addAirport(network, "BOM", "Chhatrapati Shivaji International Airport");
        addAirport(network, "MAA", "Chennai International Airport");
        addAirport(network, "BLR", "Kempegowda International Airport");
        addAirport(network, "HYD", "Rajiv Gandhi International Airport");
        addAirport(network, "CCU", "Netaji Subhas Chandra Bose International Airport");
        addAirport(network, "COK", "Cochin International Airport");

        // Add default routes
        addRoute(network, "DEL", "BOM", 1148, 125, 7500);
        addRoute(network, "DEL", "MAA", 1760, 150, 8500);
        addRoute(network, "DEL", "BLR", 1740, 150, 8200);
        addRoute(network, "DEL", "CCU", 1300, 120, 7000);

        addRoute(network, "BOM", "DEL", 1148, 130, 7800);
        addRoute(network, "BOM", "BLR", 845, 90, 5000);
        addRoute(network, "BOM", "HYD", 620, 70, 4500);

        addRoute(network, "MAA", "DEL", 1760, 155, 8700);
        addRoute(network, "MAA", "BLR", 284, 45, 3000);
        addRoute(network, "MAA", "COK", 500, 60, 3500);

        addRoute(network, "BLR", "DEL", 1740, 155, 8500);
        addRoute(network, "BLR", "BOM", 845, 95, 5200);
        addRoute(network, "BLR", "MAA", 284, 50, 3200);
        addRoute(network, "BLR", "HYD", 500, 60, 3800);

        addRoute(network, "HYD", "BOM", 620, 75, 4700);
        addRoute(network, "HYD", "BLR", 500, 65, 4000);

        addRoute(network, "CCU", "DEL", 1300, 125, 7200);
        addRoute(network, "CCU", "MAA", 1370, 130, 7800);

        addRoute(network, "COK", "MAA", 500, 65, 3700);
        addRoute(network, "COK", "BLR", 360, 55, 3500);
    }

    // Display all airports and routes
    displayAllAirports(network);
    displayAllRoutes(network);

    char choice;
    do {
        char srcCode[10], destCode[10];
        int srcIndex, destIndex;

        printf("\n=== Flight Route Optimization System ===\n");
        printf("1. Find optimal route between airports\n");
        printf("2. Display all airports\n");
        printf("3. Display all routes\n");
        printf("4. Add a new route\n");
        printf("5. Add a new airport\n");
        printf("6. Exit\n");
        printf("Enter your choice (1-6): ");
        scanf(" %c", &choice);

        switch (choice) {
            case '1':
                printf("\nEnter source airport code: ");
                scanf("%s", srcCode);
                printf("Enter destination airport code: ");
                scanf("%s", destCode);

                // Convert to uppercase
                toUpperCase(srcCode);
                toUpperCase(destCode);

                srcIndex = findAirportIndex(network, srcCode);
                destIndex = findAirportIndex(network, destCode);

                if (srcIndex != -1 && destIndex != -1) {
                    printf("\n=== Finding optimal route based on distance ===\n");
                    dijkstra(network, srcIndex, destIndex);
                } else {
                    printf("Invalid source or destination airport code\n");
                }
                break;

            case '2':
                displayAllAirports(network);
                break;

            case '3':
                displayAllRoutes(network);
                break;

            case '4': {
                char srcCode[10], destCode[10];
                int distance, duration, cost;

                printf("\n=== Add a New Route ===\n");

                printf("Enter source airport code: ");
                scanf("%s", srcCode);
                printf("Enter destination airport code: ");
                scanf("%s", destCode);

                // Convert to uppercase
                toUpperCase(srcCode);
                toUpperCase(destCode);

                // Check if airports exist
                srcIndex = findAirportIndex(network, srcCode);
                destIndex = findAirportIndex(network, destCode);

                if (srcIndex == -1 || destIndex == -1) {
                    printf("Error: One or both airports not found. Please add the airports first.\n");
                    break;
                }

                printf("Enter distance (in units): ");
                scanf("%d", &distance);
                printf("Enter duration (in minutes): ");
                scanf("%d", &duration);
                printf("Enter cost (in units): ");
                scanf("%d", &cost);

                // Validate input
                if (distance <= 0 || duration <= 0 || cost <= 0) {
                    printf("Error: Distance, duration, and cost must be positive values.\n");
                    break;
                }

                // Add the route
                addRoute(network, srcCode, destCode, distance, duration, cost);
                printf("Route from %s to %s added successfully!\n", srcCode, destCode);
                break;
            }

            case '5': {
                char code[10], name[100];

                printf("\n=== Add a New Airport ===\n");

                printf("Enter airport code (2-3 letters): ");
                scanf("%s", code);

                // Convert to uppercase
                toUpperCase(code);

                // Check if code is valid
                if (strlen(code) < 2 || strlen(code) > 3) {
                    printf("Error: Airport code must be 2-3 letters.\n");
                    break;
                }

                // Check if airport already exists
                if (findAirportIndex(network, code) != -1) {
                    printf("Error: Airport with code %s already exists.\n", code);
                    break;
                }

                printf("Enter airport name: ");
                getchar(); // Clear input buffer
                fgets(name, 100, stdin);

                // Remove newline character if present
                size_t len = strlen(name);
                if (len > 0 && name[len-1] == '\n') {
                    name[len-1] = '\0';
                }

                // Add the airport
                int index = addAirport(network, code, name);
                if (index != -1) {
                    printf("Airport %s (%s) added successfully!\n", code, name);
                }
                break;
            }

            case '6':
                printf("Saving network data and exiting...\n");
                saveNetworkToFile(network, dataFile);
                printf("Goodbye!\n");
                break;

            default:
                printf("Invalid choice. Please try again.\n");
        }

    } while (choice != '6');

    freeNetwork(network);

    return 0;
}
