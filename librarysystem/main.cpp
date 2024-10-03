#include <iostream>
#include <string>
#include <ctime>
#include "sqlite3.h"


using namespace std;

const int DUE_DAYS = 7;
const double FINE_PER_DAY = 1.00;

struct Book {
    int id;
    string title;
    string genre;
    bool isBorrowed;
    time_t dueDate;
};

struct Student {
    int id;
    string name;
    string email;
    string password;
    string borrowedBook;
    double fine;
    bool studyRoomReserved;
    time_t borrowDate;
};

struct Librarian {
    int id;
    string name;
    string email;
    string password;
};

struct Admin {
    int id;
    string name;
    string email;
    string password;
};

// Node structure for the linked list
struct Node {
    Book book;
    Node* next;
};

// Linked List class for managing books
class LinkedList {
private:
    Node* head;

public:
    LinkedList() : head(nullptr) {}

    // Add a book to the list
    void addBook(const Book& book) {
        Node* newNode = new Node{book, nullptr};
        if (head == nullptr) {
            head = newNode;
        } else {
            Node* temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    // Remove a book by ID
    void removeBook(int id) {
        if (head == nullptr) return;

        if (head->book.id == id) {
            Node* temp = head;
            head = head->next;
            delete temp;
            return;
        }

        Node* temp = head;
        while (temp->next != nullptr && temp->next->book.id != id) {
            temp = temp->next;
        }

        if (temp->next != nullptr) {
            Node* toDelete = temp->next;
            temp->next = temp->next->next;
            delete toDelete;
        }
    }

    // Display all books
    void displayBooks(sqlite3 *DB) {
    string sql = "SELECT * FROM BOOK;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        cout << "List of all books:\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *title = sqlite3_column_text(stmt, 1);
            const unsigned char *genre = sqlite3_column_text(stmt, 2);
            int isBorrowed = sqlite3_column_int(stmt, 3);

            cout << "ID: " << id << ", Title: " << title << ", Genre: " << genre;
            cout << ", Status: " << (isBorrowed ? "Borrowed" : "Available") << endl;
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Failed to retrieve books from the database." << endl;
    }
}


    // Find a book by ID
    Book* findBook(int id) {
        Node* temp = head;
        while (temp != nullptr) {
            if (temp->book.id == id) {
                return &(temp->book);
            }
            temp = temp->next;
        }
        return nullptr;
    }
};

// SQLite3 helper functions
void executeSQL(const string &sql, sqlite3 *DB) {
    char *messageError;
    int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

    if (exit != SQLITE_OK) {
        cerr << "SQL error: " << messageError << endl;
        sqlite3_free(messageError);
    } else {
        cout << "SQL executed successfully!" << endl;
    }
}

void initializeDatabase(sqlite3 *&DB) {
    int exit = sqlite3_open("library.db", &DB);

    if (exit) {
        cerr << "Error open DB " << sqlite3_errmsg(DB) << endl;
        return;
    } else {
        cout << "Opened database successfully!" << endl;
    }

    string createBookTable = "CREATE TABLE IF NOT EXISTS BOOK("
                             "ID INT PRIMARY KEY NOT NULL, "
                             "TITLE TEXT NOT NULL, "
                             "GENRE TEXT NOT NULL, "
                             "ISBORROWED INT NOT NULL, "
                             "DUEDATE INT );";

    string createStudentTable = "CREATE TABLE IF NOT EXISTS STUDENT("
                                "ID INT PRIMARY KEY NOT NULL, "
                                "NAME TEXT NOT NULL, "
                                "EMAIL TEXT NOT NULL, "
                                "PASSWORD TEXT NOT NULL,"
                                "BORROWEDBOOK TEXT, "
                                "FINE REAL NOT NULL, "
                                "STUDYROOMRESERVED INT NOT NULL, "
                                "BORROWDATE INT );";

    string createLibrarianTable ="CREATE TABLE IF NOT EXISTS LIBRARIAN("
                                 "ID INT PRIMARY KEY NOT NULL, "
                                 "NAME TEXT NOT NULL, "
                                 "EMAIL TEXT NOT NULL, "
                                 "PASSWORD TEXT NOT NULL ); ";

    string createAdminTable ="CREATE TABLE IF NOT EXISTS ADMIN("
                             "ID INT PRIMARY KEY NOT NULL,"
                             "NAME TEXT NOT NULL, "
                             "EMAIL TEXT NOT NULL, "
                             "PASSWORD TEXT NOT NULL);";


    executeSQL(createBookTable, DB);
    executeSQL(createStudentTable, DB);
    executeSQL(createLibrarianTable, DB);
    executeSQL(createAdminTable, DB);
}

// Insert initial data into the library
void insertInitialData(sqlite3 *DB) {
    string sql = "INSERT INTO BOOK (ID, TITLE, GENRE, ISBORROWED, DUEDATE) VALUES"
                 "(1, 'The Great Gatsby', 'Fiction', 0, 0),"
                 "(2, 'To Kill a Mockingbird', 'Fiction', 0, 0),"
                 "(3, '1984', 'Science Fiction', 0, 0),"
                 "(4, 'Pride and Prejudice', 'Romance', 0, 0),"
                 "(5, 'The Catcher in the Rye', 'Fiction', 0, 0),"
                 "(6, 'The Hobbit', 'Fantasy', 0, 0),"
                 "(7, 'Moby-Dick', 'Adventure', 0, 0),"
                 "(8, 'War and Peace', 'Historical', 0, 0),"
                 "(9, 'Brave New World', 'Science Fiction', 0, 0),"
                 "(10, 'Wuthering Heights', 'Gothic', 0, 0),"
                 "(11, 'Jurassic Park', 'Adventure', 0, 0),"
                 "(12, 'Harry Potter', 'Fantasy', 0, 0),"
                 "(13, 'Game of Thrones', 'Fantasy', 0, 0),"
                 "(14, 'The Book Thief', 'Historical', 0, 0),"
                 "(15, 'Jane Eyre', 'Gothic', 0, 0);";

    string studentSQL = "INSERT INTO STUDENT (ID, NAME, EMAIL, PASSWORD, BORROWEDBOOK, FINE, STUDYROOMRESERVED, BORROWDATE) VALUES"
                        "(701, 'John Doe', 'johndoe@gmail.com', '12345', '', 0.0, 0, 0),"
                        "(702, 'Kwan Zi Cheng', 'kwan123@gmail.com', '12345', '',0.0, 0, 0);";

    string librarianSQL = "INSERT INTO LIBRARIAN (ID, NAME, EMAIL, PASSWORD) VALUES"
                          "(801, 'Alex', 'alex@gmail.com', '12345');";

    string adminSQL ="INSERT INTO ADMIN (ID, NAME, EMAIL, PASSWORD) VALUES"
                     "(601, 'Jack', 'jack@gmail.com', '12345');";

    executeSQL(sql, DB);
    executeSQL(studentSQL, DB);
    executeSQL(librarianSQL, DB);
    executeSQL(adminSQL,DB);
}

void displayUserIDs(sqlite3 *DB, Student &student) {
    string studentSQL = "SELECT ID, NAME FROM STUDENT;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(DB, studentSQL.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        cout << "Available User IDs:" << endl;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *name = sqlite3_column_text(stmt, 1);
            cout << "ID: " << id << ", Name: " << name << endl;
        }
    } else {
        cerr << "Failed to retrieve user IDs." << endl;
    }

    sqlite3_finalize(stmt);
}

