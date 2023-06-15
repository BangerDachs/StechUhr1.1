#include <fstream>
#include <filesystem>
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
#include <map>
#include <tuple>


using namespace std;

// Teilen eines Strings
vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void CheckFolder(const std::filesystem::path& p)
{
    if (!std::filesystem::exists(p))
    {
        std::filesystem::create_directories(p);
        std::cout << "." << endl;; // . existiert
    }
    else
    {
        std::cout << ":" << endl;// : wurde erstellt
    }
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

// Überprüfen ob ein Wert in einer CSV-Datei vorhanden ist
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
    strftime(buffer, sizeof(buffer), "%Y-%m-%d,%H:%M:%S", &date_time);

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

void transformLower(string& userInput) {
    transform(userInput.begin(), userInput.end(), userInput.begin(),
        [](unsigned char c) { return tolower(c); });
}

void capitalizeFirstLetter(string& str) {
    if (!str.empty()) {
        str[0] = toupper(str[0]);
    }
}

// Rückgabe des letzten Werts der CSV-Datei
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
// Arbeitszeit berechnen / time1-anfang time2-ende
tuple<int, int, int> workTimeDifference(const string& time1, const string& time2) {
    std::tm tm1 = {}, tm2 = {};
    std::istringstream ss1(time2), ss2(time1);
    ss1 >> std::get_time(&tm1, "%Y-%m-%d %H:%M:%S");
    ss2 >> std::get_time(&tm2, "%Y-%m-%d %H:%M:%S");

    std::time_t t1 = std::mktime(&tm1);
    std::time_t t2 = std::mktime(&tm2);

    auto tp1 = std::chrono::system_clock::from_time_t(t1);
    auto tp2 = std::chrono::system_clock::from_time_t(t2);

    auto differenceInTime = std::chrono::duration_cast<std::chrono::seconds>(tp1 - tp2);

    int hours = differenceInTime.count() / 3600;
    int minutes = (differenceInTime.count() % 3600) / 60;
    int seconds = (differenceInTime.count() % 60);

    return std::make_tuple(hours, minutes, seconds);
}


int daysDifference(const string& date1, const string& date2) {
    tm tm1 = {}, tm2 = {};
    istringstream ss1(date1), ss2(date2);
    ss1 >> get_time(&tm1,"%Y-%m-%d"); //ss = StringStream
    ss2 >> get_time(&tm2,"%Y-%m-%d");

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
    string line;
    string folder = "arbeitszeit/";
    string arbeitszeit = folder + value + ".csv";

    while (getline(file, line)) {
        size_t pos = line.find(",");
        string date = line.substr(pos + 1, 10);
        if (daysDifference(date, today) <= 30) {
            cout << line << endl;
            addToCSV(arbeitszeit, line);
        }
    }
    file.close();
}

void displayUserDefinedDays(const string& filename, const string& value) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Fehler beim Oeffnen der Datei." << endl;
        return;
    }
    string folder = "arbeitszeit/";
    string arbeitszeit = folder + value + ".csv";
    checkCSV(arbeitszeit);
    string today = getCurrentDateTime().substr(0, 10); // YYYY-MM-DD format
    string userDefined; 
    cout << "Wie viele Tage moechtest du dir anzeigen lassen: ";
    getline(cin, userDefined);
    int userDefinedDays = stoi(userDefined);
    
    string line;
    while (getline(file, line)) {
        size_t pos = line.find(",");
        string date = line.substr(pos + 1, 10);
        if (daysDifference(date, today) <= userDefinedDays) {
            cout << line << endl;
            addToCSV(arbeitszeit, line);
        }
    }
    file.close();
}
// zeigt die tätigkeit an
string selectJob(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Fehler beim Öffnen der Datei." << endl;
        return "";
    }

    map<int, string> jobs;
    string line;
    while (getline(file, line)) {
        int commaPos = line.find(',');
        int id = stoi(line.substr(0, commaPos));
        string job = line.substr(commaPos + 1);
        jobs[id] = job;
    }

    file.close();

    for (const auto& job : jobs) {
        cout << job.first << ", " << job.second << endl;
    }

    int selection = 0;
    while (true) {
        cout << "Wählen Sie eine Jobnummer zwischen 1 und 5: ";
        cin >> selection;

        if (jobs.find(selection) != jobs.end()) {
            break;
        }
        else {
            cout << "Ungültige Auswahl, bitte versuchen Sie es erneut." << endl;
        }
    }

    return jobs[selection];
}

