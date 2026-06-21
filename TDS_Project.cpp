#include <iostream>
#include <fstream>
#include <cstring>  
#include <cstdlib>   

using namespace std;

// CUSTOM UTILITY FUNCTIONS 
int safeReadInt(const char* prompt) {
    int value;
    while (true) {
        if (prompt != NULL) cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear();                 
            cin.ignore(10000, '\n');     
            cout << "Invalid input. Please enter a whole number.\n";
        } else {
            cin.ignore(10000, '\n');     
            return value;
        }
    }
}

double safeReadDouble(const char* prompt) {
    double value;
    while (true) {
        if (prompt != NULL) cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number.\n";
        } else {
            cin.ignore(10000, '\n');
            return value;
        }
    }
}

int customStrLen(const char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

void customStrCpy(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

bool customStrCmp(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) return false;
        i++;
    }
    return (str1[i] == str2[i]);
}

bool customStrSubStr(const char* haystack, const char* needle) {
    if (*needle == '\0') return true;
    for (int i = 0; haystack[i] != '\0'; i++) {
        int j = 0;
        while (haystack[i + j] != '\0' && needle[j] != '\0' && 
               (haystack[i + j] == needle[j] || 
                (haystack[i + j] >= 'A' && haystack[i + j] <= 'Z' && haystack[i + j] + 32 == needle[j]) ||
                (haystack[i + j] >= 'a' && haystack[i + j] <= 'z' && haystack[i + j] - 32 == needle[j]))) {
            j++;
        }
        if (needle[j] == '\0') return true;
    }
    return false;
}

// REQUIRED STRUCT DATA TYPES
struct Movie {
    int id;
    char title[100];
    char genre[50];
    double rating;
    int year;
    Movie* prev;
    Movie* next;
};

struct WatchlistNode {
    char username[50];
    int movieId;
    char status[30];
    WatchlistNode* next;
};


struct Review {
    char username[50];      
    int  movieId;           
    int  score;             
    char comment[150];      
    Review* next;           
};

class AdminModule;
class CustomerModule;
void printMovieSpecs(const Movie& m);
void printWatchlistSpecs(const WatchlistNode& w);
bool verifySystemIntegrity(const AdminModule& am);
bool verifyCustomerIntegrity(const CustomerModule& cm);
void showAdminStats(const AdminModule& am);       
void showCustomerRewards(const CustomerModule& cm); 
void watchMovieFlow(Movie* head, const char* username);  

WatchlistNode* watchlistHead = NULL;

Review* reviewHead = NULL;

struct FavoriteNode {
    char username[50];
    int  movieId;
    FavoriteNode* next;
};
FavoriteNode* favoriteHead = NULL;

struct RecentNode {
    char username[50];
    int  movieId;
    RecentNode* next;
};
RecentNode* recentHead = NULL;

// WATCHLIST FUNCTIONS
void addToWatchlist(const char* username, int movieId, const char* status) {
    WatchlistNode* newNode = new WatchlistNode();
    customStrCpy(newNode->username, username);
    newNode->movieId = movieId;
    customStrCpy(newNode->status, status);
    newNode->next = NULL;
    
    if (watchlistHead == NULL) {
        watchlistHead = newNode;
    } else {
        WatchlistNode* temp = watchlistHead;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

void displayWatchlist(const char* username) {
    WatchlistNode* temp = watchlistHead;
    bool found = false;
    cout << "\n--- WATCHLIST FOR " << username << " ---\n";
    while (temp != NULL) {
        if (customStrCmp(temp->username, username)) {
            cout << "Movie ID: " << temp->movieId << " | Status: " << temp->status << endl;
            found = true;
        }
        temp = temp->next;
    }
    if (!found) {
        cout << "No movies in watchlist.\n";
    }
}

bool editWatchlistStatus(const char* username, int movieId, const char* newStatus) {
    WatchlistNode* temp = watchlistHead;
    while (temp != NULL) {
        if (customStrCmp(temp->username, username) && temp->movieId == movieId) {
            customStrCpy(temp->status, newStatus);
            return true;
        }
        temp = temp->next;
    }
    return false;
}

bool deleteFromWatchlist(const char* username, int movieId) {
    WatchlistNode* temp = watchlistHead;
    WatchlistNode* prev = NULL;
    
    while (temp != NULL) {
        if (customStrCmp(temp->username, username) && temp->movieId == movieId) {
            if (prev == NULL) {
                watchlistHead = temp->next;
            } else {
                prev->next = temp->next;
            }
            delete temp;
            return true;
        }
        prev = temp;
        temp = temp->next;
    }
    return false;
}

void saveWatchlistToFile() {
    ofstream file("watchlist_data.txt");
    if (file.is_open()) {
        WatchlistNode* temp = watchlistHead;
        while (temp != NULL) {
            file << temp->username << "," << temp->movieId << "," << temp->status << "\n";
            temp = temp->next;
        }
        file.close();
    }
}

void loadWatchlistFromFile() {
    ifstream file("watchlist_data.txt");
    if (file.is_open()) {
        char line[200];
        while (file.getline(line, 200)) {
            if (line[0] == '\0') continue;        
            char username[50] = "";               
            char status[30]  = "";
            int  movieId     = -1;
            bool gotUser = false, gotId = false, gotStatus = false;

            char* token = strtok(line, ",");
            if (token != NULL) { customStrCpy(username, token); gotUser = true; }
            token = strtok(NULL, ",");
            if (token != NULL) { movieId = atoi(token); gotId = true; }
            token = strtok(NULL, ",");
            if (token != NULL) { customStrCpy(status, token); gotStatus = true; }

            if (gotUser && gotId && gotStatus) {
                addToWatchlist(username, movieId, status);
            }
        }
        file.close();
    }
}

// REQUIRED BASE CLASSES
class Account {
protected:
    char loggedUser[50];
    bool isLoggedIn;
public:
    Account() {
        loggedUser[0] = '\0';
        isLoggedIn = false;
    }
    virtual ~Account() {}
    
    virtual void displayMenu() = 0;
    
    bool getLoginStatus() const { return isLoggedIn; }
    const char* getLoggedUser() const { return loggedUser; }
};

class DBHandler {
protected:
    char systemLogFile[50];
public:
    DBHandler() {
        customStrCpy(systemLogFile, "system_logs.txt");
    }
    virtual ~DBHandler() {}
    virtual bool testConnection() = 0;
};


// REQUIRED DERIVED CLASSES
class AdminModule : public Account {
private:
    int totalAdminActions;
public:
    AdminModule() : Account() {
        totalAdminActions = 0;
    }
    ~AdminModule() override {}

    void displayMenu() override {
        cout << "\n--- STAFF / ADMIN INTERFACE ---" << endl;
        cout << "1. Add New Movie Record" << endl;
        cout << "2. Edit Movie Record Details" << endl;
        cout << "3. Delete Movie Record" << endl;
        cout << "4. Sort Movies List" << endl;
        cout << "5. Search Movie Entry" << endl;
        cout << "6. View Master Movie List" << endl;
        cout << "7. Generate Movie Summary Report (.txt)" << endl;
        cout << "9. Movie Statistics Report (.txt)" << endl;
        cout << "10. View Login History" << endl;
        cout << "11. Filter Search (year / rating / genre)" << endl;
        cout << "12. Extra Sort (Insertion by Title / Merge by Year)" << endl;
        cout << "13. Hash Table Lookup (by ID)" << endl;
        cout << "14. Moderate Reviews (view / delete)" << endl;
        cout << "15. Analytics Menu (decade / top-N / export)" << endl;
        cout << "16. About / Help" << endl;
        cout << "17. Manage Cast Info (director / actor)" << endl;
        cout << "18. Batch Import Movies (from txt)" << endl;
        cout << "19. Export Reviews Report (.txt)" << endl;
        cout << "20. System Summary" << endl;
        cout << "8. Logout" << endl;
    }

    bool adminLogin(const char* user, const char* pass) {
        if (customStrCmp(user, "admin") && customStrCmp(pass, "admin123")) {
            customStrCpy(loggedUser, user);
            isLoggedIn = true;
            return true;
        }
        try {
            ifstream loginFile("credentials.txt");
            if (!loginFile) {
                throw "Warning: credentials file missing!";
            }
            char u[50], p[50], role[20];
            while (loginFile >> u >> p >> role) {
                if (customStrCmp(user, u) && customStrCmp(pass, p) && customStrCmp(role, "admin")) {
                    customStrCpy(loggedUser, user);
                    isLoggedIn = true;
                    loginFile.close();
                    return true;
                }
            }
            loginFile.close();
        }
        catch (const char* msg) {
            cout << "[EXCEPTION TRIGGERED] " << msg << endl;
        }
        return false;
    }
    
    bool adminRegister(const char* user, const char* pass) {
        try {
            ofstream file("credentials.txt", ios::app);
            if (!file) {
                throw "Unable to open credentials file for registration!";
            }
            file << user << " " << pass << " admin\n";
            file.close();
            return true;
        }
        catch (const char* msg) {
            cout << "[EXCEPTION] " << msg << endl;
            return false;
        }
    }

    void logout() {
        isLoggedIn = false;
        loggedUser[0] = '\0';
    }

    friend bool verifySystemIntegrity(const AdminModule& am);
    friend void showAdminStats(const AdminModule& am);
};

class CustomerModule : public Account {
private:
    int rewardsPoints;
public:
    CustomerModule() : Account() {
        rewardsPoints = 100;
    }
    ~CustomerModule() override {}

    void displayMenu() override {
        cout << "\n--- CUSTOMER MOVIE PORTAL ---" << endl;
        cout << "1. Browse All Movies" << endl;
        cout << "2. Search Specific Movie Title / Genre" << endl;
        cout << "3. Sort Movies Collection" << endl;
        cout << "4. Add Movie to Personal Watchlist" << endl;
        cout << "5. Edit Watchlist Status" << endl;
        cout << "6. Delete From Watchlist" << endl;
        cout << "7. View My Watchlist" << endl;
        cout << "8. Export & View Watchlist History Report (.txt)" << endl;
        cout << "10. Write a Movie Review" << endl;
        cout << "11. View My Reviews" << endl;
        cout << "12. View Reviews for a Movie" << endl;
        cout << "13. Get Recommendations" << endl;
        cout << "14. Edit My Review" << endl;
        cout << "15. Delete My Review" << endl;
        cout << "16. My Favourites" << endl;
        cout << "17. Recently Viewed" << endl;
        cout << "18. My Loyalty / Membership" << endl;
        cout << "19. Cast Explorer (actor / director)" << endl;
        cout << "20. My Dashboard" << endl;
        cout << "9. Logout" << endl;
    }

    bool customerLogin(const char* user, const char* pass) {
        try {
            ifstream loginFile("credentials.txt");
            if (!loginFile) {
                throw "Warning: System validation records profile matching file missing!";
            }
            char u[50], p[50], role[20];
            while (loginFile >> u >> p >> role) {
                if (customStrCmp(user, u) && customStrCmp(pass, p) && customStrCmp(role, "customer")) {
                    customStrCpy(loggedUser, user);
                    isLoggedIn = true;
                    loginFile.close();
                    return true;
                }
            }
            loginFile.close();
        }
        catch (const char* msg) {
            cout << "[EXCEPTION TRIGGERED] " << msg << endl;
        }
        return false;
    }
    
    bool customerRegister(const char* user, const char* pass) {
        try {
            ofstream file("credentials.txt", ios::app);
            if (!file) {
                throw "Unable to open credentials file for registration!";
            }
            file << user << " " << pass << " customer\n";
            file.close();
            return true;
        }
        catch (const char* msg) {
            cout << "[EXCEPTION] " << msg << endl;
            return false;
        }
    }

    void logout() {
        isLoggedIn = false;
        loggedUser[0] = '\0';
    }

    friend bool verifyCustomerIntegrity(const CustomerModule& cm);
    friend void showCustomerRewards(const CustomerModule& cm);
};

class SystemLogger : public DBHandler {
private:
    int logWriteCount;
public:
    SystemLogger() : DBHandler() {
        logWriteCount = 0;
    }
    ~SystemLogger() override {}

    bool testConnection() override {
        ofstream testFile(systemLogFile, ios::app);
        if (testFile.is_open()) {
            testFile.close();
            return true;
        }
        return false;
    }

    void logEvent(const char* eventMsg) {
        ofstream logFile(systemLogFile, ios::app);
        if (logFile.is_open()) {
            logFile << "[LOG EVENT]: " << eventMsg << "\n";
            logFile.close();
            logWriteCount++;
        }
    }
};

// REQUIRED FRIEND FUNCTIONS
void printMovieSpecs(const Movie& m) {
    cout << "ID: " << m.id << " | Title: " << m.title 
         << " | Genre: " << m.genre << " | Rating: " << m.rating 
         << " | Year: " << m.year << endl;
}

void printWatchlistSpecs(const WatchlistNode& w) {
    cout << "User: " << w.username << " | Movie ID: " << w.movieId 
         << " | Plan Status: " << w.status << endl;
}

bool verifySystemIntegrity(const AdminModule& am) {
    return am.totalAdminActions >= 0;
}

bool verifyCustomerIntegrity(const CustomerModule& cm) {
    return cm.rewardsPoints > 0;
}

void showAdminStats(const AdminModule& am) {
    cout << "[ADMIN STATS] Total admin actions performed: "
         << am.totalAdminActions << endl;
}

void showCustomerRewards(const CustomerModule& cm) {
    cout << "[CUSTOMER REWARDS] Current reward points: "
         << cm.rewardsPoints << endl;
}

// OVERLOADED / OVERRIDDEN FUNCTIONS
Movie* searchMovie(Movie* head, int targetId) {
    Movie* current = head;
    while (current != NULL) {
        if (current->id == targetId) return current;
        current = current->next;
    }
    return NULL;
}

Movie* searchMovie(Movie* head, const char* targetTitle) {
    Movie* current = head;
    while (current != NULL) {
        if (customStrSubStr(current->title, targetTitle)) return current;
        current = current->next;
    }
    return NULL;
}

// MANUALLY IMPLEMENTED SEARCHING ALGORITHM (Binary Search)
Movie* binarySearchMovie(Movie* head, int targetId) {
    int n = 0;
    Movie* walk = head;
    while (walk != NULL) {
        n++;
        walk = walk->next;
    }
    if (n == 0) return NULL;
    Movie** arr = new Movie*[n];   
    walk = head;
    for (int i = 0; i < n; i++) {
        arr[i] = walk;
        walk = walk->next;
    }
    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j]->id < arr[minIndex]->id) {
                minIndex = j;
            }
        }
        if (minIndex != i) {            
            Movie* temp = arr[i];
            arr[i] = arr[minIndex];
            arr[minIndex] = temp;
        }
    }

    int first = 0;
    int last = n - 1;
    Movie* result = NULL;
    while (first <= last) {
        int middle = (first + last) / 2;
        if (arr[middle]->id == targetId) {
            result = arr[middle];       
            break;
        } else if (arr[middle]->id < targetId) {
            first = middle + 1;         
        } else {
            last = middle - 1;          
        }
    }

    delete[] arr;  
    return result;  
}