void displayUserIDslibrarian(sqlite3 *DB, Librarian &librarian) {
    string librarianSQL = "SELECT ID, NAME FROM LIBRARIAN;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(DB, librarianSQL.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        cout << "Available User IDs:" << endl;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *name = sqlite3_column_text(stmt, 1);
            cout << "ID: " << id << ", Name: " << name << endl;
        }
    } else {
        cerr << "Failed to retrieve user IDs." << endl;
    }

    sqlite3_finalize(stmt);
}

void displayUserIDsadmin(sqlite3 *DB, Admin &admin) {
    string adminSQL = "SELECT ID, NAME FROM ADMIN;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(DB, adminSQL.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        cout << "Available User IDs:" << endl;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *name = sqlite3_column_text(stmt, 1);
            cout << "ID: " << id << ", Name: " << name << endl;
        }
    } else {
        cerr << "Failed to retrieve user IDs." << endl;
    }

    sqlite3_finalize(stmt);
}

bool login(sqlite3 *DB, Student &student, LinkedList &books) {
    string id,password;

    sqlite3_stmt *stmt;
    displayUserIDs(DB, student);
    cout << "Enter Student ID: ";
    cin >> id;
    cout << "Enter Password: ";
    cin >> password;

    
    // SQL query to check for matching student_id and password
    string sql = "SELECT NAME FROM STUDENT WHERE ID = '" + (id) + "' AND PASSWORD = '" + (password) + "';";


    
    int rc = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        cout << "Failed to execute query: " << sqlite3_errmsg(DB) << endl;
        return false;
    }

    // If a record is found, login is successful
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        cout << "Login successful. Welcome, " << student.name << "!" << endl;
        sqlite3_finalize(stmt);
        return true;
    } else {
        cout << "Login failed. Invalid Student ID or Password." << endl;
        sqlite3_finalize(stmt);
        return false;
    }
}

