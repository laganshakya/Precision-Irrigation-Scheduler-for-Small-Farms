#include <stdio.h>
#include <stdlib.h>

#define MAX_PLOTS 20

// Structure for farm plots
struct Plot {
    int id;
    char crop[30];
    float area;
    float moisture;
    float threshold;
    int lastIrrigation;
    float rainfall;
    float weeklyRainfall;
    float eto;
    float kc;
    float dailyWater;
    float weeklyWater;
    int priority;
};

// Linked list for irrigation events
struct Event {
    int plotId;
    float water;
    struct Event *next;
};

// Dynamic linked list for seasonal crops
struct Crop {
    char name[30];
    float coefficient;
    struct Crop *next;
};

// Crop coefficient function
float getCoefficient(char crop[]) {

    if (crop[0] == 'W' || crop[0] == 'w')
        return 1.15;

    if (crop[0] == 'R' || crop[0] == 'r')
        return 1.20;

    if (crop[0] == 'M' || crop[0] == 'm')
        return 1.05;

    if (crop[0] == 'C' || crop[0] == 'c')
        return 0.90;

    if (crop[0] == 'S' || crop[0] == 's')
        return 0.75;

    return 1.00;
}

// Function to calculate water requirement
float calculateWater(
    float eto,
    float kc,
    float rainfall,
    float moisture,
    float threshold,
    float area
) {
    float cropRequirement;
    float moistureDeficit;
    float irrigationDepth;
    float efficiency = 0.85;

    cropRequirement = eto * kc;

    moistureDeficit = (threshold - moisture) * 0.5;

    if (moistureDeficit < 0)
        moistureDeficit = 0;

    irrigationDepth = cropRequirement - rainfall;

    if (irrigationDepth < 0)
        irrigationDepth = 0;

    if (moistureDeficit > irrigationDepth)
        irrigationDepth = moistureDeficit;

    return (irrigationDepth * area * 10) / efficiency;
}

