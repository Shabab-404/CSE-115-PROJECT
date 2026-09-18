#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Cross-platform fix so it compiles on Windows without errors
#ifdef _WIN32
    #define strcasecmp _stricmp
#else
    #include <strings.h>
#endif

#define MAX_ITEMS 6
#define MAX_NAME_LEN 50
#define MAX_USERS 10
#define USERNAME_LEN 50
#define PASSWORD_LEN 50
#define MAX_INPUT_LEN 100

typedef struct {
    char name[MAX_NAME_LEN];
    int found;
} Item;

typedef struct {
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    Item items[MAX_ITEMS];
    int progress; 
} User;

User users[MAX_USERS];
int userCount = 0;
int loggedInUser = -1;

void displayHelp();
void intro();
void exploreHallway(Item items[]);
void exploreRoom(const char* roomName, const char* riddle, const char* answer, Item* item, const char* itemName);
int finalExam(Item items[]);
int allItemsFound(Item items[]);
void timePedestals(Item items[]);
void loadUsers();
void saveUsers();
int registerUser();
int loginUser();
void deleteUser();
void showFoundItems(Item items[]);
void quitGame();
void resetItems(Item items[]);

void quitGame() {
    saveUsers();
    printf("Progress saved. Goodbye!\n");
    exit(0);
}

void resetItems(Item items[]) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        items[i].found = 0;
        items[i].name[0] = '\0';
    }
}

void loadUsers() {
    FILE *file = fopen("users.dat", "rb");
    if (file) {
        fread(&userCount, sizeof(int), 1, file);
        fread(users, sizeof(User), userCount, file);
        fclose(file);
    }
}

void saveUsers() {
    FILE *file = fopen("users.dat", "wb");
    if (file) {
        fwrite(&userCount, sizeof(int), 1, file);
        fwrite(users, sizeof(User), userCount, file);
        fclose(file);
    }
}

int registerUser() {
    if (userCount >= MAX_USERS) {
        printf("Maximum number of users reached.\n");
        return 0;
    }
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    printf("Enter new username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("Username already exists.\n");
            return 0;
        }
    }
    printf("Enter new password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;
    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    resetItems(users[userCount].items);
    users[userCount].progress = 0;
    userCount++;
    saveUsers();
    printf("Registration successful!\n");
    return 1;
}

int loginUser() {
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;
    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            loggedInUser = i;
            printf("Login successful! Welcome, %s.\n", users[i].username);
            return 1;
        }
    }
    printf("Invalid username or password.\n");
    return 0;
}

void deleteUser() {
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    printf("Enter username to delete: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;
    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            for (int j = i; j < userCount - 1; j++) {
                users[j] = users[j + 1];
            }
            userCount--;
            saveUsers();
            printf("Account deleted successfully.\n");
            return;
        }
    }
    printf("Account not found or password incorrect.\n");
}

void displayHelp() {
    printf("\n How to Play\n");
    printf("===============================\n");
    printf("- Explore the abandoned NSU campus room by room.\n");
    printf("- Solve puzzles and riddles to find 6 lost items.\n");
    printf("- Beware of ghostly dangers in some areas.\n");
    printf("- Use clues from objects and whispers to progress.\n");
    printf("- Answer wisely - some rooms can trap you!\n");
    printf("\n Controls\n");
    printf("===============================\n");
    printf("- Type your answers and press Enter to solve puzzles.\n");
    printf("- Use numbers (1, 2, 3) in the main menu to choose options.\n");
    printf("- Type 'quit' at any input prompt to save and exit the game.\n");
    printf("\n Objective\n");
    printf("===============================\n");
    printf("Find all 6 lost items and survive the Final Exam...\n");
    printf("...or be trapped in the Forgotten Campus forever.\n");
    printf("\nPress Enter to return to the main menu...");
    getchar();
}

void intro() {
    printf("Welcome to NSU 0.1 - The Forgotten Campus\n\n");
    printf("You are a freshman at NSU. A rumor says there's an old, hidden campus.\n");
    printf("One night, you find a hollow-sounding wall in the basement...\n");
    printf("A hidden door opens. You step inside...\n");
    printf("\n> NSU 0.1 - Enter at your own risk\n\n");
    printf("Press Enter to continue...\n");
    getchar();
}