bool loginlibrarian(sqlite3 *DB, Librarian &librarian, LinkedList &books) {
    string id,password;

    sqlite3_stmt *stmt;
    displayUserIDslibrarian(DB, librarian);
    cout << "Enter librarian ID: ";
    cin >> id;
    cout << "Enter Password: ";
    cin >> password;

    
    // SQL query to check for matching student_id and password
    string sql = "SELECT NAME FROM LIBRARIAN WHERE ID = '" + (id) + "' AND PASSWORD = '" + (password) + "';";


    
    int rc = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        cout << "Failed to execute query: " << sqlite3_errmsg(DB) << endl;
        return false;
    }

    // If a record is found, login is successful
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        cout << "Login successful. Welcome, " << librarian.name << "!" << endl;
        sqlite3_finalize(stmt);
        return true;
    } else {
        cout << "Login failed. Invalid LIBRARIAN ID or Password." << endl;
        sqlite3_finalize(stmt);
        return false;
    }
}
bool loginAdmin(sqlite3 *DB, Admin &admin, LinkedList &books) {
    string id,password;

    sqlite3_stmt *stmt;
    displayUserIDsadmin(DB, admin);
    cout << "Enter Admin ID: ";
    cin >> id;
    cout << "Enter Password: ";
    cin >> password;

    
    // SQL query to check for matching student_id and password
    string sql = "SELECT NAME FROM ADMIN WHERE ID = '" + (id) + "' AND PASSWORD = '" + (password) + "';";


    
    int rc = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        cout << "Failed to execute query: " << sqlite3_errmsg(DB) << endl;
        return false;
    }

    // If a record is found, login is successful
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        cout << "Login successful. Welcome, " << admin.name << "!" << endl;
        sqlite3_finalize(stmt);
        return true;
    } else {
        cout << "Login failed. Invalid Admin ID or Password." << endl;
        sqlite3_finalize(stmt);
        return false;
    }
}

// Add a new book
void addBook(sqlite3 *DB) {
    cout << "Enter book ID: ";
    int id;
    cin >> id;
    cin.ignore(); // Clear newline character from input buffer

    cout << "Enter book title: ";
    string title;
    getline(cin, title);

    cout << "Enter book genre: ";
    string genre;
    getline(cin, genre);

    string sql = "INSERT INTO BOOK (ID, TITLE, GENRE, ISBORROWED, DUEDATE) VALUES ("
                 + to_string(id) + ", '" + title + "', '" + genre + "', 0, 0);";
    executeSQL(sql, DB);
    cout << "Book added successfully." << endl;
}

// Remove a book
void removeBook(sqlite3 *DB) {
    cout << "Enter book ID to remove: ";
    int id;
    cin >> id;

    string sql = "DELETE FROM BOOK WHERE ID = " + to_string(id) + ";";
    executeSQL(sql, DB);
    cout << "Book removed successfully." << endl;
}

// Update book details
void updateBook(sqlite3 *DB) {
    cout << "Enter book ID to update: ";
    int id;
    cin >> id;
    cin.ignore(); // Clear newline character from input buffer

    cout << "Enter new title (or leave blank to keep current): ";
    string title;
    getline(cin, title);

    cout << "Enter new genre (or leave blank to keep current): ";
    string genre;
    getline(cin, genre);

    // Start constructing the SQL query
    string sql = "UPDATE BOOK SET ";
    bool firstField = true; // To handle comma placement

    if (!title.empty()) {
        sql += "TITLE = '" + title + "'";
        firstField = false;
    }
    if (!genre.empty()) {
        if (!firstField) {
            sql += ", ";
        }
        sql += "GENRE = '" + genre + "'";
    }

    // Add the WHERE clause to update the correct book
    sql += " WHERE ID = " + to_string(id) + ";";

    // Execute the SQL statement
    executeSQL(sql, DB);
    cout << "Book updated successfully." << endl;
}