// liest die mitarbeiter csv, sucht nach dem userInput und gibt userInput, tätigkeit aus
void displayCSV(const string& filename, const string& value) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Fehler beim Öffnen der Datei." << endl;
        return;
    }
    string line;

    // erste zeile
    if (getline(file, line)) {
        cout << line << endl;
    }

    // suche nach dem gesuchten wert
    while (getline(file, line)) {
        if (line.find(value) != string::npos) {
            cout << line << endl;
        }
    }
    file.close();
}

bool checkPassword(const string& filename, const string& value1, const string& value2) {
    std::ifstream file(filename);

    std::string line;
    while (getline(file, line)) {
        std::vector<std::string> values = split(line, ',');
        if (find(values.begin(), values.end(), value1) != values.end() &&
            find(values.begin(), values.end(), value2) != values.end()) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

void displayDailyWorkingHours(const string& filename, const string& user) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Fehler beim Öffnen der Datei." << endl;
        return;
    }
    
    string line, lastStatus, lastDateTime;
    string workFolder = "arbeitszeit/";
    string arbeitszeit = workFolder + user + "DWH.csv";// DWH = DailyWorkingHours :)
    
    while (getline(file, line)) {
        vector<string> data = split(line, ','); //hier wird gespeichert in data
        string status = data[0]; //an-abwesend
        string dateTime = data[1] + " " + data[2]; // Kombinieren Datum & Uhrzeit

        if (status == "anwesend") {
            lastStatus = status;
            lastDateTime = dateTime;
        }
        else if (status == "abwesend" && lastStatus == "anwesend") {
            // berechne arbeitstag
            std::tuple<int, int, int> timeDiff = workTimeDifference(lastDateTime, dateTime);
            int hours = std::get<0>(timeDiff);
            int minutes = std::get<1>(timeDiff);
            int seconds = std::get<2>(timeDiff);
            string date = data[1];


            std::stringstream ss;
            ss << "Am " << date << " hat " << user << " " << hours << " Stunden und " << minutes << " Minuten gearbeitet." << endl;
            string output = ss.str();

            // Zurücksetzen
            lastStatus = "";
            lastDateTime = "";
            addToCSV(arbeitszeit,output);
        }
    }
}

void displaySpecificDailyWorkingHours(const string& filename, const string& user, const string& date) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Fehler beim Öffnen der Datei." << endl;
        return;
    }

    string line, lastStatus, lastDateTime;
    string workFolder = "arbeitszeit/";
    string arbeitszeit = workFolder + user + "SDWH.csv"; //SDWH = SpecificDailyWorkingHours :D

    while (getline(file, line)) {
        vector<string> data = split(line, ',');
        string status = data[0];
        string current_date = data[1];
        string dateTime = current_date + " " + data[2]; // Kombinieren Datum & Uhrzeit

        if (current_date != date) {
            continue;  // skip
        }

        if (status == "anwesend") {
            lastStatus = status;
            lastDateTime = dateTime;
        }
        else if (status == "abwesend" && lastStatus == "anwesend") {
            std::tuple<int, int, int> timeDiff = workTimeDifference(lastDateTime, dateTime);
            int hours = std::get<0>(timeDiff);
            int minutes = std::get<1>(timeDiff);
            int seconds = std::get<2>(timeDiff);

            std::stringstream ss;
            ss << "Am " << current_date << " hat " << user << " " << hours << " Stunden und " << minutes << " Minuten gearbeitet." << endl;
            string output = ss.str();
            cout << "Am " << current_date << " hat " << user << " " << hours << " Stunden und " << minutes << " Minuten gearbeitet." << endl;

            lastStatus = "";
            lastDateTime = "";
            addToCSV(arbeitszeit, output);
        }
    }
}

tuple<string, string, string> getFirstTwoValuesFromCSV(const string& filename, const string& value1) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Fehler beim Öffnen der Datei." << std::endl;
        return { "", "", "" };
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> values = split(line, ',');
        if (values.size() < 4) {
            continue;
        }
        if (values[0] == value1) {
            file.close();
            return { values[0], values[2], values[3] }; // Rückgabe der Vor- und Nachnamen
        }
    }

    file.close();
    return { "", "", "" };
}

void saveWorkInformation(const string& filename, const string& userInput, const string& mitarbeiterliste) {
    auto Information = getFirstTwoValuesFromCSV(mitarbeiterliste, userInput);
    string username = std::get<0>(Information);
    string jobtitle = std::get<1>(Information);
    string userDate = std::get<2>(Information);

    string addUserInfo = username + " " + jobtitle + " " + userDate;
    addToCSV(filename, getCurrentDateTime());
    addToCSV(filename, addUserInfo);
}


