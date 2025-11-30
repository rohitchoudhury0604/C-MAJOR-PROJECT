#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#define MAX_FLIGHTS 100

#define MAX_TICKETS 1000
#define MAX_STR_LEN 50
#define FLIGHT_FILE "flight_data.dat"
#define TICKET_FILE "ticket_data.dat"
#define PNR_COUNTER_FILE "pnr_counter.dat"
typedef struct {
    char flightId[10];      
    char source[MAX_STR_LEN]; 
    char destination[MAX_STR_LEN];
    int capacity;           
    int availableSeats;     
    float price;            
} Flight;
typedef struct {
    long pnr;               
    char flightId[10];      
    char passengerName[MAX_STR_LEN]; 
    int seatNumber;         
} Ticket;
Flight flights[MAX_FLIGHTS];       
int flightCount = 0;                   
Ticket tickets[MAX_TICKETS];       
int ticketCount = 0;                   
long nextPNRValue = 1000000;                 

int getIntegerInput(const char* prompt);
float getPositiveFloatInput(const char* prompt);
long getNextPNR();
static int findFlightIndex(const char* flightId);
int loadPNRCounter();
int savePNRCounter();
int loadData();
int saveData();
void initializeSystem();

void displayMainMenu();
void executeOption(int option);

void addFlight();
void displayAllFlights();
void searchFlights();
void bookTicket();
void cancelTicket();
void displayBookingsByPNR();

int getIntegerInput(const char* prompt) {
    int input;
    int c;
    printf("%s", prompt);
    while (scanf("%d", &input) != 1) {
        printf("Invalid input. Please enter a valid number: ");
        while ((c = getchar()) != '\n' && c != EOF);
    }
    while ((c = getchar()) != '\n' && c != EOF);
    return input;
}
float getPositiveFloatInput(const char* prompt) {
    float input;
    int c;
    do {
        printf("%s", prompt);
        if (scanf("%f", &input) != 1) {
            printf("Invalid input. Please enter a valid price (e.g., 4500.00): ");
            while ((c = getchar()) != '\n' && c != EOF);
            input = -1.0; 
        }
        while ((c = getchar()) != '\n' && c != EOF);
        
        if (input <= 0 && input != -1.0) {
            printf("Value must be positive. Please re-enter.\n");
        }
        
    } while (input <= 0); 
    
    return input;
}
long getNextPNR() {
    return nextPNRValue++;
}
void displayMainMenu() {
    printf("\n==============================================\n");
    printf("     AIRLINE TICKET MANAGEMENT SYSTEM\n");
    printf("==============================================\n");
    printf("1. Add New Flight\n");
    printf("2. Display All Flights\n");
    printf("3. Search Flights (by source/destination)\n");
    printf("4. Book Ticket\n");
    printf("5. Cancel Ticket (by PNR)\n");
    printf("6. View Booking Details (by PNR)\n");
    printf("7. Save & Exit\n");
    printf("----------------------------------------------\n");
}