// Issue a book to a student
void issueBook(sqlite3 *DB) {
    cout << "Enter student ID: ";
    int studentId;
    cin >> studentId;
    cin.ignore(); // Clear newline character from input buffer

    cout << "Enter book title to issue: ";
    string title;
    getline(cin, title);

    string sql = "SELECT ID FROM BOOK WHERE TITLE = '" + title + "' AND ISBORROWED = 0;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0) == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        int bookID = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);

        time_t now;
        time(&now);
        time_t dueDate = now + DUE_DAYS * 24 * 60 * 60;

        sql = "UPDATE BOOK SET ISBORROWED = 1, DUEDATE = " + to_string(dueDate) + " WHERE ID = " + to_string(bookID) + ";";
        executeSQL(sql, DB);

        sql = "UPDATE STUDENT SET BORROWEDBOOK = '" + title + "', BORROWDATE = " + to_string(now) + " WHERE ID = " + to_string(studentId) + ";";
        executeSQL(sql, DB);

        cout << "Book issued to student successfully. Due by " << ctime(&dueDate);
    } else {
        cout << "Book not available." << endl;
    }
}
// List available books by genre
void displayBooksByGenre(sqlite3 *DB, const string &genre) {
    string sql = "SELECT * FROM BOOK WHERE GENRE = '" + genre + "' AND ISBORROWED = 0;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        cout << "Books available in " << genre << " genre:" << endl;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << " - " << sqlite3_column_text(stmt, 1) << endl;
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Failed to fetch books by genre." << endl;
    }
}

// Borrow a book
void borrowBook(sqlite3 *DB, Student &student) {
    cout << "Enter the genre of the book you want to borrow: \n";
    cout << "Fiction\n";
    cout << "Science Fiction\n";
    cout << "Romance\n";
    cout << "Historical\n";
    cout << "Fantasy\n";
    cout << "Adventure\n";
    cout << "Gothic\n";
    string genre;
    cin >> genre;

    displayBooksByGenre(DB, genre);

    cout << "Enter the title of the book you want to borrow: ";
    string title;
    cin.ignore();
    getline(cin, title);

    string sql = "SELECT * FROM BOOK WHERE TITLE = '" + title + "' AND ISBORROWED = 0;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0) == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        int bookID = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        
        time(&student.borrowDate);
        time_t dueDate = student.borrowDate + DUE_DAYS * 24 * 60 * 60;
        
        sql = "UPDATE BOOK SET ISBORROWED = 1, DUEDATE = " + to_string(dueDate) + " WHERE ID = " + to_string(bookID) + ";";
        executeSQL(sql, DB);
        
        student.borrowedBook = title;
        sql = "UPDATE STUDENT SET BORROWEDBOOK = '" + title + "', BORROWDATE = " + to_string(student.borrowDate) + " WHERE ID = " + to_string(student.id) + ";";
        executeSQL(sql, DB);
        
        cout << "You have borrowed \"" << title << "\". Return it by " << ctime(&dueDate);
    } else {
        cout << "Book not found or already borrowed." << endl;
    }
}

// Return a borrowed book and calculate fine
void returnBook(sqlite3 *DB, Student &student) {
    if (student.borrowedBook.empty()) {
        cout << "You haven't borrowed any books." << endl;
        return;
    }

    string sql = "SELECT DUEDATE FROM BOOK WHERE TITLE = '" + student.borrowedBook + "';";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0) == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        time_t dueDate = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        
        time_t now;
        time(&now);
        double overdueDays = difftime(now, dueDate) / (60 * 60 * 24);
        
        if (overdueDays > 0) {
            student.fine += overdueDays * FINE_PER_DAY;
            cout << "You have an overdue fine of $" << overdueDays * FINE_PER_DAY << " for late return." << endl;
        }
        
        sql = "UPDATE BOOK SET ISBORROWED = 0, DUEDATE = 0 WHERE TITLE = '" + student.borrowedBook + "';";
        executeSQL(sql, DB);
        
        student.borrowedBook = "";
        sql = "UPDATE STUDENT SET BORROWEDBOOK = '', FINE = " + to_string(student.fine) + " WHERE ID = " + to_string(student.id) + ";";
        executeSQL(sql, DB);
        
        cout << "You have returned the book." << endl;
    } else {
        cout << "Error returning the book." << endl;
    }
}

