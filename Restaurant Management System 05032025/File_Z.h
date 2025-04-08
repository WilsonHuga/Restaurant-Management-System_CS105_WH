#ifndef File_Z
#define File_Z

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <ctime>
#include <mutex>
#include <sstream>
#include <functional>
#include <conio.h>
#include <limits>

using namespace std;

namespace TwoCli {

    // Base class for records
    class stbase {
    public:
        virtual ~stbase() = default;
        virtual string toString() const = 0;

        static string getCurrentTimeMark() {
            auto nowTime = chrono::system_clock::now();
            time_t currentTime = chrono::system_clock::to_time_t(nowTime);

            char buffer[26];
#ifdef _WIN32
            ctime_s(buffer, sizeof(buffer), &currentTime); // Fixed
#else
            ctime_r(&currentTime, buffer); // Fixed
#endif

            string timestamp(buffer);
            if (!timestamp.empty() && timestamp[timestamp.length() - 1] == '\n') {
                timestamp.pop_back();
            }
            return timestamp;
        }
    };

    // Message class to handle kitchen notifications
    class Message : public stbase {
    public:
        string timestamp;
        string action;
        int tableNumber;
        string status;
        vector<string> items;
        double total;
        string rawText;

        Message() : tableNumber(0), total(0.0) {}

        Message(const string& ts, const string& act, int table, const string& stat,
            const vector<string>& itemList, double t, const string& raw)
            : timestamp(ts), action(act), tableNumber(table), status(stat),
            items(itemList), total(t), rawText(raw) {
        }

        string toString() const override {
            return rawText;
        }

        static Message fromString(const string& line) {
            throw runtime_error("fromString not implemented for single-line parsing");
        }
    };

    // File manager implementation
    template<typename R>
    class FileManager {
    private:
        string filename;
        mutex file_mutex;

        static void clearScreen() {
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
        }

    public:
        FileManager(const string& file) : filename(file) {
            ifstream checkFile(filename);
            if (!checkFile.good()) {
                ofstream newFile(filename);
                newFile.close();
            }
        }

        vector<R> readRecords() {
            lock_guard<mutex> lock(file_mutex);
            vector<R> records;

            ifstream file(filename);
            if (!file.is_open()) {
                cerr << "Error: Could not open file " << filename << " for reading.\n";
                return records;
            }

            string line;
            string currentBlock;
            string timestamp, action, status;
            int tableNumber = 0;
            vector<string> items;
            double total = 0.0;
            bool inBlock = false;

            while (getline(file, line)) {
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }

                if (line.empty()) {
                    continue;
                }

                if (line.find("| Action:") != string::npos) {
                    if (inBlock) {
                        Message msg(timestamp, action, tableNumber, status, items, total, currentBlock);
                        records.push_back(msg);
                    }

                    inBlock = true;
                    currentBlock = line + "\n";
                    items.clear();

                    stringstream ss(line);
                    string temp;

                    getline(ss, timestamp, '|');
                    timestamp = timestamp.substr(0, timestamp.find_last_not_of(" \t") + 1);

                    getline(ss, temp, '|');
                    size_t actionPos = temp.find("Action: ");
                    if (actionPos != string::npos) {
                        action = temp.substr(actionPos + 8);
                        action = action.substr(0, action.find_last_not_of(" \t"));
                    }

                    getline(ss, temp, '|');
                    size_t tablePos = temp.find("Table: ");
                    if (tablePos != string::npos) {
                        string tableStr = temp.substr(tablePos + 7);
                        tableStr = tableStr.substr(0, tableStr.find_last_not_of(" \t"));
                        try {
                            tableNumber = stoi(tableStr);
                        }
                        catch (...) {
                            tableNumber = 0;
                        }
                    }

                    getline(ss, status, '|');
                    size_t statusPos = status.find("Status: ");
                    if (statusPos != string::npos) {
                        status = status.substr(statusPos + 8);
                        status = status.substr(0, status.find_last_not_of(" \t"));
                    }
                }
                else if (line == "--------------------------------------------------") {
                    if (inBlock) {
                        Message msg(timestamp, action, tableNumber, status, items, total, currentBlock);
                        records.push_back(msg);
                        inBlock = false;
                        currentBlock.clear();
                    }
                }
                else if (inBlock) {
                    currentBlock += line + "\n";

                    if (line.find("- ") != string::npos) {
                        string item = line.substr(line.find("- ") + 2);
                        items.push_back(item);
                    }
                    else if (line.find("Total: $") != string::npos) {
                        string totalStr = line.substr(line.find("$") + 1);
                        try {
                            total = stod(totalStr);
                        }
                        catch (...) {
                            total = 0.0;
                        }
                    }
                }
            }

            if (inBlock) {
                Message msg(timestamp, action, tableNumber, status, items, total, currentBlock);
                records.push_back(msg);
            }

            file.close();
            return records;
        }

        bool writeRecords(const vector<R>& records) {
            lock_guard<mutex> lock(file_mutex);

            ofstream file(filename);
            if (file.is_open()) {
                for (const auto& record : records) {
                    file << record.toString() << endl;
                }
                file.close();
                return true;
            }
            else {
                cerr << "Error: Could not open file for writing: " << filename << endl;
                return false;
            }
        }

        bool addRecord(const R& record) {
            auto records = readRecords();
            records.push_back(record);
            return writeRecords(records);
        }

        int getNextId() {
            auto records = readRecords();
            if (records.empty()) {
                return 1;
            }

            int maxId = 0;
            for (const auto& record : records) {
                if (record.id > maxId) {
                    maxId = record.id;
                }
            }
            return maxId + 1;
        }

        void startMonitor(
            function<void(const vector<R>&)> displayFunc,
            function<bool(const vector<R>&, const vector<R>&)> hasChangesFunc,
            int refreshInterval = 3
        ) {
            clearScreen();
            cout << "Starting live monitoring. Screen will refresh every "
                << refreshInterval << " seconds when changes detected." << endl;
            cout << "Press Enter to return to menu." << endl;

            vector<R> lastRecords;
            string lastDisplayTimestamp;
            bool needsRefresh = true;
            bool running = true;

            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            while (running) {
                vector<R> currentRecords = readRecords();
                string currentTimestamp = stbase::getCurrentTimeMark();

                bool hasChanges = needsRefresh || hasChangesFunc(currentRecords, lastRecords);

                if (hasChanges) {
                    clearScreen();
                    cout << "===== LIVE MONITOR =====" << endl;
                    cout << "Refreshing every " << refreshInterval << " seconds when changes detected" << endl;
                    cout << "Last updated: " << currentTimestamp << endl;
                    cout << "(Press Enter to return to menu)" << endl << endl;

                    if (lastRecords.size() < currentRecords.size() && !lastRecords.empty()) {
                        cout << "*** NEW RECORDS RECEIVED! ***" << endl << endl;
                    }

                    displayFunc(currentRecords);

                    lastRecords = currentRecords;
                    lastDisplayTimestamp = currentTimestamp;
                    needsRefresh = false;
                }

                auto startTime = chrono::steady_clock::now();
                while (chrono::duration_cast<chrono::seconds>(
                    chrono::steady_clock::now() - startTime).count() < refreshInterval) {
                    if (_kbhit()) {
                        char ch = _getch();
                        if (ch == '\r' || ch == '\n') {
                            running = false;
                            break;
                        }
                    }
                    this_thread::sleep_for(chrono::milliseconds(250));
                }
            }

            cout << "Exiting monitor mode..." << endl;
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    };

} // namespace TwoCli

#endif // File_Z