// Function to add an irrigation event
void addEvent(struct Event **head, int id, float water) {

    struct Event *newEvent;
    struct Event *temp;

    newEvent = (struct Event *)malloc(sizeof(struct Event));

    if (newEvent == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    newEvent->plotId = id;
    newEvent->water = water;
    newEvent->next = NULL;

    if (*head == NULL) {
        *head = newEvent;
    }
    else {
        temp = *head;

        while (temp->next != NULL)
            temp = temp->next;

        temp->next = newEvent;
    }
}

// Function to display irrigation events
void displayEvents(struct Event *head) {

    printf("\n===== IRRIGATION EVENT HISTORY =====\n");

    if (head == NULL) {
        printf("No irrigation events recorded.\n");
        return;
    }

    while (head != NULL) {
        printf("Plot %d : %.2f cubic metres\n",
               head->plotId, head->water);

        head = head->next;
    }
}

// Function to add seasonal crops dynamically
void addCrop(struct Crop **head, char name[], float kc) {

    struct Crop *newCrop;
    struct Crop *temp;

    newCrop = (struct Crop *)malloc(sizeof(struct Crop));

    if (newCrop == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    int i = 0;

    while (name[i] != '\0' && i < 29) {
        newCrop->name[i] = name[i];
        i++;
    }

    newCrop->name[i] = '\0';
    newCrop->coefficient = kc;
    newCrop->next = NULL;

    if (*head == NULL) {
        *head = newCrop;
    }
    else {
        temp = *head;

        while (temp->next != NULL)
            temp = temp->next;

        temp->next = newCrop;
    }
}

// Function to display seasonal crops
void displayCrops(struct Crop *head) {

    printf("\n===== SEASONAL CROP LIST =====\n");

    while (head != NULL) {

        printf("Crop: %s | Coefficient: %.2f\n",
               head->name, head->coefficient);

        head = head->next;
    }
}

// Function to sort plots by priority
void sortPriority(struct Plot plots[], int n) {

    int i, j;
    struct Plot temp;

    for (i = 0; i < n - 1; i++) {

        for (j = 0; j < n - i - 1; j++) {

            if (plots[j].priority < plots[j + 1].priority) {

                temp = plots[j];
                plots[j] = plots[j + 1];
                plots[j + 1] = temp;
            }
        }
    }
}

// Function to display daily and weekly plans
void generateSchedule(struct Plot plots[], int n, float cost) {

    int i;
    float totalDaily = 0;
    float totalWeekly = 0;

    printf("\n========== IRRIGATION SCHEDULE ==========\n");

    for (i = 0; i < n; i++) {

        plots[i].dailyWater = calculateWater(
            plots[i].eto,
            plots[i].kc,
            plots[i].rainfall,
            plots[i].moisture,
            plots[i].threshold,
            plots[i].area
        );

        plots[i].weeklyWater = calculateWater(
            plots[i].eto * 7,
            plots[i].kc,
            plots[i].weeklyRainfall,
            plots[i].moisture,
            plots[i].threshold,
            plots[i].area
        );

        totalDaily += plots[i].dailyWater;
        totalWeekly += plots[i].weeklyWater;

        printf("\nPlot ID: %d", plots[i].id);
        printf("\nCrop: %s", plots[i].crop);
        printf("\nDaily Water: %.2f m3", plots[i].dailyWater);
        printf("\nWeekly Water: %.2f m3", plots[i].weeklyWater);

        if (plots[i].moisture < plots[i].threshold)
            printf("\nALERT: Immediate irrigation attention required!");
        else
            printf("\nStatus: Moisture is above threshold.");

        printf("\n-----------------------------------\n");
    }

    printf("\nTOTAL DAILY WATER: %.2f m3", totalDaily);
    printf("\nTOTAL WEEKLY WATER: %.2f m3", totalWeekly);

    printf("\nDaily Estimated Cost: Rs. %.2f", totalDaily * cost);
    printf("\nWeekly Estimated Cost: Rs. %.2f", totalWeekly * cost);
}

// Function for water shortage simulation
void simulateWaterShortage(
    struct Plot plots[],
    int n,
    float availableWater,
    float cost,
    struct Event **events
) {

    int i;
    float usedWater = 0;
    float allocated;
    float remaining = availableWater;

    printf("\n========== WATER SHORTAGE SIMULATION ==========\n");

    sortPriority(plots, n);

    printf("\nPriority-wise allocation:\n");

    for (i = 0; i < n; i++) {

        printf("\nPlot %d | Priority: %d",
               plots[i].id, plots[i].priority);

        if (plots[i].dailyWater <= remaining) {

            allocated = plots[i].dailyWater;
        }
        else {
            allocated = remaining;
        }

        remaining -= allocated;
        usedWater += allocated;

        printf("\nRequired: %.2f m3", plots[i].dailyWater);
        printf("\nAllocated: %.2f m3", allocated);

        if (allocated >= plots[i].dailyWater) {
            printf("\nStatus: Fully irrigated");
        }
        else if (allocated > 0) {
            printf("\nStatus: Partially irrigated");
        }
        else {
            printf("\nStatus: Water unavailable!");
        }

        if (allocated > 0)
            addEvent(events, plots[i].id, allocated);

        printf("\n");
    }

    printf("\n========== SIMULATION REPORT ==========\n");

    printf("Available Water: %.2f m3\n", availableWater);
    printf("Used Water: %.2f m3\n", usedWater);
    printf("Remaining Water: %.2f m3\n", remaining);
    printf("Estimated Cost: Rs. %.2f\n", usedWater * cost);
}

// Function to free event linked list memory
void freeEvents(struct Event *head) {

    struct Event *temp;

    while (head != NULL) {

        temp = head;
        head = head->next;

        free(temp);
    }
}

// Function to free crop linked list memory
void freeCrops(struct Crop *head) {

    struct Crop *temp;

    while (head != NULL) {

        temp = head;
        head = head->next;

        free(temp);
    }
}

// MAIN FUNCTION
int main() {

    struct Plot plots[MAX_PLOTS];

    struct Event *events = NULL;
    struct Crop *crops = NULL;

    int n, i, choice;
    float cost;
    float availableWater;

    printf("========================================\n");
    printf("  PRECISION IRRIGATION SCHEDULER\n");
    printf("       FOR SMALL FARMS\n");
    printf("========================================\n");

    printf("\nEnter number of farm plots (Max %d): ", MAX_PLOTS);
    scanf("%d", &n);

    if (n < 1 || n > MAX_PLOTS) {
        printf("Invalid number of plots!\n");
        return 0;
    }

    printf("Enter water cost per cubic metre (Rs.): ");
    scanf("%f", &cost);

    // Taking plot information
    for (i = 0; i < n; i++) {

        printf("\n========== PLOT %d ==========\n", i + 1);

        plots[i].id = i + 1;

        printf("Enter crop name (Wheat/Rice/Maize/Cotton/Sugarcane): ");
        scanf("%29s", plots[i].crop);

        plots[i].kc = getCoefficient(plots[i].crop);

        printf("Enter plot area (hectares): ");
        scanf("%f", &plots[i].area);

        printf("Enter current soil moisture (%%): ");
        scanf("%f", &plots[i].moisture);

        printf("Enter moisture threshold (%%): ");
        scanf("%f", &plots[i].threshold);

        printf("Days since last irrigation: ");
        scanf("%d", &plots[i].lastIrrigation);

        printf("Enter daily reference ET (mm): ");
        scanf("%f", &plots[i].eto);

        printf("Enter today's rainfall (mm): ");
        scanf("%f", &plots[i].rainfall);

        printf("Enter expected weekly rainfall (mm): ");
        scanf("%f", &plots[i].weeklyRainfall);

        // Priority calculation
        plots[i].priority = 0;

        if (plots[i].moisture < plots[i].threshold)
            plots[i].priority += 10;

        if (plots[i].moisture < plots[i].threshold - 10)
            plots[i].priority += 10;

        if (plots[i].lastIrrigation >= 3)
            plots[i].priority += 5;

        // Add crop to dynamic list
        addCrop(&crops, plots[i].crop, plots[i].kc);
    }

    // Main menu
    do {

        printf("\n\n========== MAIN MENU ==========\n");
        printf("1. Generate Daily and Weekly Schedule\n");
        printf("2. Display Seasonal Crop List\n");
        printf("3. Simulate Limited Water Availability\n");
        printf("4. Display Irrigation Event History\n");
        printf("5. Exit\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                generateSchedule(plots, n, cost);
                break;

            case 2:
                displayCrops(crops);
                break;

            case 3:

                // Generate requirements before simulation
                for (i = 0; i < n; i++) {

                    plots[i].dailyWater = calculateWater(
                        plots[i].eto,
                        plots[i].kc,
                        plots[i].rainfall,
                        plots[i].moisture,
                        plots[i].threshold,
                        plots[i].area
                    );
                }

                printf("\nEnter available water (m3): ");
                scanf("%f", &availableWater);

                simulateWaterShortage(
                    plots,
                    n,
                    availableWater,
                    cost,
                    &events
                );

                break;

            case 4:
                displayEvents(events);
                break;

            case 5:
                printf("\nThank you for using the Irrigation Scheduler!\n");
                break;

            default:
                printf("\nInvalid choice! Try again.\n");
        }

    } while (choice != 5);

    // Release dynamically allocated memory
    freeEvents(events);
    freeCrops(crops);

    return 0;
}