void showFoundItems(Item items[]) {
    printf("\nItems Found So Far:\n");
    int count = 0;
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (items[i].found) {
            printf("- %s\n", items[i].name);
            count++;
        }
    }
    if (count == 0) printf("- None yet.\n");
}

void exploreRoom(const char* roomName, const char* riddle, const char* answer, Item* item, const char* itemName) {
    if (item->found) return; // Skip if already found
    
    char input[MAX_INPUT_LEN];
    printf("\nYou enter the %s...\n", roomName);
    printf("A voice asks: \"%s\"\n", riddle);
    printf("Your answer (or type 'quit' to save/exit): ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    
    if (strcasecmp(input, "quit") == 0) {
        quitGame();
    }
    if (strcasecmp(input, answer) == 0) {
        printf("Correct!\n");
        item->found = 1;
        strcpy(item->name, itemName);
    } else {
        printf("Wrong. The room grows colder...\n");
    }
}

void exploreHallway(Item items[]) {
    exploreRoom("Plaza", "Cold and quiet, no voices loud,\nWhere books live and heads are bowed.", "Central Library", &items[0], "Old Yearbook");
    exploreRoom("Room 603", "Smells great at first bite,\nThen bugs join the invite.", "Kashundi", &items[1], "Cafeteria Key");
    exploreRoom("Archives", "Full of green, chill and light,\nCards and guitars feel just right.", "Gallery", &items[2], "Mysterious ID Card");
    exploreRoom("Round Stairs", "High above with cozy views,\nA perfect spot for lovebirds to choose.", "SAC7", &items[3], "Lost Lecture Notes");
    exploreRoom("Field", "It stinks, its loud, not the best,\nBut still, its where we all rest.", "Lounge", &items[4], "Faculty Pen");
    exploreRoom("Cafe Booth", "It begins with fun, a moment of fame,\nBut soon enough, it's controversy's game.", "Flashmob", &items[5], "Final Exam Paper");
    showFoundItems(items);
}

int allItemsFound(Item items[]) {
    int count = 0;
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (items[i].found) count++;
    }
    return count;
}

void timePedestals(Item items[]) {
    int has_yearbook = 0, has_id = 0, has_exam = 0;
    int used[MAX_ITEMS] = {0};

    for (int i = 0; i < MAX_ITEMS; i++) {
        if (items[i].found) {
            if (strcmp(items[i].name, "Old Yearbook") == 0) has_yearbook = 1;
            if (strcmp(items[i].name, "Mysterious ID Card") == 0) has_id = 1;
            if (strcmp(items[i].name, "Final Exam Paper") == 0) has_exam = 1;
        }
    }
    if (!(has_yearbook && has_id && has_exam)) {
        printf("\nYou need these items to proceed:\n");
        printf("- Old Yearbook\n- Mysterious ID Card\n- Final Exam Paper\n");
        printf("Return to exploration!\n");
        users[loggedInUser].progress = 1; // Send them back to hallway
        return;
    }

    printf("\n=== Time Pedestals ===\n");
    printf("Place items on three pedestals:\n");
    printf("1. Past\n2. Present\n3. Future\n");

    int choices[3][2];

    for (int p = 0; p < 3; p++) {
        printf("\nItems for %s pedestal:\n", (p == 0) ? "Past" : (p == 1) ? "Present" : "Future");
        int available_indices[MAX_ITEMS];
        int available_count = 0;

        for (int i = 0; i < MAX_ITEMS; i++) {
            if (items[i].found && !used[i]) {
                printf("%d. %s\n", available_count + 1, items[i].name);
                available_indices[available_count++] = i;
            }
        }

        printf("Choose two items (e.g., 1 2) or type 'quit': ");
        char input[MAX_INPUT_LEN];
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        
        if (strcasecmp(input, "quit") == 0) quitGame();
        
        int a, b;
        if (sscanf(input, "%d %d", &a, &b) != 2 || a < 1 || b < 1 || a > available_count || b > available_count || a == b) {
            printf("Invalid selection.\n");
            p--;
            continue;
        }
        choices[p][0] = available_indices[a - 1];
        choices[p][1] = available_indices[b - 1];
        used[choices[p][0]] = 1;
        used[choices[p][1]] = 1;
    }

    has_yearbook = has_id = has_exam = 0;
    for (int i = 0; i < 2; i++) {
        if (strcmp(items[choices[0][i]].name, "Old Yearbook") == 0) has_yearbook = 1;
        if (strcmp(items[choices[1][i]].name, "Mysterious ID Card") == 0) has_id = 1;
        if (strcmp(items[choices[2][i]].name, "Final Exam Paper") == 0) has_exam = 1;
    }

    if (has_yearbook && has_id && has_exam) {
        printf("\nPedestals glow! Path appears.\n");
        users[loggedInUser].progress = 3;
    } else {
        printf("\nNothing happens. Try again.\n");
    }
}