void executeOption(int option) {
    switch (option) {
        case 1:
            addFlight();
            break;
        case 2:
            displayAllFlights();
            break;
        case 3:
            searchFlights();
            break;
        case 4:
            bookTicket();
            break;
        case 5:
            cancelTicket();
            break;
        case 6:
            displayBookingsByPNR();
            break;
        case 7:
            printf("\nSaving data and exiting. Goodbye!\n");
            break;
        default:
            printf("Error: Invalid option. Please enter a number between 1 and 7.\n");
    }
}
int main() {
    srand(time(NULL)); 
    int choice = 0;
    printf("--- Initializing System ---\n");
    loadPNRCounter();
    initializeSystem();
    do {
        displayMainMenu();
        choice = getIntegerInput("Enter your choice (1-7): ");
        
        executeOption(choice);

        if (choice == 7) {
            saveData();
            savePNRCounter(); 
            break;
        }
        
    } while (1); 

    return 0;
}
void initializeSystem() {
    if (loadData() == 0) {
        printf("Status: Data loaded successfully. (%d flights, %d tickets)\n", flightCount, ticketCount);
    } else {
        printf("Status: No existing flight/ticket data found. Starting with fresh data.\n");
    }
}
int loadPNRCounter() {
    FILE *fp = fopen(PNR_COUNTER_FILE, "rb");
    if (fp == NULL) {
        return -1;
    }
    if (fread(&nextPNRValue, sizeof(long), 1, fp) != 1) {
        printf("Warning: Error reading PNR counter file. Resetting PNR to default.\n");
        nextPNRValue = 1000000;
        fclose(fp);
        return -1;
    }
    
    printf("Status: PNR counter loaded. Next PNR will be %ld.\n", nextPNRValue);
    fclose(fp);
    return 0;
}
int savePNRCounter() {
    FILE *fp = fopen(PNR_COUNTER_FILE, "wb");
    if (fp == NULL) {
        printf("Error: Could not open PNR counter file for writing.\n");
        return -1;
    }
    if (fwrite(&nextPNRValue, sizeof(long), 1, fp) != 1) {
        printf("Error: Failed to write PNR counter to file.\n");
        fclose(fp);
        return -1;
    }
    
    fclose(fp);
    return 0;
}
int loadData() {
    FILE *fp;
    int success = 0;
    fp = fopen(FLIGHT_FILE, "rb");
    if (fp != NULL) {
        if (fread(&flightCount, sizeof(int), 1, fp) == 1) {
            if (fread(flights, sizeof(Flight), flightCount, fp) == flightCount) {
                 success++;
            } else {
                 printf("Warning: Mismatch in flight data count and actual records.\n");
                 flightCount = 0;
            }
        } else {
            flightCount = 0; 
        }
        fclose(fp);
    }
    fp = fopen(TICKET_FILE, "rb");
    if (fp != NULL) {
        if (fread(&ticketCount, sizeof(int), 1, fp) == 1) {
            if (fread(tickets, sizeof(Ticket), ticketCount, fp) == ticketCount) {
                success++;
            } else {
                printf("Warning: Mismatch in ticket data count and actual records.\n");
                ticketCount = 0;
            }
        } else {
            ticketCount = 0;
        }
        fclose(fp);
    }
    return (success > 0) ? 0 : -1;
}
int saveData() {
    FILE *fp;
    int success_count = 0; 
    fp = fopen(FLIGHT_FILE, "wb");
    if (fp == NULL) {
        printf("Error: Could not open flight data file for writing.\n");
    } else {
        if (fwrite(&flightCount, sizeof(int), 1, fp) == 1) {
            if (fwrite(flights, sizeof(Flight), flightCount, fp) == flightCount) {
                 success_count++;
            } else {
                 printf("Warning: Failed to write all flight records.\n");
            }
        }
        fclose(fp);
    }
    fp = fopen(TICKET_FILE, "wb");
    if (fp == NULL) {
        printf("Error: Could not open ticket data file for writing.\n");
    } else {
        if (fwrite(&ticketCount, sizeof(int), 1, fp) == 1) {
            if (fwrite(tickets, sizeof(Ticket), ticketCount, fp) == ticketCount) {
                success_count++;
            } else {
                 printf("Warning: Failed to write all ticket records.\n");
            }
        }
        fclose(fp);
    }
    
    if (success_count == 2) {
        printf("Status: Flight and Ticket data saved successfully.\n");
        return 0;
    } else {
        printf("Status: Data save partially failed.\n");
        return -1;
    }
}
static int findFlightIndex(const char* flightId) {
    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].flightId, flightId) == 0) {
            return i;
        }
    }
    return -1;
}
void addFlight() {
    printf("\n--- Add New Flight ---\n");
    if (flightCount >= MAX_FLIGHTS) {
        printf("Error: System limit of %d flights reached.\n", MAX_FLIGHTS);
        return;
    }

    Flight newFlight;
    char tempId[10];
    do {
        printf("Enter Flight ID (e.g., AI701): ");
        scanf("%9s", tempId); 
        while (getchar() != '\n');
        if (findFlightIndex(tempId) != -1) {
            printf("Error: Flight ID already exists. Please enter a unique ID.\n");
        } else {
            strcpy(newFlight.flightId, tempId);
            break;
        }
    } while (1);

    printf("Enter Source: ");
    scanf("%49s", newFlight.source); 
    while (getchar() != '\n');
    
    printf("Enter Destination: ");
    scanf("%49s", newFlight.destination); 
    while (getchar() != '\n');
    do {
        newFlight.capacity = getIntegerInput("Enter Total Capacity (must be > 0): ");
        if (newFlight.capacity <= 0) {
            printf("Error: Capacity must be a positive number.\n");
        }
    } while (newFlight.capacity <= 0);
    newFlight.price = getPositiveFloatInput("Enter Price (must be > 0.00): ");
    newFlight.availableSeats = newFlight.capacity;
    flights[flightCount++] = newFlight;
    printf("\nSUCCESS: Flight %s from %s to %s added.\n", newFlight.flightId, newFlight.source, newFlight.destination);
}
void displayAllFlights() {
    printf("\n========================================================================\n");
    printf("                                ALL FLIGHTS\n");
    printf("========================================================================\n");
    printf("| %-8s | %-15s | %-15s | %-8s | %-8s | %-8s |\n", 
           "ID", "Source", "Destination", "Capacity", "Avail", "Price");
    printf("------------------------------------------------------------------------\n");

    if (flightCount == 0) {
        printf("| No flights currently available in the system.                        |\n");
    } else {
        for (int i = 0; i < flightCount; i++) {
            printf("| %-8s | %-15s | %-15s | %-8d | %-8d | %-8.2f |\n",
                   flights[i].flightId, 
                   flights[i].source, 
                   flights[i].destination, 
                   flights[i].capacity, 
                   flights[i].availableSeats,
                   flights[i].price);
        }
    }
    printf("========================================================================\n");
}
void searchFlights() {
    printf("\n--- Search Flights ---\n");
    char searchSource[MAX_STR_LEN];
    char searchDest[MAX_STR_LEN];
    int foundCount = 0;

    printf("Enter Source (or type 'any' to skip): ");
    scanf("%49s", searchSource); 
    while (getchar() != '\n');
    
    printf("Enter Destination (or type 'any' to skip): ");
    scanf("%49s", searchDest); 
    while (getchar() != '\n');

    printf("\n========================================================================\n");
    printf("                             SEARCH RESULTS\n");
    printf("========================================================================\n");
    printf("| %-8s | %-15s | %-15s | %-8s | %-8s | %-8s |\n", 
           "ID", "Source", "Destination", "Capacity", "Avail", "Price");
    printf("------------------------------------------------------------------------\n");

    for (int i = 0; i < flightCount; i++) {
        int sourceMatch = (strcmp(searchSource, "any") == 0 || strcmp(flights[i].source, searchSource) == 0);
        int destMatch = (strcmp(searchDest, "any") == 0 || strcmp(flights[i].destination, searchDest) == 0);
        
        if (sourceMatch && destMatch) {
            printf("| %-8s | %-15s | %-15s | %-8d | %-8d | %-8.2f |\n",
                   flights[i].flightId, 
                   flights[i].source, 
                   flights[i].destination, 
                   flights[i].capacity, 
                   flights[i].availableSeats,
                   flights[i].price);
            foundCount++;
        }
    }

    if (foundCount == 0) {
         printf("| No flights match your search criteria.                               |\n");
    }
    printf("========================================================================\n");
}
void bookTicket() {
    printf("\n--- Book New Ticket ---\n");

    if (ticketCount >= MAX_TICKETS) {
        printf("Error: System limit of %d tickets reached.\n", MAX_TICKETS);
        return;
    }
    
    char flightIdToBook[10];
    printf("Enter Flight ID to book: ");
    scanf("%9s", flightIdToBook); 
    while (getchar() != '\n');

    int flightIndex = findFlightIndex(flightIdToBook);

    if (flightIndex == -1) {
        printf("Error: Flight ID '%s' not found.\n", flightIdToBook);
        return;
    }

    if (flights[flightIndex].availableSeats <= 0) {
        printf("Error: Flight %s is fully booked.\n", flightIdToBook);
        return;
    }

    Ticket newTicket;
    newTicket.pnr = getNextPNR();
    strcpy(newTicket.flightId, flightIdToBook);

    printf("Enter Passenger Name (Max %d chars): ", MAX_STR_LEN - 1);
    char tempName[MAX_STR_LEN];
    if (fgets(tempName, sizeof(tempName), stdin) != NULL) {
        tempName[strcspn(tempName, "\n")] = 0; 
        strcpy(newTicket.passengerName, tempName);
    } else {
         strcpy(newTicket.passengerName, "N/A");
    }
    newTicket.seatNumber = flights[flightIndex].capacity - flights[flightIndex].availableSeats + 1;
    flights[flightIndex].availableSeats--;
    tickets[ticketCount++] = newTicket;

    printf("\n************************************************\n");
    printf("SUCCESS: Ticket Booked!\n");
    printf("PNR: %ld\n", newTicket.pnr);
    printf("Flight: %s (%s to %s)\n", newTicket.flightId, flights[flightIndex].source, flights[flightIndex].destination);
    printf("Passenger: %s\n", newTicket.passengerName);
    printf("Seat: %d\n", newTicket.seatNumber);
    printf("Price: %.2f\n", flights[flightIndex].price);
    printf("************************************************\n");
}
void cancelTicket() {
    printf("\n--- Cancel Ticket ---\n");
    long pnrToCancel = getIntegerInput("Enter PNR to cancel: ");
    int ticketIndex = -1;
    for (int i = 0; i < ticketCount; i++) {
        if (tickets[i].pnr == pnrToCancel) {
            ticketIndex = i;
            break;
        }
    }

    if (ticketIndex == -1) {
        printf("Error: PNR %ld not found in our records. Cancellation failed.\n", pnrToCancel);
        return;
    }
    int flightIndex = findFlightIndex(tickets[ticketIndex].flightId);
    if (flightIndex != -1) {
        flights[flightIndex].availableSeats++;
    }
    for (int i = ticketIndex; i < ticketCount - 1; i++) {
        tickets[i] = tickets[i+1];
    }
    ticketCount--;
    
    printf("\nSUCCESS: Ticket with PNR %ld has been successfully cancelled.\n", pnrToCancel);
    if (flightIndex != -1) {
         printf("Flight %s seat restored. Available seats: %d.\n", flights[flightIndex].flightId, flights[flightIndex].availableSeats);
    }
}
void displayBookingsByPNR() {
    printf("\n--- View Booking Details ---\n");
    long pnrToView = getIntegerInput("Enter PNR to view: ");
    for (int i = 0; i < ticketCount; i++) {
        if (tickets[i].pnr == pnrToView) {
            int flightIndex = findFlightIndex(tickets[i].flightId);
            
            printf("\n--- Booking Details for PNR %ld ---\n", pnrToView);
            printf("Passenger Name: %s\n", tickets[i].passengerName);
            printf("Flight ID: %s\n", tickets[i].flightId);
            printf("Seat Number: %d\n", tickets[i].seatNumber);
            
            if (flightIndex != -1) {
                printf("Route: %s -> %s\n", flights[flightIndex].source, flights[flightIndex].destination);
                printf("Ticket Price: %.2f\n", flights[flightIndex].price);
            } else {
                 printf("Warning: Associated flight data not found (flight might have been removed).\n");
            }
            printf("---------------------------------------\n");
            return;
        }
    }
    
    printf("Error: PNR %ld not found in the system.\n", pnrToView);
}
