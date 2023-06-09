#include <fstream>
#include <string>
#include <sstream>
#include <cctype>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <stdio.h>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace std;
// Kommentare bleiben erstmal drin für die überschaubarkeit
// code wird im laufe des Prozesses verbessert/optimiert
// 

// Teilen eines Strings anhand eines Trennzeichens
vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool checkCSV(const string& filename) {
    ifstream file(filename);
    if (file) {
        file.close();
        return true;
    }

    // Datei wird erstellt wenn sie nicht existiert
    ofstream out_file(filename);
    if (!out_file) {
        cout << "Fehler beim Erstellen der Datei." << endl;
        return false;
    }

    out_file.close();
    return true;
}

// Überprüfen, ob ein Wert in einer CSV-Datei vorhanden ist
bool isValueInCSV(const string& filename, const string& value) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    string line;
    while (getline(file, line)) {
        vector<string> values = split(line, ',');
        if (find(values.begin(), values.end(), value) != values.end()) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

// Hier wird Uhrzeit und Datum erstellt
string getCurrentDateTime() {
    auto now = chrono::system_clock::now();
    time_t now_time_t = chrono::system_clock::to_time_t(now);
    tm date_time;
    localtime_s(&date_time, &now_time_t);

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &date_time);

    return string(buffer);
}

// Hinzufügen eines Werts CSV-Datei
void addToCSV(const string& filename, const string& value) {
    ofstream file(filename, ios::app);
    if (!file.is_open()) {
        cout << "Fehler beim Öffnen der Datei." << endl;
        return;
    }

    file << value << endl;
    file.close();
}

// löschen eines Werts CSV-Datei
void removeToCSV(const string& filename, const string& value) {
    ofstream file(filename, ios::app);
    if (!file.is_open()) {
        cout << "Fehler beim Öffnen der Datei." << endl;
        return;
    }

    file << "" << endl;
    file.close();
}

void removeValueFromCSV(const string& filename, const string& value) {
    ifstream file(filename);
    ofstream temp("temp.csv");

    if (!file.is_open() || !temp.is_open()) {
        cout << "Fehler beim Öffnen der Datei." << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        vector<string> tokens;
        string token;

        while (getline(iss, token, ',')) {
            tokens.push_back(token);
        }

        if (find(tokens.begin(), tokens.end(), value) == tokens.end()) {
            temp << line << "\n";
        }
    }

    file.close();
    temp.close();

    remove(filename.c_str());

    int result = rename("temp.csv", filename.c_str());
    if (result != 0) {
        cout << "Fehler beim Umbenennen der Datei." << endl;
    }
}

void transformLower(string& userInput) {
    transform(userInput.begin(), userInput.end(), userInput.begin(),
        [](unsigned char c) { return tolower(c); });
}

void capitalizeFirstLetter(string& str) {
    if (!str.empty()) {
        str[0] = toupper(str[0]);
    }
}

// Rückgabe des letzten Werts einer CSV-Datei
string getLastValueFromCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Fehler beim Öffnen der Datei." << endl;
        return "";
    }

    string lastLine;
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            lastLine = line;
        }
    }

    vector<string> values = split(lastLine, ',');
    return values.empty() ? "" : values[0];
}

int daysDifference(const string& date1, const string& date2) {
    tm tm1 = {}, tm2 = {};
    istringstream ss1(date1), ss2(date2);
    ss1 >> get_time(&tm1, "%Y-%m-%d");
    ss2 >> get_time(&tm2, "%Y-%m-%d");

    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);

    auto tp1 = chrono::system_clock::from_time_t(t1);
    auto tp2 = chrono::system_clock::from_time_t(t2);

    auto differenceInDays = chrono::duration_cast<chrono::hours>(tp2 - tp1).count() / 24;
    return differenceInDays;
}

void displayLast30Days(const string& filename, const string& value) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Fehler beim Öffnen der Datei." << endl;
        return;
    }

    string today = getCurrentDateTime().substr(0, 10); // YYYY-MM-DD format
    string answ;

    string line;
    while (getline(file, line)) {
        size_t pos = line.find(",");
        string date = line.substr(pos + 2, 10);
        if (daysDifference(date, today) <= 30) {
            cout << line << endl;
            string folder = "arbeitszeit/";
            string arbeitszeit = folder + value + ".csv";
            addToCSV(arbeitszeit, line);
        }
    }
    file.close();
}

int main() {
    string folder = "mitarbeiter/";
    string mitarbeiterliste = folder + "mitarbeiterliste.csv";
    string status;
    vector<string> names;

    do {
        cout << "Gib deinen Vor und Nachnamen ein: ";
        string userInput;
        getline(cin, userInput);
        transformLower(userInput);

        // Gibt es die CSV datei ?
        bool exists = checkCSV(mitarbeiterliste);

        // Überprüfen, ob der Wert in der CSV-Datei vorhanden ist
        if (exists && isValueInCSV(mitarbeiterliste, userInput)) {
            string userFile = folder + userInput + ".csv";
            checkCSV(userFile);
            string lastStatus = getLastValueFromCSV(userFile);

            if (lastStatus == "anwesend") {
                cout << "Was möchtest du machen? abmelden | letzten 30 Tage anschauen: abmelden = 1 | 30 Tage = 2 | EXIT = 0 " << endl;
                getline(cin, status);
                if (status == "1") {
                    addToCSV(userFile, "abwesend, " + getCurrentDateTime());
                    break;
                }
                if (status == "2") {
                    string userFile = folder + userInput + ".csv";
                    displayLast30Days(userFile,userInput);
                    break;
                }
                else { break; }
            }
            else {
                cout << "Möchtest du dich anmelden ? ja/nein: " << endl;
                getline(cin, status);
                if (status == "ja") {
                    addToCSV(userFile, "anwesend, " + getCurrentDateTime());
                    cout << "Du bist jetzt anwesend: " << getCurrentDateTime() << endl;

                    cout << "Möchtest du dir die letzten 30 Tage anschauen? ja/nein: ";
                    string display;
                    getline(cin, display);
                    if (display == "ja") {
                        string userFile = folder + userInput + ".csv";
                        displayLast30Days(userFile,userInput);
                        
                        break;
                    }
                    else {
                        break;
                    }
                }
            }
        }
        else {
            cout << "Mitarbeiter ist nicht registriert. Registrieren Sie den Mitarbeiter? ja/nein: " << endl;
            getline(cin, status);
            if (status == "ja") {
                addToCSV(mitarbeiterliste, userInput);
                string userFile = folder + userInput + ".csv";
                checkCSV(userFile);
                cout << "Mitarbeiter wurde erfolgreich registriert!" << endl;
            }
        }

    } while (status != "nein");

    return 0;
}