int finalExam(Item items[]) {
    if (allItemsFound(items) == MAX_ITEMS) {
        printf("\nYou arrive at the Final Exam Room...\n");
        printf("All items glow with eerie light, guiding you.\n");
        printf("You face the last question: What binds knowledge and memory?\n");
        printf("Your answer (or type 'quit' to save/exit): ");
        char input[MAX_INPUT_LEN];
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        
        if (strcasecmp(input, "quit") == 0) quitGame();
        
        if (strcasecmp(input, "present") == 0) {
            return 1;
        } else {
            printf("The room collapses into darkness...\n");
            return 0;
        }
    } else {
        printf("\nYou need all 6 items before attempting the Final Exam!\n");
        users[loggedInUser].progress = 1; // Send back to exploration
        return 0;
    }
}

// Helper to safely get integer input without infinite loops
int getSafeInt() {
    int choice;
    while (scanf("%d", &choice) != 1) {
        while (getchar() != '\n'); // clear buffer
        printf("Invalid input. Please enter a number: ");
    }
    getchar(); // clear trailing newline
    return choice;
}

int main() {
    loadUsers();
    int choice;
    while (1) {
        printf("\n=== Lost In Academia ===\n");
        printf("1. Login\n");
        printf("2. Register\n");
        printf("3. Delete Account\n");
        printf("4. Exit\n");
        printf("Choose an option: ");
        choice = getSafeInt();
        
        if (choice == 1) {
            if (loginUser()) break;
        } else if (choice == 2) {
            registerUser();
        } else if (choice == 3) {
            deleteUser();
        } else if (choice == 4) {
            saveUsers();
            printf("Goodbye!\n");
            exit(0);
        } else {
            printf("Invalid choice.\n");
        }
    }

    Item *items = users[loggedInUser].items;
    while (1) {
        printf("\n=== Lost In Academia - Adventure Menu ===\n");
        printf("1. Explore Campus / Continue Game\n");
        printf("2. Restart from Beginning\n");
        printf("3. How to Play\n");
        printf("4. Save and Exit\n");
        printf("Choose an option: ");
        choice = getSafeInt();

        if (choice == 1) {
            printf("Continuing your game...\n");
            if (users[loggedInUser].progress == 0) intro();
            
            // Revisit logic so you don't get soft-locked
            if (users[loggedInUser].progress <= 1) {
                users[loggedInUser].progress = 1;
                exploreHallway(items);
                if (allItemsFound(items) >= 3) {
                    users[loggedInUser].progress = 2;
                }
            }
            
            if (users[loggedInUser].progress == 2) {
                timePedestals(items);
            }
            
            if (users[loggedInUser].progress == 3) {
                if (finalExam(items)) {
                    printf("\nYou have passed the Final Exam. You wake up in the parking lot - was it all a dream?\n");
                    users[loggedInUser].progress = 4;
                }
            }
            
            if (users[loggedInUser].progress == 4) {
                printf("You already completed the game!\n");
            }
            
        } else if (choice == 2) {
            users[loggedInUser].progress = 0; 
            resetItems(items);
            printf("Progress reset. Select 'Explore Campus' to start over.\n");
        } else if (choice == 3) {
            displayHelp();
        } else if (choice == 4) {
            saveUsers();
            printf("Progress Saved. Goodbye!\n");
            exit(0);
        } else {
            printf("Invalid choice.\n");
        }
    }
    return 0;
}