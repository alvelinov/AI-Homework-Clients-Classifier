#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <cmath>
#include <ctime>

enum ExpenseFrequency {
    Never,
    Rarely,
    Sometimes,
    Often,
    VeryOften,
};

struct Client {
    int expenses;
    ExpenseFrequency frequency;
    bool discount;

    Client(int exp = 0, ExpenseFrequency freq = Never, bool disc = false) : expenses(exp), frequency(freq), discount(disc) {};

    friend std::ostream& operator<<(std::ostream& os, const Client& c) {
        std::string freq;

        switch (c.frequency) {
        case Rarely:
            freq = "Rarely";
            break;
        case Sometimes:
            freq = "Sometimes";
            break;
        case Often:
            freq = "Often";
            break;
        case VeryOften:
            freq = "Very often";
            break;
        default:
            freq = "Never";
        }

        return os << c.expenses << ';' << freq << ';' << c.discount;
    }
};

struct Pair {
    Client client;
    double distance;

    friend bool operator>(const Pair& left, const Pair& right) {
        return left.distance > right.distance;
    }
};

void initializeData(std::vector<Client>& clients, const std::string& filename, int& min, int& max) {
    std::ifstream iFile(filename);

    if (!iFile) {
        std::cerr << "Couldn't open the file!\n";
        return;
    }

    std::string buf, segment;

    // get rid of the first row
    getline(iFile, buf);
    buf.clear();

    while (getline(iFile, buf)) {
        Client client;

        std::stringstream s(buf);

        getline(s, segment, ';');
        client.expenses = std::stoi(segment);
        if (client.expenses > max)
            max = client.expenses;

        if (client.expenses < min)
            min = client.expenses;

        getline(s, segment, ';');
        switch (segment[0]) {
        case 'V':
            client.frequency = VeryOften;
            break;
        case 'S':
            client.frequency = Sometimes;
            break;
        case 'O':
            client.frequency = Often;
            break;
        case 'R':
            client.frequency = Rarely;
            break;
        }
        

        getline(s, segment);
        if (segment[0] == '1')
            client.discount = true;
        else
            client.discount = false;

        clients.push_back(client);
    }
}

double normalizedDistanceBetween(const Client& c1, const Client& c2, int& min, int& max) {
    double normalizedExpense1 = ((double)c1.expenses - min) / ((double)max - (double)min);
    double normalizedExpense2 = ((double)c2.expenses - min) / ((double)max - (double)min);

    double normalizedFrequency1 = ((double)c1.frequency - (int)Rarely) / ((double)VeryOften - (int)Rarely);
    double normalizedFrequency2 = ((double)c2.frequency - (int)Rarely) / ((double)VeryOften - (int)Rarely);

    double sqDist1 = std::pow(normalizedExpense1 - normalizedExpense2, 2);
    double sqDist2 = std::pow(normalizedFrequency1 - normalizedFrequency2, 2);

    return std::sqrt(sqDist1 + sqDist2);
}

void normalizeData(std::vector<Client>& clients, const Client& client, int& min, int& max,
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair> >& q)
{
    for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        Client current = *it;
        q.push({ current, normalizedDistanceBetween(current, client, min, max) });
    }
}

bool majority(int k, std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair> >& q)
{
    unsigned ones = 0, zeros = 0;
    Client firstClient = q.top().client;

    while (k > 0) {
        Client c = q.top().client;
        double d = q.top().distance;
        q.pop();

        // pretty prints for the k nearest neighbours
        std::cout << std::setw(5) << c.expenses << ',';
        switch (c.frequency) {
        case Rarely:
            std::cout << std::setw(10) << "Rarely";
            break;
        case Sometimes:
            std::cout << std::setw(10) << "Sometimes";
            break;
        case Often:
            std::cout << std::setw(10) << "Often";
            break;
        case VeryOften:
            std::cout << std::setw(10) << "Very often";
            break;
        default:
            std::cout << std::setw(10) << "Never";
        };            
        std::cout << ',' << c.discount << " -----> " << d << '\n';

        if (c.discount)
            ones++;
        else
            zeros++;

        --k;
    }

    if (zeros == ones)
        return firstClient.discount;

    return ones > zeros;
}

int main()
{
    srand(time(NULL));

    int min = INT_MAX, max = INT_MIN;
    std::vector<Client> clients;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> clientQueue;
    
    
    // test data
    int k = 7;
    Client testClient(1500, Sometimes);

    initializeData(clients, "Data.csv", min, max);
    normalizeData(clients, testClient, min, max, clientQueue);
    testClient.discount = majority(k, clientQueue); 

    std::cout << "\n\nTest case classified: \n";
    std::cout << testClient << std::endl;
    

    /*
    initializeData(clients, "Data.csv", min, max);

    std::ofstream oFile("testResults.txt", std::ios::app);
    for (int exp = 600; exp <= 1600; exp += 50) {
        for (int freq = 1; freq <= 4; freq++) {
            for (int k = 1; k <= 39; k += 2) {
                Client testClient(exp, (ExpenseFrequency)freq);
                normalizeData(clients, testClient, min, max, clientQueue);
                testClient.discount = majority(k, clientQueue);
                
                oFile << std::setw(5) << testClient.expenses << ',';
                switch (testClient.frequency) {
                case Rarely:
                    oFile << std::setw(10) << "Rarely";
                    break;
                case Sometimes:
                    oFile << std::setw(10) << "Sometimes";
                    break;
                case Often:
                    oFile << std::setw(10) << "Often";
                    break;
                case VeryOften:
                    oFile << std::setw(10) << "Very often";
                    break;
                default:
                    oFile << std::setw(10) << "Never";
                };
                oFile << ',' << testClient.discount << " -----> " << "k= " << k << '\n';

                std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> emptyQueue;
                clientQueue = emptyQueue;
            }
            oFile << "\n\n\n";
        }
    }
    */
    return 0;
}