// DOUBLY LINKED LIST ENGINE & UTILITIES
void appendMovie(Movie*& head, int id, const char* title, const char* genre, double rating, int year) {
    Movie* newNode = new Movie();
    newNode->id = id;
    customStrCpy(newNode->title, title);
    customStrCpy(newNode->genre, genre);
    newNode->rating = rating;
    newNode->year = year;
    newNode->next = NULL;
    newNode->prev = NULL;

    if (head == NULL) {
        head = newNode;
        return;
    }
    Movie* temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
    newNode->prev = temp;
}

void clearMoviesList(Movie*& head) {
    Movie* current = head;
    while (current != NULL) {
        Movie* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    head = NULL;
}

int getMovieCount(Movie* head) {
    int count = 0;
    Movie* temp = head;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

// MANUALLY IMPLEMENTED SORTING ALGORITHM (Bubble Sort)
void bubbleSortMovies(Movie* head, int criteriaOption) {
    if (head == NULL || head->next == NULL) return;

    bool swapped;
    Movie* ptr1;
    Movie* lptr = NULL;

    do {
        swapped = false;
        ptr1 = head;

        while (ptr1->next != lptr) {
            bool condition = false;
            if (criteriaOption == 1) {
                condition = (ptr1->rating < ptr1->next->rating);
            } else {
                condition = (ptr1->year > ptr1->next->year);
            }

            if (condition) {
                int tempId = ptr1->id;
                char tempTitle[100];
                char tempGenre[50];
                double tempRating = ptr1->rating;
                int tempYear = ptr1->year;

                customStrCpy(tempTitle, ptr1->title);
                customStrCpy(tempGenre, ptr1->genre);

                ptr1->id = ptr1->next->id;
                customStrCpy(ptr1->title, ptr1->next->title);
                customStrCpy(ptr1->genre, ptr1->next->genre);
                ptr1->rating = ptr1->next->rating;
                ptr1->year = ptr1->next->year;

                ptr1->next->id = tempId;
                customStrCpy(ptr1->next->title, tempTitle);
                customStrCpy(ptr1->next->genre, tempGenre);
                ptr1->next->rating = tempRating;
                ptr1->next->year = tempYear;

                swapped = true;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

// MOVIE DATABASE FUNCTIONS
void saveMoviesToFile(Movie* head) {
    ofstream file("movies_db.txt");
    if (file.is_open()) {
        Movie* temp = head;
        while (temp != NULL) {
            file << temp->id << "," << temp->title << "," << temp->genre << "," << temp->rating << "," << temp->year << "\n";
            temp = temp->next;
        }
        file.close();
    }
}

void seedDefaultMovies(Movie*& head) {
    appendMovie(head, 101, "The Dark Knight", "Action", 9.0, 2008);
    appendMovie(head, 102, "Inception", "Sci-Fi", 8.8, 2010);
    appendMovie(head, 103, "Interstellar", "Sci-Fi", 8.6, 2014);
    appendMovie(head, 104, "Pulp Fiction", "Crime", 8.9, 1994);
    appendMovie(head, 105, "Spirited Away", "Animation", 8.6, 2001);
    appendMovie(head, 106, "The Godfather", "Crime", 9.2, 1972);
    appendMovie(head, 107, "The Shawshank Redemption", "Drama", 9.3, 1994);
    appendMovie(head, 108, "The Dark Knight Rises", "Action", 8.4, 2012);
    appendMovie(head, 109, "Forrest Gump", "Drama", 8.8, 1994);
    appendMovie(head, 110, "The Matrix", "Sci-Fi", 8.7, 1999);
    appendMovie(head, 111, "Goodfellas", "Crime", 8.7, 1990);
    appendMovie(head, 112, "Fight Club", "Drama", 8.8, 1999);
    appendMovie(head, 113, "The Lord of the Rings", "Fantasy", 8.9, 2001);
    appendMovie(head, 114, "Gladiator", "Action", 8.5, 2000);
    appendMovie(head, 115, "The Lion King", "Animation", 8.5, 1994);
    appendMovie(head, 116, "The Avengers", "Action", 8.0, 2012);
    appendMovie(head, 117, "Titanic", "Drama", 7.9, 1997);
    appendMovie(head, 118, "Avatar", "Sci-Fi", 7.8, 2009);
    appendMovie(head, 119, "The Incredibles", "Animation", 8.0, 2004);
    appendMovie(head, 120, "Finding Nemo", "Animation", 8.2, 2003);
    appendMovie(head, 121, "Toy Story", "Animation", 8.3, 1995);
    appendMovie(head, 122, "The Departed", "Crime", 8.5, 2006);
    appendMovie(head, 123, "Shutter Island", "Thriller", 8.2, 2010);
    appendMovie(head, 124, "Gravity", "Sci-Fi", 7.7, 2013);
    appendMovie(head, 125, "Django Unchained", "Western", 8.4, 2012);
    appendMovie(head, 126, "The Revenant", "Adventure", 8.0, 2015);
    appendMovie(head, 127, "Mad Max", "Action", 8.1, 2015);
    appendMovie(head, 128, "The Wolf of Wall Street", "Drama", 8.2, 2013);
    appendMovie(head, 129, "American Beauty", "Drama", 8.3, 1999);
    appendMovie(head, 130, "The Pianist", "Drama", 8.5, 2002);
    appendMovie(head, 131, "Se7en", "Thriller", 8.6, 1995);
    appendMovie(head, 132, "The Silence of the Lambs", "Thriller", 8.6, 1991);
    appendMovie(head, 133, "The Usual Suspects", "Crime", 8.5, 1995);
    appendMovie(head, 134, "Memento", "Thriller", 8.4, 2000);
    appendMovie(head, 135, "The Prestige", "Drama", 8.5, 2006);
    appendMovie(head, 136, "The Green Mile", "Drama", 8.6, 1999);
    appendMovie(head, 137, "Gran Torino", "Drama", 8.1, 2008);
    appendMovie(head, 138, "Million Dollar Baby", "Drama", 8.1, 2004);
    appendMovie(head, 139, "The Pursuit of Happyness", "Drama", 8.0, 2006);
    appendMovie(head, 140, "A Beautiful Mind", "Drama", 8.2, 2001);
    appendMovie(head, 141, "The Aviator", "Drama", 7.5, 2004);
    appendMovie(head, 142, "Catch Me If You Can", "Drama", 8.1, 2002);
    appendMovie(head, 143, "The Terminal", "Drama", 7.4, 2004);
    appendMovie(head, 144, "Cast Away", "Adventure", 7.8, 2000);
    appendMovie(head, 145, "The Truman Show", "Comedy", 8.2, 1998);
    appendMovie(head, 146, "Eternal Sunshine", "Romance", 8.3, 2004);
    appendMovie(head, 147, "The Big Lebowski", "Comedy", 8.1, 1998);
    appendMovie(head, 148, "A Clockwork Orange", "Crime", 8.3, 1971);
    appendMovie(head, 149, "Reservoir Dogs", "Crime", 8.3, 1992);
    appendMovie(head, 150, "The Sixth Sense", "Thriller", 8.2, 1999);
}

void loadMasterDatabase(Movie*& head) {
    try {
        ifstream infile("movies_db.txt");
        if (!infile) {
            throw "Crucial Alert: Master database file execution records missing!";
        }
        char line[200];
        while (infile.getline(line, 200)) {
            if (line[0] == '\0') continue;        
            int    id = -1, year = 0;             
            double rating = 0.0;
            char   title[100] = "", genre[50] = "";
            bool gotId = false, gotTitle = false;

            char* token = strtok(line, ",");
            if (token != NULL) { id = atoi(token); gotId = true; }
            token = strtok(NULL, ",");
            if (token != NULL) { customStrCpy(title, token); gotTitle = true; }
            token = strtok(NULL, ",");
            if (token != NULL) customStrCpy(genre, token);
            token = strtok(NULL, ",");
            if (token != NULL) rating = atof(token);
            token = strtok(NULL, ",");
            if (token != NULL) year = atoi(token);

            if (gotId && gotTitle) {
                appendMovie(head, id, title, genre, rating, year);
            }
        }
        infile.close();
    }
    catch (const char* e) {
        cout << "[IO EXCEPTION HANDLED] " << e << "\nPopulating localized fallback items..." << endl;
        seedDefaultMovies(head);   
    }
    if (getMovieCount(head) == 0) {
        seedDefaultMovies(head);
        saveMoviesToFile(head);
    }
}

// ADMIN HELPER FUNCTIONS
bool addMovieToDatabase(Movie*& head) {
    try {
        int id, year;
        double rating;
        char title[100], genre[50];
        
        id = safeReadInt("Enter Movie ID: ");
        
        if (searchMovie(head, id) != NULL) {
            cout << "Movie ID already exists!\n";
            return false;
        }
        
        cout << "Enter Movie Title: ";
        cin >> ws;
        cin.getline(title, 100);
        cout << "Enter Genre: ";
        cin.getline(genre, 50);
        rating = safeReadDouble("Enter Rating (0-10): ");
        year = safeReadInt("Enter Year: ");
        
        appendMovie(head, id, title, genre, rating, year);
        saveMoviesToFile(head);
        cout << "Movie added successfully!\n";
        return true;
    }
    catch (...) {
        cout << "Error adding movie!\n";
        return false;
    }
}

bool editMovieInDatabase(Movie*& head) {
    try {
        int id;
        cout << "Enter Movie ID to edit: ";
        cin >> id;
        
        Movie* movie = searchMovie(head, id);
        if (movie == NULL) {
            cout << "Movie not found!\n";
            return false;
        }
        
        cout << "Current Details:\n";
        printMovieSpecs(*movie);
        
        cout << "\nEnter new Title (or '-' to keep): ";
        char newTitle[100];
        cin >> ws;
        cin.getline(newTitle, 100);
        if (!customStrCmp(newTitle, "-")) {
            customStrCpy(movie->title, newTitle);
        }
        
        cout << "Enter new Genre (or '-' to keep): ";
        char newGenre[50];
        cin.getline(newGenre, 50);
        if (!customStrCmp(newGenre, "-")) {
            customStrCpy(movie->genre, newGenre);
        }
        
        double newRating = safeReadDouble("Enter new Rating (or -1 to keep): ");
        if (newRating >= 0) {
            movie->rating = newRating;
        }
        
        int newYear = safeReadInt("Enter new Year (or -1 to keep): ");
        if (newYear >= 0) {
            movie->year = newYear;
        }
        
        saveMoviesToFile(head);
        cout << "Movie updated successfully!\n";
        return true;
    }
    catch (...) {
        cout << "Error editing movie!\n";
        return false;
    }
}

bool deleteMovieFromDatabase(Movie*& head) {
    try {
        int id;
        cout << "Enter Movie ID to delete: ";
        cin >> id;
        
        Movie* movie = searchMovie(head, id);
        if (movie == NULL) {
            cout << "Movie not found!\n";
            return false;
        }
        
        if (movie->prev != NULL) {
            movie->prev->next = movie->next;
        } else {
            head = movie->next;
        }
        if (movie->next != NULL) {
            movie->next->prev = movie->prev;
        }
        
        delete movie;
        saveMoviesToFile(head);
        cout << "Movie deleted successfully!\n";
        return true;
    }
    catch (...) {
        cout << "Error deleting movie!\n";
        return false;
    }
}

void displayAllMovies(Movie* head) {
    cout << "\n--- ALL MOVIES (" << getMovieCount(head) << " records) ---\n";
    Movie* temp = head;
    int count = 0;
    while (temp != NULL) {
        printMovieSpecs(*temp);
        temp = temp->next;
        count++;
        if (count % 10 == 0 && temp != NULL) {
            cout << "Press Enter to continue...";
            cin.ignore(1000, '\n');   
            cin.get();                
        }
    }
}

// CUSTOMER HELPER FUNCTIONS
void browseMoviesForCustomer(Movie* head, const char* username) {
    int choice;
    do {
        cout << "\n--- BROWSE MOVIES ---\n";
        cout << "1. View All Movies\n";
        cout << "2. Search by Title\n";
        cout << "3. Search by Genre\n";
        cout << "4. Sort Movies\n";
        cout << "5. Add to Watchlist\n";
        cout << "6. Watch a Movie\n";
        cout << "7. Return to Main Menu\n";
        choice = safeReadInt("Enter choice: ");
        
        if (choice == 1) {
            displayAllMovies(head);
        } else if (choice == 2) {
            char title[100];
            cout << "Enter title to search: ";
            cin >> ws;
            cin.getline(title, 100);
            Movie* movie = searchMovie(head, title);
            if (movie != NULL) {
                printMovieSpecs(*movie);
            } else {
                cout << "Movie not found!\n";
            }
        } else if (choice == 3) {
            char genre[50];
            cout << "Enter genre to search: ";
            cin >> ws;
            cin.getline(genre, 50);
            Movie* temp = head;
            bool found = false;
            while (temp != NULL) {
                if (customStrSubStr(temp->genre, genre)) {
                    printMovieSpecs(*temp);
                    found = true;
                }
                temp = temp->next;
            }
            if (!found) {
                cout << "No movies found in this genre!\n";
            }
        } else if (choice == 4) {
            int crit;
            cout << "Sort by (1: Rating High-Low, 2: Year Old-New): ";
            crit = safeReadInt(NULL);
            bubbleSortMovies(head, crit);
            cout << "Movies sorted!\n";
        } else if (choice == 5) {
            int movieId;
            char status[30];
            displayAllMovies(head);
            cout << "Enter Movie ID to add: ";
            cin >> movieId;
            if (searchMovie(head, movieId) != NULL) {
                cout << "Enter status (Plan to Watch/Completed/Watching): ";
                cin >> ws;
                cin.getline(status, 30);
                addToWatchlist(username, movieId, status);
                saveWatchlistToFile();
                cout << "Added to watchlist!\n";
            } else {
                cout << "Invalid Movie ID!\n";
            }
        } else if (choice == 6) {
            watchMovieFlow(head, username);     
        }
    } while (choice != 7);
}

// REVIEW SYSTEM  (customer ratings & comments, stored in a singly linked list)
void addReview(const char* username, int movieId, int score, const char* comment) {
    Review* newNode = new Review();          
    customStrCpy(newNode->username, username);
    newNode->movieId = movieId;
    newNode->score   = score;
    customStrCpy(newNode->comment, comment);
    newNode->next = NULL;

    if (reviewHead == NULL) {
        reviewHead = newNode;                
    } else {
        Review* temp = reviewHead;
        while (temp->next != NULL) {          
            temp = temp->next;
        }
        temp->next = newNode;                 
    }
}

void displayReviewsForMovie(int movieId) {
    Review* temp = reviewHead;
    bool found = false;
    cout << "\n--- REVIEWS FOR MOVIE ID " << movieId << " ---\n";
    while (temp != NULL) {
        if (temp->movieId == movieId) {
            cout << "User: " << temp->username
                 << " | Score: " << temp->score << "/10"
                 << " | Comment: " << temp->comment << endl;
            found = true;
        }
        temp = temp->next;
    }
    if (!found) {
        cout << "No reviews yet for this movie.\n";
    }
}

void displayReviewsByUser(const char* username) {
    Review* temp = reviewHead;
    bool found = false;
    cout << "\n--- YOUR REVIEWS (" << username << ") ---\n";
    while (temp != NULL) {
        if (customStrCmp(temp->username, username)) {
            cout << "Movie ID: " << temp->movieId
                 << " | Score: " << temp->score << "/10"
                 << " | Comment: " << temp->comment << endl;
            found = true;
        }
        temp = temp->next;
    }
    if (!found) {
        cout << "You have not written any reviews yet.\n";
    }
}

bool editReview(const char* username, int movieId, int newScore, const char* newComment) {
    Review* temp = reviewHead;
    while (temp != NULL) {
        if (customStrCmp(temp->username, username) && temp->movieId == movieId) {
            temp->score = newScore;
            customStrCpy(temp->comment, newComment);
            return true;
        }
        temp = temp->next;
    }
    return false;
}

bool deleteReview(const char* username, int movieId) {
    Review* temp = reviewHead;
    Review* prev = NULL;
    while (temp != NULL) {
        if (customStrCmp(temp->username, username) && temp->movieId == movieId) {
            if (prev == NULL) {
                reviewHead = temp->next;     
            } else {
                prev->next = temp->next;     
            }
            delete temp;                    
            return true;
        }
        prev = temp;
        temp = temp->next;
    }
    return false;
}

double averageReviewScore(int movieId) {
    Review* temp = reviewHead;
    int total = 0;
    int count = 0;
    while (temp != NULL) {
        if (temp->movieId == movieId) {
            total += temp->score;
            count++;
        }
        temp = temp->next;
    }
    if (count == 0) return 0.0;
    return (double)total / count;
}

void saveReviewsToFile() {
    ofstream file("reviews_data.txt");
    if (file.is_open()) {
        Review* temp = reviewHead;
        while (temp != NULL) {
            file << temp->username << "," << temp->movieId << ","
                 << temp->score << "," << temp->comment << "\n";
            temp = temp->next;
        }
        file.close();
    }
}

void loadReviewsFromFile() {
    ifstream file("reviews_data.txt");
    if (file.is_open()) {
        char line[300];
        while (file.getline(line, 300)) {
            if (line[0] == '\0') continue;           
            char username[50] = "", comment[150] = "";
            int  movieId = -1, score = 0;
            bool gotUser = false, gotId = false, gotScore = false, gotComment = false;

            char* token = strtok(line, ",");
            if (token != NULL) { customStrCpy(username, token); gotUser = true; }
            token = strtok(NULL, ",");
            if (token != NULL) { movieId = atoi(token); gotId = true; }
            token = strtok(NULL, ",");
            if (token != NULL) { score = atoi(token); gotScore = true; }
            token = strtok(NULL, ",");
            if (token != NULL) { customStrCpy(comment, token); gotComment = true; }

            if (gotUser && gotId && gotScore && gotComment) {
                addReview(username, movieId, score, comment);
            }
        }
        file.close();
    }
}

void clearReviews() {
    Review* current = reviewHead;
    while (current != NULL) {
        Review* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    reviewHead = NULL;
}

void writeReviewFlow(Movie* head, const char* username) {
    try {
        int movieId;
        cout << "Enter Movie ID to review: ";
        cin >> movieId;

        Movie* m = binarySearchMovie(head, movieId);
        if (m == NULL) {
            cout << "Movie ID not found!\n";
            return;
        }
        cout << "You are reviewing: " << m->title << endl;

        int score;
        int _s = safeReadInt("Enter your score (1-10): "); score = _s;
        if (score < 1 || score > 10) {
            throw "Score must be between 1 and 10!";
        }

        char comment[150];
        cout << "Enter a short comment: ";
        cin >> ws;
        cin.getline(comment, 150);

        addReview(username, movieId, score, comment);
        saveReviewsToFile();
        cout << "Review added successfully!\n";
    }
    catch (const char* msg) {
        cout << "[EXCEPTION] " << msg << endl;
    }
    catch (...) {
        cout << "Error while adding review!\n";
    }
}

// STATISTICS & ANALYTICS REPORTS  (admin side)
int countMoviesByGenre(Movie* head, const char* genre) {
    int count = 0;
    Movie* temp = head;
    while (temp != NULL) {
        if (customStrCmp(temp->genre, genre)) {
            count++;
        }
        temp = temp->next;
    }
    return count;
}

double averageRatingAllMovies(Movie* head) {
    double total = 0.0;
    int count = 0;
    Movie* temp = head;
    while (temp != NULL) {
        total += temp->rating;
        count++;
        temp = temp->next;
    }
    if (count == 0) return 0.0;
    return total / count;
}

Movie* highestRatedMovie(Movie* head) {
    if (head == NULL) return NULL;
    Movie* best = head;
    Movie* temp = head->next;
    while (temp != NULL) {
        if (temp->rating > best->rating) {
            best = temp;
        }
        temp = temp->next;
    }
    return best;
}

Movie* lowestRatedMovie(Movie* head) {
    if (head == NULL) return NULL;
    Movie* worst = head;
    Movie* temp = head->next;
    while (temp != NULL) {
        if (temp->rating < worst->rating) {
            worst = temp;
        }
        temp = temp->next;
    }
    return worst;
}

void generateStatisticsReport(Movie* head) {
    char genres[50][50];      
    int  genreCount = 0;

    Movie* temp = head;
    while (temp != NULL) {
        bool exists = false;
        for (int i = 0; i < genreCount; i++) {
            if (customStrCmp(genres[i], temp->genre)) {
                exists = true;
                break;
            }
        }
        if (!exists && genreCount < 50) {
            customStrCpy(genres[genreCount], temp->genre);
            genreCount++;
        }
        temp = temp->next;
    }

    double avg = averageRatingAllMovies(head);
    Movie* best = highestRatedMovie(head);
    Movie* worst = lowestRatedMovie(head);
    int totalMovies = getMovieCount(head);

    cout << "\n=========== MOVIE STATISTICS REPORT ===========\n";
    cout << "Total movies      : " << totalMovies << endl;
    cout << "Average rating    : " << avg << endl;
    if (best != NULL)  cout << "Highest rated     : " << best->title  << " (" << best->rating  << ")\n";
    if (worst != NULL) cout << "Lowest rated      : " << worst->title << " (" << worst->rating << ")\n";
    cout << "Distinct genres   : " << genreCount << endl;
    cout << "----- Movies per genre -----\n";
    for (int i = 0; i < genreCount; i++) {
        cout << genres[i] << " : " << countMoviesByGenre(head, genres[i]) << endl;
    }

    ofstream report("statistics_report.txt");
    if (report.is_open()) {
        report << "=========== MOVIE STATISTICS REPORT ===========\n";
        report << "Total movies      : " << totalMovies << "\n";
        report << "Average rating    : " << avg << "\n";
        if (best != NULL)  report << "Highest rated     : " << best->title  << " (" << best->rating  << ")\n";
        if (worst != NULL) report << "Lowest rated      : " << worst->title << " (" << worst->rating << ")\n";
        report << "Distinct genres   : " << genreCount << "\n";
        report << "----- Movies per genre -----\n";
        for (int i = 0; i < genreCount; i++) {
            report << genres[i] << " : " << countMoviesByGenre(head, genres[i]) << "\n";
        }
        report.close();
        cout << "\nStatistics report saved to 'statistics_report.txt'.\n";
    }
}

// LOGIN HISTORY  (writes every successful login into a text file)
void recordLoginHistory(const char* username, const char* role) {
    static int sessionNo = 0;       
    sessionNo++;
    ofstream file("login_history.txt", ios::app);   
    if (file.is_open()) {
        file << "Session #" << sessionNo << " | User: " << username
             << " | Role: " << role << "\n";
        file.close();
    }
}

void viewLoginHistory() {
    ifstream file("login_history.txt");
    cout << "\n--- LOGIN HISTORY ---\n";
    if (file.is_open()) {
        char line[200];
        bool any = false;
        while (file.getline(line, 200)) {
            cout << line << endl;
            any = true;
        }
        if (!any) cout << "No login records found.\n";
        file.close();
    } else {
        cout << "No login history file found yet.\n";
    }
}

// RANGE / FILTER SEARCHES  (extra search features beyond the core requirement)
void searchByYearRange(Movie* head, int fromYear, int toYear) {
    cout << "\n--- MOVIES FROM " << fromYear << " TO " << toYear << " ---\n";
    Movie* temp = head;
    bool found = false;
    while (temp != NULL) {
        if (temp->year >= fromYear && temp->year <= toYear) {
            printMovieSpecs(*temp);
            found = true;
        }
        temp = temp->next;
    }
    if (!found) cout << "No movies found in that year range.\n";
}

void searchByMinRating(Movie* head, double minRating) {
    cout << "\n--- MOVIES WITH RATING >= " << minRating << " ---\n";
    Movie* temp = head;
    bool found = false;
    while (temp != NULL) {
        if (temp->rating >= minRating) {
            printMovieSpecs(*temp);
            found = true;
        }
        temp = temp->next;
    }
    if (!found) cout << "No movies meet that rating.\n";
}

void filterSearchMenu(Movie* head) {
    int choice = 0;
    do {
        cout << "\n--- FILTER SEARCH ---\n";
        cout << "1. By year range\n";
        cout << "2. By minimum rating\n";
        cout << "3. By genre count\n";
        cout << "0. Back\n";
        choice = safeReadInt("Enter choice: ");

        if (choice == 1) {
            int a, b;
            cout << "From year: "; cin >> a;
            cout << "To year: ";   cin >> b;
            searchByYearRange(head, a, b);
        } else if (choice == 2) {
            double r;
            cout << "Minimum rating: "; cin >> r;
            searchByMinRating(head, r);
        } else if (choice == 3) {
            char g[50];
            cout << "Enter genre: ";
            cin >> ws;
            cin.getline(g, 50);
            cout << g << " has " << countMoviesByGenre(head, g) << " movie(s).\n";
        } else if (choice != 0) {
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

// ADDITIONAL SORTING ALGORITHMS (Insertion Sort & Merge Sort, manual)
Movie** copyListToArray(Movie* head, int& outCount) {
    int n = 0;
    Movie* walk = head;
    while (walk != NULL) { n++; walk = walk->next; }
    outCount = n;
    if (n == 0) return NULL;

    Movie** arr = new Movie*[n];     
    walk = head;
    for (int i = 0; i < n; i++) {
        arr[i] = walk;
        walk = walk->next;
    }
    return arr;
}

void writeArrayBackToList(Movie* head, Movie** sortedValues, int n) {
    Movie* walk = head;
    for (int i = 0; i < n && walk != NULL; i++) {
        walk->id     = sortedValues[i]->id;
        customStrCpy(walk->title, sortedValues[i]->title);
        customStrCpy(walk->genre, sortedValues[i]->genre);
        walk->rating = sortedValues[i]->rating;
        walk->year   = sortedValues[i]->year;
        walk = walk->next;
    }
}

void insertionSortByTitle(Movie* head) {
    int n = 0;
    Movie* walk = head;
    while (walk != NULL) { n++; walk = walk->next; }
    if (n < 2) return;

    Movie* snap = new Movie[n];
    walk = head;
    for (int i = 0; i < n; i++) {
        snap[i] = *walk;          
        walk = walk->next;
    }

    for (int i = 1; i < n; i++) {
        Movie key = snap[i];
        int j = i - 1;
        while (j >= 0) {
            int a = 0;
            bool greater = false, decided = false;
            while (snap[j].title[a] != '\0' || key.title[a] != '\0') {
                char c1 = snap[j].title[a];
                char c2 = key.title[a];
                if (c1 != c2) { greater = (c1 > c2); decided = true; break; }
                a++;
            }
            if (decided && greater) {
                snap[j + 1] = snap[j];
                j--;
            } else {
                break;
            }
        }
        snap[j + 1] = key;
    }

    walk = head;
    for (int i = 0; i < n && walk != NULL; i++) {
        walk->id = snap[i].id;
        customStrCpy(walk->title, snap[i].title);
        customStrCpy(walk->genre, snap[i].genre);
        walk->rating = snap[i].rating;
        walk->year = snap[i].year;
        walk = walk->next;
    }
    delete[] snap;
}

void mergeByYear(Movie* arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Movie* L = new Movie[n1];
    Movie* R = new Movie[n2];
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].year <= R[j].year) {
            arr[k] = L[i]; i++;
        } else {
            arr[k] = R[j]; j++;
        }
        k++;
    }
    while (i < n1) { arr[k] = L[i]; i++; k++; }
    while (j < n2) { arr[k] = R[j]; j++; k++; }

    delete[] L;
    delete[] R;
}

void mergeSortByYear(Movie* arr, int left, int right) {
    if (left < right) {
        int mid = (left + right) / 2;
        mergeSortByYear(arr, left, mid);        
        mergeSortByYear(arr, mid + 1, right);   
        mergeByYear(arr, left, mid, right);     
    }
}

void mergeSortMoviesByYear(Movie* head) {
    int n = 0;
    Movie* walk = head;
    while (walk != NULL) { n++; walk = walk->next; }
    if (n < 2) return;

    Movie* snap = new Movie[n];
    walk = head;
    for (int i = 0; i < n; i++) { snap[i] = *walk; walk = walk->next; }

    mergeSortByYear(snap, 0, n - 1);

    walk = head;
    for (int i = 0; i < n && walk != NULL; i++) {
        walk->id = snap[i].id;
        customStrCpy(walk->title, snap[i].title);
        customStrCpy(walk->genre, snap[i].genre);
        walk->rating = snap[i].rating;
        walk->year = snap[i].year;
        walk = walk->next;
    }
    delete[] snap;
}


// HASH TABLE for fast movie lookup by ID (separate chaining)
const int HASH_SIZE = 17;   

struct HashEntry {
    int movieId;
    char title[100];
    HashEntry* next;        
};

struct MovieHashTable {
    HashEntry* buckets[HASH_SIZE];
};

void initHashTable(MovieHashTable& ht) {
    for (int i = 0; i < HASH_SIZE; i++) {
        ht.buckets[i] = NULL;
    }
}

int hashFunction(int id) {
    return id % HASH_SIZE;
}

void hashInsert(MovieHashTable& ht, int id, const char* title) {
    int index = hashFunction(id);
    HashEntry* node = new HashEntry();
    node->movieId = id;
    customStrCpy(node->title, title);
    node->next = ht.buckets[index];   
    ht.buckets[index] = node;
}

void buildHashTable(MovieHashTable& ht, Movie* head) {
    initHashTable(ht);
    Movie* temp = head;
    while (temp != NULL) {
        hashInsert(ht, temp->id, temp->title);
        temp = temp->next;
    }
}

bool hashSearch(MovieHashTable& ht, int id, char* outTitle) {
    int index = hashFunction(id);
    HashEntry* node = ht.buckets[index];
    while (node != NULL) {
        if (node->movieId == id) {
            customStrCpy(outTitle, node->title);
            return true;
        }
        node = node->next;
    }
    return false;
}

void clearHashTable(MovieHashTable& ht) {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashEntry* node = ht.buckets[i];
        while (node != NULL) {
            HashEntry* nextNode = node->next;
            delete node;
            node = nextNode;
        }
        ht.buckets[i] = NULL;
    }
}

// RECOMMENDATION FEATURE (fits the "Movie Recommendation System" theme)
void recommendByGenre(Movie* head, const char* username) {
    int movieId;
    cout << "Enter a Movie ID you like (we'll recommend similar ones): ";
    cin >> movieId;

    Movie* base = binarySearchMovie(head, movieId);
    if (base == NULL) {
        cout << "Movie ID not found!\n";
        return;
    }
    cout << "\nBecause you like \"" << base->title
         << "\" (" << base->genre << "), you may also enjoy:\n";

    int n = 0;
    Movie* walk = head;
    while (walk != NULL) {
        if (customStrCmp(walk->genre, base->genre) && walk->id != base->id) n++;
        walk = walk->next;
    }
    if (n == 0) {
        cout << "No other movies found in this genre.\n";
        return;
    }

    Movie** arr = new Movie*[n];
    int idx = 0;
    walk = head;
    while (walk != NULL) {
        if (customStrCmp(walk->genre, base->genre) && walk->id != base->id) {
            arr[idx] = walk;
            idx++;
        }
        walk = walk->next;
    }

    for (int i = 0; i < n - 1; i++) {
        int maxI = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j]->rating > arr[maxI]->rating) maxI = j;
        }
        if (maxI != i) {
            Movie* t = arr[i]; arr[i] = arr[maxI]; arr[maxI] = t;
        }
    }

    int show = (n < 5) ? n : 5;
    for (int i = 0; i < show; i++) {
        cout << "  - " << arr[i]->title << " (Rating: " << arr[i]->rating << ")\n";
    }
    delete[] arr;
}

// CUSTOMER REVIEW MANAGEMENT FLOWS (edit / delete own review)
void editReviewFlow(const char* username) {
    displayReviewsByUser(username);
    int movieId;
    cout << "Enter the Movie ID of the review to edit: ";
    cin >> movieId;
    int newScore;
    newScore = safeReadInt("Enter new score (1-10): ");
    if (newScore < 1 || newScore > 10) {
        cout << "Invalid score.\n";
        return;
    }
    char newComment[150];
    cout << "Enter new comment: ";
    cin >> ws;
    cin.getline(newComment, 150);
    if (editReview(username, movieId, newScore, newComment)) {
        saveReviewsToFile();
        cout << "Review updated!\n";
    } else {
        cout << "No matching review found.\n";
    }
}

void deleteReviewFlow(const char* username) {
    displayReviewsByUser(username);
    int movieId;
    cout << "Enter the Movie ID of the review to delete: ";
    cin >> movieId;
    if (deleteReview(username, movieId)) {
        saveReviewsToFile();
        cout << "Review deleted!\n";
    } else {
        cout << "No matching review found.\n";
    }
}

// ADMIN REVIEW MODERATION (view all reviews, delete any review)
void displayAllReviews() {
    Review* temp = reviewHead;
    cout << "\n--- ALL CUSTOMER REVIEWS ---\n";
    bool any = false;
    while (temp != NULL) {
        cout << "User: " << temp->username
             << " | Movie ID: " << temp->movieId
             << " | Score: " << temp->score
             << " | Comment: " << temp->comment << endl;
        any = true;
        temp = temp->next;
    }
    if (!any) cout << "No reviews in the system.\n";
}

void adminDeleteReviewFlow() {
    displayAllReviews();
    char targetUser[50];
    int movieId;
    cout << "Enter username of the review to remove: ";
    cin >> targetUser;
    cout << "Enter movie ID of that review: ";
    cin >> movieId;
    if (deleteReview(targetUser, movieId)) {
        saveReviewsToFile();
        cout << "Review removed by admin.\n";
    } else {
        cout << "No matching review found.\n";
    }
}

// EXTRA UTILITIES & REPORTS (catalog export, decade analysis, top-N, about)
void exportFullCatalog(Movie* head) {
    ofstream file("full_catalog.txt");
    if (!file.is_open()) {
        cout << "Could not create catalogue file.\n";
        return;
    }
    file << "==================================================\n";
    file << "          FULL MOVIE CATALOGUE EXPORT             \n";
    file << "==================================================\n";
    file << "Total movies: " << getMovieCount(head) << "\n\n";

    Movie* temp = head;
    int rowNo = 1;
    while (temp != NULL) {
        file << rowNo << ". [" << temp->id << "] "
             << temp->title << " | " << temp->genre
             << " | Rating " << temp->rating
             << " | Year " << temp->year << "\n";
        rowNo++;
        temp = temp->next;
    }
    file.close();
    cout << "Full catalogue exported to 'full_catalog.txt'.\n";
}

void decadeDistribution(Movie* head) {
    int buckets[6] = {0, 0, 0, 0, 0, 0};
    const int baseDecade = 1970;

    Movie* temp = head;
    while (temp != NULL) {
        int idx = (temp->year - baseDecade) / 10;
        if (idx >= 0 && idx < 6) {
            buckets[idx]++;
        }
        temp = temp->next;
    }

    cout << "\n--- MOVIES PER DECADE ---\n";
    for (int i = 0; i < 6; i++) {
        int decadeStart = baseDecade + i * 10;
        cout << decadeStart << "s : " << buckets[i] << " movie(s)\n";
    }
}

void topNRatedMovies(Movie* head, int topN) {
    int n = 0;
    Movie* walk = head;
    while (walk != NULL) { n++; walk = walk->next; }
    if (n == 0) {
        cout << "No movies available.\n";
        return;
    }

    Movie** arr = new Movie*[n];
    walk = head;
    for (int i = 0; i < n; i++) { arr[i] = walk; walk = walk->next; }

    for (int i = 0; i < n - 1; i++) {
        int maxI = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j]->rating > arr[maxI]->rating) maxI = j;
        }
        if (maxI != i) { Movie* t = arr[i]; arr[i] = arr[maxI]; arr[maxI] = t; }
    }

    if (topN > n) topN = n;
    cout << "\n--- TOP " << topN << " HIGHEST-RATED MOVIES ---\n";
    for (int i = 0; i < topN; i++) {
        cout << (i + 1) << ". " << arr[i]->title
             << " (" << arr[i]->rating << ")\n";
    }
    delete[] arr;
}