// Reserve a study room
void reserveStudyRoom(Student &student, sqlite3 *DB) {
    if (student.studyRoomReserved) {
        cout << "You have already reserved a study room." << endl;
    } else {
        student.studyRoomReserved = true;
        string sql = "UPDATE STUDENT SET STUDYROOMRESERVED = 1 WHERE ID = " + to_string(student.id) + ";";
        executeSQL(sql, DB);
        cout << "Study room reserved successfully." << endl;
    }
}

// Update personal information
void updatePersonalInformation(Student &student, sqlite3 *DB) {
    cout << "Enter new name: ";
    cin.ignore();
    getline(cin, student.name);
    cout << "Enter new email: ";
    getline(cin, student.email);
    cout << "Enter new password: ";
    getline(cin, student.password);

    string studentSQL = "UPDATE STUDENT SET NAME = '" + student.name + "', EMAIL = '" + student.email + "', PASSWORD = '" + student.password + "' WHERE ID = " + to_string(student.id) + ";";
    executeSQL(studentSQL, DB);

    cout << "Personal information updated." << endl;
}

// View account information
void viewAccountInformation(const Student &student, sqlite3 *DB) {
    string sql = "SELECT * FROM STUDENT WHERE ID = " + to_string(student.id) + ";";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0) == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        cout << "Account Information:" << endl;
        cout << "Name: " << sqlite3_column_text(stmt, 1) << endl;
        cout << "Email: " << sqlite3_column_text(stmt, 2) << endl;
        cout << "Password: " << sqlite3_column_text(stmt, 3) << endl;
        cout << "Borrowed Book: " << (sqlite3_column_text(stmt, 4) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)) : "None") << endl;
        cout << "Fine: $" << sqlite3_column_double(stmt, 5) << endl;
        cout << "Study Room Reserved: " << (sqlite3_column_int(stmt, 6) ? "Yes" : "No") << endl;
        sqlite3_finalize(stmt);
    } else {
        cout << "Error fetching account information." << endl;
    }
}

// Add a new student
void addStudent(sqlite3 *DB, Student &student) {
    cout << "Enter student ID: ";
    int id;
    cin >> student.id;
    cin.ignore(); // Clear newline character from input buffer

    cout << "Enter student name: ";
    string name;
    getline(cin, student.name);

    cout << "Enter student email: ";
    string email;
    getline(cin, student.email);

    cout << "Enter student password: ";
    string password;
    getline(cin, student.password);

    string studentSQL = "INSERT INTO STUDENT (ID, NAME, EMAIL, PASSWORD, BORROWEDBOOK, FINE, STUDYROOMRESERVED, BORROWDATE) VALUES ("
                 + to_string(student.id) + ", '" + student.name + "', '" + student.email + "', '" + student.password + "', '', 0.0, 0, 0);";
    executeSQL(studentSQL, DB);
    cout << "Student added successfully." << endl;
}

void removeStudent(sqlite3 *DB, Student &student) {
    cout << "Enter Student ID to remove: ";
    int id;
    cin >> student.id;

    string studentSQL = "DELETE FROM STUDENT WHERE ID = " + to_string(student.id) + ";";
    executeSQL(studentSQL, DB);
    cout << "Student removed successfully." << endl;
}

void addLibrarian(sqlite3 *DB, Librarian &librarian) {
    cout << "Enter librarian ID: ";
    int id;
    cin >> librarian.id;
    cin.ignore(); // Clear newline character from input buffer

    cout << "Enter librarian name: ";
    string name;
    getline(cin, librarian.name);

    cout << "Enter librarian email: ";
    string email;
    getline(cin, librarian.email);

    cout << "Enter librarian password: ";
    string password;
    getline(cin, librarian.password);

    string librarianSQL = "INSERT INTO LIBRARIAN (ID, NAME, EMAIL, PASSWORD) VALUES ("
                 + to_string(librarian.id) + ", '" + librarian.name + "', '" + librarian.email + "', '" + librarian.password + " ');";
    executeSQL(librarianSQL, DB);
    cout << "Librarian added successfully." << endl;
}