void handleSignIn(const string& userFile, const string& lastStatus) {
    string status;
    cout << "Moechtest du dich anmelden ? ja/nein: " << endl;
    getline(cin, status);
    if (status == "ja") {
        addToCSV(userFile, "anwesend," + getCurrentDateTime());
        cout << "Du bist jetzt anwesend: " << getCurrentDateTime() << endl;
    }
}


void handleRegistration(const string& mitarbeiterliste, const string& userInput, const string& password, const string& folder) {
    string jobFolder = "jobs/";
    string jobCSV = jobFolder + "jobs.csv";

    cout << "Welche Tätigkeit übt der Mitarbeiter aus:" << endl;
    string jobtitle = selectJob(jobCSV);
    capitalizeFirstLetter(jobtitle);

    addToCSV(mitarbeiterliste, userInput + "," + password + "," + jobtitle + "," + getCurrentDateTime());
    string userFile = folder + userInput + ".csv";
    checkCSV(userFile);
    cout << "Mitarbeiter wurde erfolgreich registriert!" << endl;
}

void manageUserActions(const string& userFile, const string& userInput, const string& mitarbeiterliste, const string& workFolder) {
    string status;
    while(true) {
    cout << "Was moechtest du machen ?\n1 = abmelden\n2 = Uebersicht letzte 30 Tage\n3 = Uebersicht beliebiger Tage\n4 = beliebiger Arbeitszeitnachweis\n5 = Kompletter Arbeitszeitnachweis\n6 = Dein Profil\n0 = EXIT\n";
    getline(cin, status);
    if (status == "1") {
        addToCSV(userFile, "abwesend," + getCurrentDateTime());
        cout << "Auf wiedersehen.";
        exit(0);
    }
    else if (status == "2") {
        string arbeitszeit = workFolder + userInput + "1Month.csv";
        saveWorkInformation(arbeitszeit, userInput, mitarbeiterliste);
        displayLast30Days(userFile, userInput);
    }
    else if (status == "3") {
        string arbeitszeit = workFolder + userInput + "UDD.csv";
        saveWorkInformation(arbeitszeit, userInput, mitarbeiterliste);
        displayUserDefinedDays(userFile, userInput);
    }
    else if (status == "4") {
        string date;
        cout << "Welches Tag moechtest du einsehen? bsp: 2023-05-24\nDein Datum: ";
        getline(cin, date);
        displaySpecificDailyWorkingHours(userFile, userInput, date);
    }
    else if (status == "5") {
        string arbeitszeit = workFolder + userInput + "DWH.csv";
        saveWorkInformation(arbeitszeit, userInput, mitarbeiterliste);
        displayDailyWorkingHours(userFile, userInput);
    }
    else if (status == "6") {
        displayCSV(mitarbeiterliste, userInput);
        cout << "\n";
    }
    else {
        cout << "Auf wiedersehen.";
        exit(0);
    }
    }
}


// Hauptfunktion
int main() {
    string folder = "mitarbeiter/";
    string workFolder = "arbeitszeit/";
    string mitarbeiterliste = folder + "mitarbeiterliste.csv";
    string status;
    vector<string> names;

    string userInput, password;

    cout << "Gib deinen Benutzernamen ein: ";
    getline(cin, userInput);
    cout << "Gib dein Passwort ein: ";
    getline(cin, password);
    CheckFolder(folder);
    CheckFolder(workFolder);

    do {
        transformLower(userInput);

        bool exists = checkCSV(mitarbeiterliste);

        if (checkPassword(mitarbeiterliste, userInput, password)) {

            if (exists && isValueInCSV(mitarbeiterliste, userInput)) {
                string userFile = folder + userInput + ".csv";
                checkCSV(userFile);
                string lastStatus = getLastValueFromCSV(userFile);

                if (lastStatus == "anwesend") {
                    manageUserActions(userFile, userInput, mitarbeiterliste, workFolder);
                }
                else {
                    handleSignIn(userFile, lastStatus);
                }
            }
        }
        else {
            cout << "Mitarbeiter ist nicht registriert.\nSoll der Mitarbeiter registriert werden? ja/nein: " << endl;
            getline(cin, status);
            if (status == "ja") {
                handleRegistration(mitarbeiterliste, userInput, password, folder);
                continue;
            }
        }
    } while (status != "nein");

    return 0;
}