double averageRatingByGenre(Movie* head, const char* genre) {
    double total = 0.0;
    int count = 0;
    Movie* temp = head;
    while (temp != NULL) {
        if (customStrCmp(temp->genre, genre)) {
            total += temp->rating;
            count++;
        }
        temp = temp->next;
    }
    if (count == 0) return 0.0;
    return total / count;
}

int countMoviesByYear(Movie* head, int year) {
    int count = 0;
    Movie* temp = head;
    while (temp != NULL) {
        if (temp->year == year) count++;
        temp = temp->next;
    }
    return count;
}

void showAboutScreen() {
    cout << "\n==================================================\n";
    cout << "        ABOUT THIS SYSTEM\n";
    cout << "==================================================\n";
    cout << "Movie Recommendation System (TDS4223 Project)\n";
    cout << "Data structures used:\n";
    cout << "  - Doubly linked list  : movie catalogue\n";
    cout << "  - Singly linked list  : watchlist & reviews\n";
    cout << "  - Hash table          : fast lookup by movie id\n";
    cout << "Algorithms used:\n";
    cout << "  - Bubble / Insertion / Merge / Selection sort\n";
    cout << "  - Binary search (sorted array) + Hash search\n";
    cout << "Files used: movies_db, credentials, watchlist_data,\n";
    cout << "            reviews_data, login_history, reports.\n";
    cout << "==================================================\n";
}