void removeLibrarian(sqlite3 *DB, Librarian &librarian) {
    cout << "Enter Librarian ID to remove: ";
    int id;
    cin >> librarian.id;

    string librarianSQL = "DELETE FROM LIBRARIAN WHERE ID = " + to_string(librarian.id) + ";";
    executeSQL(librarianSQL, DB);
    cout << "Librarian removed successfully." << endl;
}

// Review student information
void reviewStudentInfo(sqlite3 *DB, Student &student) {
    displayUserIDs(DB, student);
    cout << "Enter student ID to review: ";
    int id;
    cin >> student.id;

    string sql = "SELECT * FROM STUDENT WHERE ID = " + to_string(student.id) + ";";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0) == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        cout << "Student Information:" << endl;
        cout << "Name: " << sqlite3_column_text(stmt, 1) << endl;
        cout << "Email: " << sqlite3_column_text(stmt, 2) << endl;
        cout << "Borrowed Book: " << (sqlite3_column_text(stmt, 4) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)) : "None") << endl;
        cout << "Fine: $" << sqlite3_column_double(stmt, 5) << endl;
        cout << "Study Room Reserved: " << (sqlite3_column_int(stmt, 6) ? "Yes" : "No") << endl;
        sqlite3_finalize(stmt);
    } else {
        cout << "Error fetching student information." << endl;
    }
}

// Add delay record for late return
void addDelayRecord(sqlite3 *DB) {
    cout << "Enter student ID: ";
    int studentId;
    cin >> studentId;

    cout << "Enter book title: ";
    string title;
    cin.ignore();
    getline(cin, title);

    string sql = "SELECT DUEDATE FROM BOOK WHERE TITLE = '" + title + "';";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0) == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        time_t dueDate = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);

        time_t now;
        time(&now);
        double overdueDays = difftime(now, dueDate) / (60 * 60 * 24);
        if (overdueDays > 0) {
            string delaySQL = "UPDATE STUDENT SET FINE = FINE + " + to_string(overdueDays * FINE_PER_DAY) + " WHERE ID = " + to_string(studentId) + ";";
            executeSQL(delaySQL, DB);
            cout << "Delay record added. Fine updated." << endl;
        } else {
            cout << "No delay to record." << endl;
        }
    } else {
        cout << "Error fetching book due date." << endl;
    }
}

// Main librarian menu
void librarianMenu(sqlite3 *DB, LinkedList &books, Student &student) {
    while (true) {
        cout << "\nLibrarian Menu:\n";
        cout << "1. Add Book\n";
        cout << "2. Remove Book\n";
        cout << "3. Update Book\n";
        cout << "4. Issue Book\n";
        cout << "5. Shows all book\n";
        cout << "6. Add Student\n";
        cout << "7. Remove Student\n";
        cout << "8. Review Student Info\n";
        cout << "9. Add Delay Record\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                addBook(DB);
                break;
            case 2:
                removeBook(DB);
                break;
            case 3:
                updateBook(DB);
                break;
            case 4:
                issueBook(DB);
                break;
            case 5:
                books.displayBooks(DB);
                break;
            case 6:
                addStudent(DB, student);
                break;
            case 7:
                removeStudent(DB, student);
                break;
            case 8:
                reviewStudentInfo(DB, student);
                break;
            case 9:
                addDelayRecord(DB);
                break;
            case 0:
                cout << "Exiting Librarian Menu." << endl;
                return;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
}

// Main student menu
void studentMenu(Student &student, sqlite3 *DB, LinkedList &books) {
    while (true) {
        cout << "\nStudent Menu:\n";
        cout << "1. Borrow a Book\n";
        cout << "2. Return a Book\n";
        cout << "3. Shows all Books\n";
        cout << "4. Reserve a Study Room\n";
        cout << "5. Update Personal Information\n";
        cout << "6. View Account Information\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                borrowBook(DB, student);
                break;
            case 2:
                returnBook(DB, student);
                break;
            case 3:
                books.displayBooks(DB);
                break;
            case 4:
                reserveStudyRoom(student, DB);
                break;
            case 5:
                updatePersonalInformation(student, DB);
                break;
            case 6:
                viewAccountInformation(student, DB);
                break;
            case 7:
                cout << "Goodbye!" << endl;
                return;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
}

void adminMenu(sqlite3 *DB, LinkedList &books, Student &student, Librarian &librarian) {
    int choice;
    while (true) {
        cout << "Admin Menu:" << endl;
        cout << "1. Add Book" << endl;
        cout << "2. Remove Book" << endl;
        cout << "3. Update Book" << endl;
        cout << "4. Issue Book" << endl;
        cout << "5. Return Book" << endl;
        cout << "6. Add Student" << endl;
        cout << "7. Remove Student" << endl;
        cout << "8. Create Librarian" << endl;
        cout << "9. Remove Librarian" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(); // Clear newline character from input buffer

        switch (choice) {
            case 1:
                addBook(DB);
                break;
            case 2:
                removeBook(DB);
                break;
            case 3:
                updateBook(DB);
                break;
            case 4:
                issueBook(DB);
                break;
            case 5: {
                Student student;
                cout << "Enter student ID: ";
                cin >> student.id;
                cin.ignore(); // Clear newline character from input buffer
                // Fetch student details from DB for returning book
                string sql = "SELECT BORROWEDBOOK FROM STUDENT WHERE ID = " + to_string(student.id) + ";";
                sqlite3_stmt *stmt;
                if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0) == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
                    student.borrowedBook = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                    sqlite3_finalize(stmt);
                    returnBook(DB, student);
                } else {
                    cout << "Student not found." << endl;
                }
                break;
            }
            case 6:
                addStudent(DB, student);
                break;
            case 7:
                removeStudent(DB, student);
                break;
            case 8:
                addLibrarian(DB, librarian);
                break;
            case 9:
                removeLibrarian(DB, librarian);
                break;
            case 0:
                cout << "Exiting Admin Menu." << endl;
                return;
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    }
}


// Main menu handler
void handleMenuOption(int option, sqlite3 *DB, Student &student, LinkedList &books, Librarian &librarian, Admin &admin) {
    switch (option) {
        case 1:
            if (login(DB, student, books) == true){
            studentMenu(student,DB ,books);
            } else if(login(DB, student, books) == false) {
                handleMenuOption(option, DB, student, books, librarian, admin);
            } else{

            }
            break;
        case 2:
        if (loginlibrarian(DB, librarian, books) == true){
            librarianMenu(DB, books, student);
            } else if(loginlibrarian(DB, librarian, books) == false) {
                handleMenuOption(option, DB, student, books, librarian, admin);
            } else {

            }
            break;
        case 3:
        if (loginAdmin(DB, admin, books) == true){
            adminMenu(DB, books, student, librarian);
            } else if(loginAdmin(DB, admin, books) == false){
                handleMenuOption(option, DB, student, books, librarian, admin);
            } else {

            }
            
            break;
        case 4:
            cout << "Exiting program." << endl;
            exit(0); // Exit the program or handle as needed
            break;
        default:
            cout << "Invalid option. Please try again." << endl;
    }
}

int main() {
    sqlite3 *DB;
    LinkedList books;
    initializeDatabase(DB);
    insertInitialData(DB);

    Student student = {701, "John Doe", "johndoe@gmail.com", "12345", "", 0.0, false, 0};
    Librarian librarian ={801, "Alex", "alex@gmail.com", "12345"};
    Admin admin ={601, "Jack", "jack@gmail.com", "12345"};

    sqlite3_stmt *stmt;
    string sql = "SELECT * FROM BOOK;";
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Book book;
            book.id = sqlite3_column_int(stmt, 0);
            book.title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            book.genre = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            book.isBorrowed = sqlite3_column_int(stmt, 3);
            book.dueDate = sqlite3_column_int(stmt, 4);

            books.addBook(book);
        }
    }

    while (true) {
        cout << "\nMain Menu:\n";
        cout << "1. Student Menu\n";
        cout << "2. Librarian Menu\n";
        cout << "3. Admin Menu\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;
        handleMenuOption(choice, DB, student, books, librarian, admin);
     
    }
    
   


    sqlite3_close(DB);
    return 0;
}