void analyticsMenu(Movie* head) {
    int choice = 0;
    do {
        cout << "\n--- ANALYTICS MENU ---\n";
        cout << "1. Movies per decade\n";
        cout << "2. Top N highest rated\n";
        cout << "3. Average rating by genre\n";
        cout << "4. Count movies by year\n";
        cout << "5. Export full catalogue (.txt)\n";
        cout << "0. Back\n";
        choice = safeReadInt("Enter choice: ");

        if (choice == 1) {
            decadeDistribution(head);
        } else if (choice == 2) {
            int k;
            cout << "How many top movies to show? ";
            cin >> k;
            topNRatedMovies(head, k);
        } else if (choice == 3) {
            char g[50];
            cout << "Enter genre: ";
            cin >> ws;
            cin.getline(g, 50);
            cout << "Average rating for " << g << " : "
                 << averageRatingByGenre(head, g) << "\n";
        } else if (choice == 4) {
            int y;
            cout << "Enter year: ";
            cin >> y;
            cout << countMoviesByYear(head, y) << " movie(s) released in " << y << ".\n";
        } else if (choice == 5) {
            exportFullCatalog(head);
        } else if (choice != 0) {
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

// FAVORITES MODULE  (each customer can save favourite movies)
bool isFavorite(const char* username, int movieId) {
    FavoriteNode* temp = favoriteHead;
    while (temp != NULL) {
        if (customStrCmp(temp->username, username) && temp->movieId == movieId) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

void addFavorite(const char* username, int movieId) {
    if (isFavorite(username, movieId)) {
        cout << "Already in your favourites.\n";
        return;
    }
    FavoriteNode* node = new FavoriteNode();
    customStrCpy(node->username, username);
    node->movieId = movieId;
    node->next = favoriteHead;     
    favoriteHead = node;
    cout << "Added to favourites.\n";
}

bool removeFavorite(const char* username, int movieId) {
    FavoriteNode* temp = favoriteHead;
    FavoriteNode* prev = NULL;
    while (temp != NULL) {
        if (customStrCmp(temp->username, username) && temp->movieId == movieId) {
            if (prev == NULL) favoriteHead = temp->next;
            else              prev->next = temp->next;
            delete temp;
            return true;
        }
        prev = temp;
        temp = temp->next;
    }
    return false;
}

void displayFavorites(Movie* head, const char* username) {
    FavoriteNode* temp = favoriteHead;
    bool found = false;
    cout << "\n--- YOUR FAVOURITE MOVIES ---\n";
    while (temp != NULL) {
        if (customStrCmp(temp->username, username)) {
            Movie* m = binarySearchMovie(head, temp->movieId);
            if (m != NULL) {
                cout << "  [" << m->id << "] " << m->title
                     << " (" << m->genre << ", " << m->rating << ")\n";
            } else {
                cout << "  Movie ID " << temp->movieId << " (details not found)\n";
            }
            found = true;
        }
        temp = temp->next;
    }
    if (!found) cout << "You have no favourites yet.\n";
}

void saveFavoritesToFile() {
    ofstream file("favorites_data.txt");
    if (file.is_open()) {
        FavoriteNode* temp = favoriteHead;
        while (temp != NULL) {
            file << temp->username << "," << temp->movieId << "\n";
            temp = temp->next;
        }
        file.close();
    }
}

void loadFavoritesFromFile() {
    ifstream file("favorites_data.txt");
    if (file.is_open()) {
        char line[120];
        while (file.getline(line, 120)) {
            if (line[0] == '\0') continue;
            char username[50] = "";
            int  movieId = -1;
            bool gotUser = false, gotId = false;
            char* token = strtok(line, ",");
            if (token != NULL) { customStrCpy(username, token); gotUser = true; }
            token = strtok(NULL, ",");
            if (token != NULL) { movieId = atoi(token); gotId = true; }
            if (gotUser && gotId) {
                FavoriteNode* node = new FavoriteNode();
                customStrCpy(node->username, username);
                node->movieId = movieId;
                node->next = favoriteHead;
                favoriteHead = node;
            }
        }
        file.close();
    }
}

void clearFavorites() {
    FavoriteNode* current = favoriteHead;
    while (current != NULL) {
        FavoriteNode* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    favoriteHead = NULL;
}

void favoritesMenu(Movie* head, const char* username) {
    int choice = 0;
    do {
        cout << "\n--- FAVOURITES MENU ---\n";
        cout << "1. View my favourites\n";
        cout << "2. Add a favourite (by ID)\n";
        cout << "3. Remove a favourite (by ID)\n";
        cout << "0. Back\n";
        choice = safeReadInt("Enter choice: ");

        if (choice == 1) {
            displayFavorites(head, username);
        } else if (choice == 2) {
            int id;
            cout << "Enter Movie ID: ";
            cin >> id;
            if (binarySearchMovie(head, id) != NULL) {
                addFavorite(username, id);
                saveFavoritesToFile();
            } else {
                cout << "Movie ID not found!\n";
            }
        } else if (choice == 3) {
            int id;
            cout << "Enter Movie ID to remove: ";
            cin >> id;
            if (removeFavorite(username, id)) {
                saveFavoritesToFile();
                cout << "Removed from favourites.\n";
            } else {
                cout << "That movie is not in your favourites.\n";
            }
        } else if (choice != 0) {
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

// RECENTLY VIEWED MODULE  (tracks the last movies a user looked at)
void recordRecentView(const char* username, int movieId) {
    RecentNode* node = new RecentNode();
    customStrCpy(node->username, username);
    node->movieId = movieId;
    node->next = recentHead;
    recentHead = node;
}

void displayRecentViews(Movie* head, const char* username) {
    RecentNode* temp = recentHead;
    int shown = 0;
    const int LIMIT = 10;
    cout << "\n--- RECENTLY VIEWED (newest first) ---\n";
    bool found = false;
    while (temp != NULL && shown < LIMIT) {
        if (customStrCmp(temp->username, username)) {
            Movie* m = binarySearchMovie(head, temp->movieId);
            if (m != NULL) {
                cout << "  " << m->title << " (" << m->year << ")\n";
            }
            shown++;
            found = true;
        }
        temp = temp->next;
    }
    if (!found) cout << "No recent views yet.\n";
}

void clearRecent() {
    RecentNode* current = recentHead;
    while (current != NULL) {
        RecentNode* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    recentHead = NULL;
}

// CAST INFO MODULE  (director & lead actor per movie, parallel linked list)
struct CastInfo {
    int  movieId;
    char director[60];
    char leadActor[60];
    CastInfo* next;
};
CastInfo* castHead = NULL;

void addCastInfo(int movieId, const char* director, const char* leadActor) {
    CastInfo* temp = castHead;
    while (temp != NULL) {
        if (temp->movieId == movieId) {
            customStrCpy(temp->director, director);
            customStrCpy(temp->leadActor, leadActor);
            return;
        }
        temp = temp->next;
    }
    CastInfo* node = new CastInfo();
    node->movieId = movieId;
    customStrCpy(node->director, director);
    customStrCpy(node->leadActor, leadActor);
    node->next = castHead;
    castHead = node;
}

CastInfo* findCastInfo(int movieId) {
    CastInfo* temp = castHead;
    while (temp != NULL) {
        if (temp->movieId == movieId) return temp;
        temp = temp->next;
    }
    return NULL;
}

void searchByActor(Movie* head, const char* actorName) {
    cout << "\n--- MOVIES FEATURING \"" << actorName << "\" ---\n";
    bool found = false;
    CastInfo* temp = castHead;
    while (temp != NULL) {
        if (customStrSubStr(temp->leadActor, actorName)) {
            Movie* m = binarySearchMovie(head, temp->movieId);
            if (m != NULL) {
                cout << "  " << m->title << " (dir. " << temp->director << ")\n";
                found = true;
            }
        }
        temp = temp->next;
    }
    if (!found) cout << "No movies found for that actor.\n";
}

void saveCastToFile() {
    ofstream file("cast_data.txt");
    if (file.is_open()) {
        CastInfo* temp = castHead;
        while (temp != NULL) {
            file << temp->movieId << "," << temp->director << "," << temp->leadActor << "\n";
            temp = temp->next;
        }
        file.close();
    }
}

void loadCastFromFile() {
    ifstream file("cast_data.txt");
    if (file.is_open()) {
        char line[200];
        while (file.getline(line, 200)) {
            if (line[0] == '\0') continue;
            int movieId = -1;
            char director[60] = "", actor[60] = "";
            bool gotId = false, gotDir = false, gotAct = false;
            char* token = strtok(line, ",");
            if (token != NULL) { movieId = atoi(token); gotId = true; }
            token = strtok(NULL, ",");
            if (token != NULL) { customStrCpy(director, token); gotDir = true; }
            token = strtok(NULL, ",");
            if (token != NULL) { customStrCpy(actor, token); gotAct = true; }
            if (gotId && gotDir && gotAct) {
                addCastInfo(movieId, director, actor);
            }
        }
        file.close();
    }
}

void clearCast() {
    CastInfo* current = castHead;
    while (current != NULL) {
        CastInfo* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    castHead = NULL;
}

void manageCastFlow(Movie* head) {
    int id;
    cout << "Enter Movie ID to add/update cast info: ";
    cin >> id;
    if (binarySearchMovie(head, id) == NULL) {
        cout << "Movie ID not found!\n";
        return;
    }
    char director[60], actor[60];
    cout << "Enter director name: ";
    cin >> ws;
    cin.getline(director, 60);
    cout << "Enter lead actor name: ";
    cin.getline(actor, 60);
    addCastInfo(id, director, actor);
    saveCastToFile();
    cout << "Cast info saved.\n";
}

// MEMBERSHIP / LOYALTY POINTS  (simple tier logic based on points)
const char* membershipTier(int points) {
    if (points >= 500) return "Platinum";
    if (points >= 300) return "Gold";
    if (points >= 150) return "Silver";
    return "Bronze";
}

void showLoyaltySummary(const char* username) {
    int points = 100;  

    WatchlistNode* w = watchlistHead;
    while (w != NULL) {
        if (customStrCmp(w->username, username)) points += 10;
        w = w->next;
    }
    Review* r = reviewHead;
    while (r != NULL) {
        if (customStrCmp(r->username, username)) points += 20;
        r = r->next;
    }
    FavoriteNode* f = favoriteHead;
    while (f != NULL) {
        if (customStrCmp(f->username, username)) points += 5;
        f = f->next;
    }

    cout << "\n--- LOYALTY SUMMARY for " << username << " ---\n";
    cout << "Total points : " << points << "\n";
    cout << "Membership   : " << membershipTier(points) << "\n";
}

// BATCH IMPORT  (admin can import many movies from a text file at once)
void batchImportMovies(Movie*& head) {
    char filename[100];
    cout << "Enter the import file name (e.g. import.txt): ";
    cin >> ws;
    cin.getline(filename, 100);

    ifstream infile(filename);
    if (!infile) {
        cout << "Could not open '" << filename << "'. Make sure it exists.\n";
        return;
    }

    int added = 0, skipped = 0;
    char line[200];
    while (infile.getline(line, 200)) {
        if (line[0] == '\0') continue;
        int id = -1, year = 0;
        double rating = 0.0;
        char title[100] = "", genre[50] = "";
        bool gotId = false, gotTitle = false;

        char* token = strtok(line, ",");
        if (token != NULL) { id = atoi(token); gotId = true; }
        token = strtok(NULL, ",");
        if (token != NULL) { customStrCpy(title, token); gotTitle = true; }
        token = strtok(NULL, ",");
        if (token != NULL) customStrCpy(genre, token);
        token = strtok(NULL, ",");
        if (token != NULL) rating = atof(token);
        token = strtok(NULL, ",");
        if (token != NULL) year = atoi(token);

        if (gotId && gotTitle) {
            if (binarySearchMovie(head, id) == NULL) {   
                appendMovie(head, id, title, genre, rating, year);
                added++;
            } else {
                skipped++;
            }
        } else {
            skipped++;
        }
    }
    infile.close();
    saveMoviesToFile(head);
    cout << "Import complete. Added: " << added << ", Skipped: " << skipped << ".\n";
}

// ADDITIONAL CAST & REVIEW UTILITIES (final feature set)
void searchByDirector(Movie* head, const char* directorName) {
    cout << "\n--- MOVIES DIRECTED BY \"" << directorName << "\" ---\n";
    bool found = false;
    CastInfo* temp = castHead;
    while (temp != NULL) {
        if (customStrSubStr(temp->director, directorName)) {
            Movie* m = binarySearchMovie(head, temp->movieId);
            if (m != NULL) {
                cout << "  " << m->title
                     << " (lead: " << temp->leadActor << ")\n";
                found = true;
            }
        }
        temp = temp->next;
    }
    if (!found) cout << "No movies found for that director.\n";
}

void displayCastForMovie(int movieId) {
    CastInfo* c = findCastInfo(movieId);
    if (c == NULL) {
        cout << "No cast info recorded for this movie.\n";
        return;
    }
    cout << "Director  : " << c->director << "\n";
    cout << "Lead actor: " << c->leadActor << "\n";
}

void castExploreMenu(Movie* head) {
    int choice = 0;
    do {
        cout << "\n--- CAST EXPLORER ---\n";
        cout << "1. View cast of a movie (by ID)\n";
        cout << "2. Search by actor\n";
        cout << "3. Search by director\n";
        cout << "0. Back\n";
        choice = safeReadInt("Enter choice: ");

        if (choice == 1) {
            int id;
            cout << "Enter Movie ID: ";
            cin >> id;
            if (binarySearchMovie(head, id) != NULL) {
                displayCastForMovie(id);
            } else {
                cout << "Movie ID not found!\n";
            }
        } else if (choice == 2) {
            char name[60];
            cout << "Enter actor name: ";
            cin >> ws;
            cin.getline(name, 60);
            searchByActor(head, name);
        } else if (choice == 3) {
            char name[60];
            cout << "Enter director name: ";
            cin >> ws;
            cin.getline(name, 60);
            searchByDirector(head, name);
        } else if (choice != 0) {
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

int countReviewsForMovie(int movieId) {
    int count = 0;
    Review* temp = reviewHead;
    while (temp != NULL) {
        if (temp->movieId == movieId) count++;
        temp = temp->next;
    }
    return count;
}

int mostReviewedMovieId() {
    int bestId = -1;
    int bestCount = 0;
    Review* outer = reviewHead;
    while (outer != NULL) {
        int c = countReviewsForMovie(outer->movieId);
        if (c > bestCount) {
            bestCount = c;
            bestId = outer->movieId;
        }
        outer = outer->next;
    }
    return bestId;
}

void exportReviewsReport(Movie* head) {
    ofstream file("reviews_report.txt");
    if (!file.is_open()) {
        cout << "Could not create reviews report file.\n";
        return;
    }
    file << "==================================================\n";
    file << "             CUSTOMER REVIEWS REPORT              \n";
    file << "==================================================\n";

    Review* temp = reviewHead;
    int total = 0;
    while (temp != NULL) {
        Movie* m = binarySearchMovie(head, temp->movieId);
        const char* title = (m != NULL) ? m->title : "(unknown)";
        file << "User: " << temp->username
             << " | Movie: " << title
             << " | Score: " << temp->score
             << " | " << temp->comment << "\n";
        total++;
        temp = temp->next;
    }
    file << "\nTotal reviews: " << total << "\n";

    int topId = mostReviewedMovieId();
    if (topId != -1) {
        Movie* m = binarySearchMovie(head, topId);
        if (m != NULL) {
            file << "Most reviewed movie: " << m->title
                 << " (" << countReviewsForMovie(topId) << " reviews)\n";
        }
    }
    file.close();
    cout << "Reviews report exported to 'reviews_report.txt'.\n";
}

int countWatchlistItems(const char* username) {
    int count = 0;
    WatchlistNode* temp = watchlistHead;
    while (temp != NULL) {
        if (customStrCmp(temp->username, username)) count++;
        temp = temp->next;
    }
    return count;
}

void showCustomerDashboard(const char* username) {
    int wl = countWatchlistItems(username);

    int reviews = 0;
    Review* r = reviewHead;
    while (r != NULL) {
        if (customStrCmp(r->username, username)) reviews++;
        r = r->next;
    }

    int favs = 0;
    FavoriteNode* f = favoriteHead;
    while (f != NULL) {
        if (customStrCmp(f->username, username)) favs++;
        f = f->next;
    }

    cout << "\n--- MY DASHBOARD (" << username << ") ---\n";
    cout << "Watchlist items : " << wl << "\n";
    cout << "Reviews written : " << reviews << "\n";
    cout << "Favourites      : " << favs << "\n";
    cout << "Membership tier : " << membershipTier(100 + wl * 10 + reviews * 20 + favs * 5) << "\n";
}

int totalReviewsInSystem() {
    int count = 0;
    Review* temp = reviewHead;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

int totalFavoritesInSystem() {
    int count = 0;
    FavoriteNode* temp = favoriteHead;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

int totalWatchlistInSystem() {
    int count = 0;
    WatchlistNode* temp = watchlistHead;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

void showSystemSummary(Movie* head) {
    cout << "\n--- SYSTEM SUMMARY ---\n";
    cout << "Total movies        : " << getMovieCount(head) << "\n";
    cout << "Total reviews       : " << totalReviewsInSystem() << "\n";
    cout << "Total favourites    : " << totalFavoritesInSystem() << "\n";
    cout << "Total watchlist rows : " << totalWatchlistInSystem() << "\n";
    int topId = mostReviewedMovieId();
    if (topId != -1) {
        Movie* m = binarySearchMovie(head, topId);
        if (m != NULL) {
            cout << "Most reviewed movie : " << m->title << "\n";
        }
    }
}

// WATCH A MOVIE  (customer feature: "play" a movie, record it, optional extras)
void watchMovieFlow(Movie* head, const char* username) {
    try {
        int id;
        cout << "\nEnter the Movie ID you want to watch: ";
        cin >> id;

        Movie* m = binarySearchMovie(head, id);
        if (m == NULL) {
            throw "Movie ID not found!";
        }

        cout << "\n=================================================\n";
        cout << "   NOW PLAYING\n";
        cout << "   Title : " << m->title << " (" << m->year << ")\n";
        cout << "   Genre : " << m->genre << "\n";
        cout << "   Rating: " << m->rating << "/10\n";
        cout << "=================================================\n";
        cout << "   ...streaming...  Enjoy the movie!\n\n";

        recordRecentView(username, id);

        char ans;
        cout << "Add this movie to your favourites? (y/n): ";
        cin >> ans;
        if (ans == 'y' || ans == 'Y') {
            addFavorite(username, id);
            saveFavoritesToFile();
        }

        cout << "Would you like to write a review now? (y/n): ";
        cin >> ans;
        if (ans == 'y' || ans == 'Y') {
            int score;
            int _s = safeReadInt("Enter your score (1-10): "); score = _s;
            if (score < 1 || score > 10) {
                throw "Score must be between 1 and 10!";
            }
            char comment[150];
            cout << "Enter a short comment: ";
            cin >> ws;
            cin.getline(comment, 150);
            addReview(username, id, score, comment);
            saveReviewsToFile();
            cout << "Thanks! Your review was saved.\n";
        }
    }
    catch (const char* msg) {
        cout << "[EXCEPTION] " << msg << endl;
    }
    catch (...) {
        cout << "Something went wrong while watching the movie.\n";
    }
}

// MAIN EXECUTIVE INTERFACE HUB
int main() {
    Movie* movieRepositoryHead = NULL;
    loadMasterDatabase(movieRepositoryHead);
    loadWatchlistFromFile();
    loadReviewsFromFile();   
    loadFavoritesFromFile(); 
    loadCastFromFile();      

    AdminModule adminEngine;
    CustomerModule customerEngine;
    SystemLogger logEngine;

    logEngine.logEvent("Application Engine safely initialized inside Dev-C++ Workspace Container.");

    int coreChoice = 0;
    while (true) {
        cout << "\n==============================================" << endl;
        cout << "   WELCOME TO THE MOVIE RECOMMENDATION SYSTEM   " << endl;
        cout << "==============================================" << endl;
        cout << "1. Staff / Admin Login Terminal" << endl;
        cout << "2. Customer Login Portal" << endl;
        cout << "3. Register New Account" << endl;
        cout << "4. Terminate Application Instance" << endl;
        cout << "Enter system choice destination: ";
        
        if (!(cin >> coreChoice)) {
            cin.clear();
            char discard;
            while (cin.get(discard) && discard != '\n');
            cout << "Invalid terminal selection type. Retry again." << endl;
            continue;
        }

        if (coreChoice == 4) {
            cout << "Shutting down database context safely..." << endl;
            break;
        }

        if (coreChoice == 3) {
            int regChoice;
            cout << "Register as:\n1. Admin\n2. Customer\nEnter choice: ";
            regChoice = safeReadInt(NULL);
            
            char user[50], pass[50], confirmPass[50];
            cout << "Enter Username: ";
            cin >> user;
            cout << "Enter Password: ";
            cin >> pass;
            cout << "Confirm Password: ";
            cin >> confirmPass;
            
            if (!customStrCmp(pass, confirmPass)) {
                cout << "Passwords don't match!\n";
                continue;
            }
            
            if (regChoice == 1) {
                if (adminEngine.adminRegister(user, pass)) {
                    cout << "Admin registered successfully!\n";
                }
            } else if (regChoice == 2) {
                if (customerEngine.customerRegister(user, pass)) {
                    cout << "Customer registered successfully!\n";
                }
            } else {
                cout << "Invalid choice!\n";
            }
            continue;
        }

        if (coreChoice == 1) {
            char user[50], pass[50];
            cout << "Enter Staff User ID credentials: ";
            cin >> user;
            cout << "Enter Staff Verification Security Pin: ";
            cin >> pass;

            if (adminEngine.adminLogin(user, pass)) {
                logEngine.logEvent("Authorized Access granted to Administrative Panel Profile.");
                showAdminStats(adminEngine);      
                recordLoginHistory(user, "admin");    
                int subChoice = 0;
                while (adminEngine.getLoginStatus()) {
                    adminEngine.displayMenu();
                    cout << "Select Admin structural option: ";
                    subChoice = safeReadInt(NULL);

                    if (subChoice == 8) {
                        adminEngine.logout();
                        cout << "Safely logged out of admin context successfully." << endl;
                    } 
                    else if (subChoice == 9) {
                        generateStatisticsReport(movieRepositoryHead);
                    }
                    else if (subChoice == 10) {
                        viewLoginHistory();
                    }
                    else if (subChoice == 11) {
                        filterSearchMenu(movieRepositoryHead);
                    }
                    else if (subChoice == 12) {
                        int s;
                        cout << "Sort by (1: Title A-Z Insertion, 2: Year Merge Sort): ";
                        s = safeReadInt(NULL);
                        if (s == 1) {
                            insertionSortByTitle(movieRepositoryHead);
                            cout << "Sorted by title using Insertion Sort.\n";
                        } else {
                            mergeSortMoviesByYear(movieRepositoryHead);
                            cout << "Sorted by year using Merge Sort.\n";
                        }
                        saveMoviesToFile(movieRepositoryHead);
                    }
                    else if (subChoice == 13) {
                        MovieHashTable ht;
                        buildHashTable(ht, movieRepositoryHead);
                        int id;
                        cout << "Enter Movie ID to look up via hash table: ";
                        cin >> id;
                        char foundTitle[100];
                        if (hashSearch(ht, id, foundTitle)) {
                            cout << "[HASH HIT] ID " << id << " -> " << foundTitle << endl;
                        } else {
                            cout << "[HASH MISS] No movie with that ID.\n";
                        }
                        clearHashTable(ht);
                    }
                    else if (subChoice == 14) {
                        adminDeleteReviewFlow();
                    }
                    else if (subChoice == 15) {
                        analyticsMenu(movieRepositoryHead);
                    }
                    else if (subChoice == 16) {
                        showAboutScreen();
                    }
                    else if (subChoice == 17) {
                        manageCastFlow(movieRepositoryHead);
                    }
                    else if (subChoice == 18) {
                        batchImportMovies(movieRepositoryHead);
                    }
                    else if (subChoice == 19) {
                        exportReviewsReport(movieRepositoryHead);
                    }
                    else if (subChoice == 20) {
                        showSystemSummary(movieRepositoryHead);
                    }
                    else if (subChoice == 1) {
                        addMovieToDatabase(movieRepositoryHead);
                    }
                    else if (subChoice == 2) {
                        editMovieInDatabase(movieRepositoryHead);
                    }
                    else if (subChoice == 3) {
                        deleteMovieFromDatabase(movieRepositoryHead);
                    }
                    else if (subChoice == 4) {
                        int crit;
                        cout << "Select Manual Sorting Criteria (1: High-to-Low Rating, 2: Chronological Year): ";
                        crit = safeReadInt(NULL);
                        bubbleSortMovies(movieRepositoryHead, crit);
                        saveMoviesToFile(movieRepositoryHead);
                        cout << "Database successfully reordered in memory via custom Bubble Sort algorithm!" << endl;
                    }
                    else if (subChoice == 5) {
                        cout << "Search by:\n1. ID\n2. Title\nEnter choice: ";
                        int searchChoice;
                        searchChoice = safeReadInt(NULL);
                        if (searchChoice == 1) {
                            int id;
                            cout << "Enter ID: ";
                            cin >> id;
                            Movie* match = binarySearchMovie(movieRepositoryHead, id);  
                            if (match != NULL) {
                                printMovieSpecs(*match);
                            } else {
                                cout << "Movie not found!\n";
                            }
                        } else {
                            char sTitle[100];
                            cout << "Enter query phrase matching targets: ";
                            cin >> ws;
                            cin.getline(sTitle, 100);
                            Movie* match = searchMovie(movieRepositoryHead, sTitle);
                            if (match != NULL) {
                                cout << "\n[MATCH RETRIEVED]:" << endl;
                                printMovieSpecs(*match);
                            } else {
                                cout << "No items match criteria string pattern in data repository fields." << endl;
                            }
                        }
                    }
                    else if (subChoice == 6) {
                        displayAllMovies(movieRepositoryHead);
                    }
                    else if (subChoice == 7) {
                        ofstream outReport("admin_report.txt");
                        if (outReport.is_open()) {
                            outReport << "=== ADMIN SUMMARY REPORT: MOVIE REPOSITORY HIGHLIGHTS ===\n";
                            outReport << "Total Movies: " << getMovieCount(movieRepositoryHead) << "\n\n";
                            Movie* temp = movieRepositoryHead;
                            while (temp != NULL) {
                                outReport << "ID: " << temp->id << " | " << temp->title << " [" << temp->year << "] - Rating: " << temp->rating << "\n";
                                temp = temp->next;
                            }
                            outReport.close();
                            cout << "Summary data successfully compiled and saved to 'admin_report.txt'!" << endl;
                        } else {
                            cout << "Failed to generate file report output." << endl;
                        }
                    }
                    else {
                        cout << "Invalid option!\n";
                    }
                }
            } else {
                cout << "Access denied. Verification failure encountered." << endl;
            }
        }
        else if (coreChoice == 2) {
            char user[50], pass[50];
            cout << "Enter Customer Username: ";
            cin >> user;
            cout << "Enter Customer Password Entry: ";
            cin >> pass;

            if (customerEngine.customerLogin(user, pass)) {
                logEngine.logEvent("Customer portfolio profile checked and verified successfully.");
                showCustomerRewards(customerEngine);  
                recordLoginHistory(user, "customer"); 
                int subChoice = 0;
                while (customerEngine.getLoginStatus()) {
                    customerEngine.displayMenu();
                    cout << "Select Customer feature pathway: ";
                    subChoice = safeReadInt(NULL);

                    if (subChoice == 9) {
                        customerEngine.logout();
                        cout << "Customer session cleanly flushed and finalized." << endl;
                    }
                    else if (subChoice == 10) {
                        writeReviewFlow(movieRepositoryHead, user);
                    }
                    else if (subChoice == 11) {
                        displayReviewsByUser(user);
                    }
                    else if (subChoice == 12) {
                        int mid;
                        cout << "Enter Movie ID to see its reviews: ";
                        cin >> mid;
                        displayReviewsForMovie(mid);
                        cout << "Average score: " << averageReviewScore(mid) << "/10\n";
                    }
                    else if (subChoice == 13) {
                        recommendByGenre(movieRepositoryHead, user);
                    }
                    else if (subChoice == 14) {
                        editReviewFlow(user);
                    }
                    else if (subChoice == 15) {
                        deleteReviewFlow(user);
                    }
                    else if (subChoice == 16) {
                        favoritesMenu(movieRepositoryHead, user);
                    }
                    else if (subChoice == 17) {
                        displayRecentViews(movieRepositoryHead, user);
                    }
                    else if (subChoice == 18) {
                        showLoyaltySummary(user);
                    }
                    else if (subChoice == 19) {
                        castExploreMenu(movieRepositoryHead);
                    }
                    else if (subChoice == 20) {
                        showCustomerDashboard(user);
                    }
                    else if (subChoice == 1) {
                        browseMoviesForCustomer(movieRepositoryHead, user);
                    }
                    else if (subChoice == 2) {
                        cout << "Search by:\n1. ID\n2. Title\nEnter choice: ";
                        int searchChoice;
                        searchChoice = safeReadInt(NULL);
                        if (searchChoice == 1) {
                            int id;
                            cout << "Enter ID: ";
                            cin >> id;
                            Movie* match = binarySearchMovie(movieRepositoryHead, id);  
                            if (match != NULL) {
                                printMovieSpecs(*match);
                                recordRecentView(user, match->id);   
                            } else {
                                cout << "Movie not found!\n";
                            }
                        } else {
                            char sTitle[100];
                            cout << "Search movie recommendations by keywords: ";
                            cin >> ws;
                            cin.getline(sTitle, 100);
                            Movie* match = searchMovie(movieRepositoryHead, sTitle);
                            if (match != NULL) {
                                cout << "\n[RECOMMENDED MATCH FOUND]:" << endl;
                                printMovieSpecs(*match);
                            } else {
                                cout << "No personalized suggestion matched that keyword phrase pattern." << endl;
                            }
                        }
                    }
                    else if (subChoice == 3) {
                        int crit;
                        cout << "Select Manual Sorting Criteria (1: High-to-Low Rating, 2: Chronological Year): ";
                        crit = safeReadInt(NULL);
                        bubbleSortMovies(movieRepositoryHead, crit);
                        saveMoviesToFile(movieRepositoryHead);
                        cout << "Movie selection updated via manual processing logic." << endl;
                    }
                    else if (subChoice == 4) {
                        int movieId;
                        char status[30];
                        cout << "Available Movies:\n";
                        displayAllMovies(movieRepositoryHead);
                        cout << "Enter Movie ID to add to watchlist: ";
                        cin >> movieId;
                        if (searchMovie(movieRepositoryHead, movieId) != NULL) {
                            cout << "Enter status (Plan to Watch/Completed/Watching): ";
                            cin >> ws;
                            cin.getline(status, 30);
                            addToWatchlist(user, movieId, status);
                            saveWatchlistToFile();
                            cout << "Added to watchlist!\n";
                        } else {
                            cout << "Movie ID not found!\n";
                        }
                    }
                    else if (subChoice == 5) {
                        int movieId;
                        char newStatus[30];
                        displayWatchlist(user);
                        cout << "Enter Movie ID to edit status: ";
                        cin >> movieId;
                        cout << "Enter new status (Plan to Watch/Completed/Watching): ";
                        cin >> ws;
                        cin.getline(newStatus, 30);
                        if (editWatchlistStatus(user, movieId, newStatus)) {
                            saveWatchlistToFile();
                            cout << "Status updated!\n";
                        } else {
                            cout << "Movie not found in watchlist!\n";
                        }
                    }
                    else if (subChoice == 6) {
                        int movieId;
                        displayWatchlist(user);
                        cout << "Enter Movie ID to delete from watchlist: ";
                        cin >> movieId;
                        if (deleteFromWatchlist(user, movieId)) {
                            saveWatchlistToFile();
                            cout << "Removed from watchlist!\n";
                        } else {
                            cout << "Movie not found in watchlist!\n";
                        }
                    }
                    else if (subChoice == 7) {
                        displayWatchlist(user);
                    }
                    else if (subChoice == 8) {
                        ofstream custReport("watchlist_report.txt");
                        if (custReport.is_open()) {
                            custReport << "=== CUSTOMER WATCHLIST REPORT ===\n";
                            custReport << "User: " << customerEngine.getLoggedUser() << "\n\n";
                            WatchlistNode* temp = watchlistHead;
                            bool hasItems = false;
                            while (temp != NULL) {
                                if (customStrCmp(temp->username, user)) {
                                    custReport << "Movie ID: " << temp->movieId << " | Status: " << temp->status << "\n";
                                    hasItems = true;
                                }
                                temp = temp->next;
                            }
                            if (!hasItems) {
                                custReport << "No movies in watchlist.\n";
                            }
                            custReport.close();
                            cout << "Watchlist report exported successfully to 'watchlist_report.txt'!" << endl;                       
                            ifstream reportFile("watchlist_report.txt");
                            if (reportFile.is_open()) {
                                char line[200];
                                while (reportFile.getline(line, 200)) {
                                    cout << line << endl;
                                }
                                reportFile.close();
                            }
                        } else {
                            cout << "Unable to export user report profile details." << endl;
                        }
                    }
                    else {
                        cout << "Invalid option!\n";
                    }
                }
            } else {
                cout << "Invalid customer log tracking data. Verify credentials or try again." << endl;
            }
        }
    }

    clearMoviesList(movieRepositoryHead);
    clearReviews();   
    clearFavorites();
    clearRecent();
    clearCast();
    return 0;
}
