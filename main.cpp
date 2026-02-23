#include <iostream>
#include <fstream>
#include <algorithm>

#include <locale>
#include <windows.h>

#include "Parser.h"
#include "Random.h"

using namespace std;

int kRoundCount = 10;

// main currency
int population, landInAcre, wheat;

// game info
int roundIndex;

// stat
int diedFromHungry;
float diedFromHungryPercent;
float diedFromHungryPercentRound;

// round info
bool wasPlague;
int arrivedPeople, collectWheat, collectWheatPerAcre, wheatRatted, pricePerAcre;

float wheatRattedPercent;

constexpr int kMaxAcrePlantByOnePerson = 10;
constexpr int kWheatForOnePersonForYear = 20;

constexpr const char* SavePath = "C:\\Users\\God-ish\\Projects\\Cpp\\LR1_Hammurabi\\Save\\savedGame.txt";

struct Orders {
    int landBought = 0;
    int landSold = 0;
    int wheatForPeople = 0;
    int landForPlant = 0;
};


bool HasSaveFile();
bool LoadSavedGame();
void Save();
void CreateNewGame(Random& random);
void PrintStatus();
void PrintResult(int completedRounds);
bool AskYesNo(const char* inviteMessage);
void ReadOrders(Orders& orders);
void ComputeRoundConstants(Random& random, int& collectWheatPerAcre, float& wheatRattedPercent, bool& wasPlague);
bool ValidateOrdersNonNegative(const Orders& orders);
bool ValidateOrdersHasEnoughLand(const Orders& orders, int land_in_acre);
bool ValidateOrdersPeopleLimit(const Orders& orders, int population);
bool ValidateOrdersHasEnoughWheat(const Orders& orders, int wheat, int price_per_acre);

bool IsUprisingThisYear();

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // setlocale(LC_ALL, "rus");

    auto random = Random();

    CreateNewGame(random);

    if (HasSaveFile()) {
        const bool wannaLoad = AskYesNo(
            "Найдено сохранение. Желаете его загрузить? 1 - да, 0 - нет: "
        );

        if (wannaLoad)
        {
            if (!LoadSavedGame())
            {
                cout << "Сохранение повреждено. Начинаю новую игру.\n\n";
                CreateNewGame(random);
            }
        }
    }
    int completedRounds = 0;

    while (completedRounds < kRoundCount) {
        pricePerAcre = random.GetRandomInt(17, 26);

        PrintStatus();
        Orders orders;
        ReadOrders(orders);

        while ((ValidateOrdersNonNegative(orders)
            && ValidateOrdersHasEnoughLand(orders, landInAcre)
            && ValidateOrdersPeopleLimit(orders, population)
            && ValidateOrdersHasEnoughWheat(orders, wheat, pricePerAcre))
            == false
            ) {
            cout << "О, повелитель, пощади нас! У нас только " << population << " человек, "
                << wheat << " бушелей пшеницы и " << landInAcre << " акров земли!\n\n";
            ReadOrders(orders);
        }

        //apply orders
        const int landDiffAdded = orders.landBought - orders.landSold;
        landInAcre += orders.landBought - orders.landSold;
        wheat = wheat - landDiffAdded * pricePerAcre - orders.wheatForPeople - orders.landForPlant * 0.5;

        ComputeRoundConstants(random, collectWheatPerAcre, wheatRattedPercent, wasPlague);
        // simulate year
        collectWheat = collectWheatPerAcre * orders.landForPlant;
        wheat += collectWheat;

        wheatRatted = static_cast<int>(wheatRattedPercent * static_cast<float>(wheat));
        wheat -= wheatRatted;

        int peopleWhoEat = orders.wheatForPeople / kWheatForOnePersonForYear;
        if (peopleWhoEat < population) {
            diedFromHungry = population - peopleWhoEat;
            diedFromHungryPercentRound = static_cast<float>(diedFromHungry) / static_cast<float>(population);
            diedFromHungryPercent += diedFromHungryPercentRound;
            population -= diedFromHungry;
        }
        wheat -= peopleWhoEat * 20;

        arrivedPeople = diedFromHungry / 2 + (5 - collectWheatPerAcre) * wheat / 600 + 1;
        arrivedPeople = clamp(arrivedPeople, 0, 50);
        population += arrivedPeople;

        if (wasPlague) {
            population /= 2;
        }

        completedRounds++;
        roundIndex++;

        if (IsUprisingThisYear())
            break;

        Save();
        cout << endl;
    }
    PrintResult(completedRounds);
    return 0;
}

bool HasSaveFile() {
    ifstream fin(SavePath);
    return fin.good();
}

bool LoadSavedGame() {
    ifstream file(SavePath, ios::ate);
    bool success = false;

    if (file.is_open())
    {
        bool isEmpty = (file.tellg() == 0);

        if (isEmpty) {
            cout << "Файл сохранения поврежден!\n\n";
        }
        else {
            file.seekg(0, ios::beg);

            if (!(file >> population >> wheat >> landInAcre >> roundIndex
                >> diedFromHungry >> arrivedPeople >> wasPlague
                >> collectWheat >> collectWheatPerAcre >> wheatRatted >> pricePerAcre))
            {
                cout << "Файл сохранения поврежден!\n\n";
                return false;
            }
            cout << "Сохранение загружено.\n\n";
            success = true;
        }
        file.close();
    }
    else {
        cout << "Нет файла сохранения или он поврежден!\n\n";
    }

    return success;
}

void Save() {
    ofstream file(SavePath, ios::trunc);

    if (file.is_open())
    {
        file << population << "\n"
            << wheat << "\n"
            << landInAcre << "\n"
            << roundIndex << "\n"
            << diedFromHungry << "\n"
            << arrivedPeople << "\n"
            << wasPlague << "\n"
            << collectWheat << "\n"
            << collectWheatPerAcre << "\n"
            << wheatRatted << "\n"
            << pricePerAcre << "\n";

        file.close();
    }

    else
    {
        cout << "Нет файла сохранения или он поврежден!\n\n";
    }
}

void CreateNewGame(Random& random) {
    population = 100, wheat = 2800, landInAcre = 1000;
    roundIndex = 0;
    diedFromHungry = 0;

    wasPlague=false;
    arrivedPeople=0, collectWheatPerAcre=0, wheatRatted=0;
    pricePerAcre=random.GetRandomInt(1,6);

    float ratIndex=0.;
}

void PrintStatus() {
    cout << "Мой повелитель,соизволь поведать тебе:\n"
    << "В году " << roundIndex+1 << " твоего высочайшего правления\n";

    if (diedFromHungry > 0) {
        cout << diedFromHungry << " человек умерли с голоду" << ((arrivedPeople > 0) ? ", и " : ";\n");
    }
    if (arrivedPeople > 0) {
        cout << arrivedPeople << " человек прибыли в наш великий город\n";
    }
    if (wasPlague) {
        cout << "Чума уничтожила половину населения;\n";
    }

    cout << "Население города сейчас составляет " << population << " человек;\n"
    << "Мы собрали "<< collectWheat <<" бушелей пшеницы, по "<< collectWheatPerAcre <<" бушеля с акра;\n"
    << "Крысы истребили "<< wheatRatted <<" бушелей пшеницы, оставив "<< wheat <<" бушеля в амбарах;\n"
    << "Город сейчас занимает "<< landInAcre <<" акров;\n"
    << "1 акр земли стоит сейчас "<< pricePerAcre <<" бушель;\n";

    cout << "Что пожелаешь, повелитель?\n";

}

bool AskYesNo(const char *inviteMessage) {
    int userInput;
    string userInputBuff;

    cout << inviteMessage;
    cin >> userInputBuff;

    while (TryParseInt(userInputBuff, userInput) == false && userInput >= 0 && userInput <= 1)
    {
        cout << "Некореектный ввод.\n" << inviteMessage << endl;
        cin >> userInputBuff;
    }
    return userInput == 1;
}

void ReadOrders(Orders &orders) {
    bool inputCorrect = false;
    string buffLandBought, buffLandSold, buffWheatForPeople, buffLandForPlant;

    while (inputCorrect == false) {
        cout << "Сколько акров земли повелеваешь купить? ";
        cin >> buffLandBought;

        cout << "Сколько акров земли повелеваешь продать? ";
        cin >> buffLandSold;

        cout << "Сколько бушелей пшеницы повелеваешь съесть? ";
        cin >> buffWheatForPeople;

        cout << "Сколько акров земли повелеваешь засеять? ";
        cin >> buffLandForPlant;
        inputCorrect = (TryParseInt(buffLandBought, orders.landBought)
                && TryParseInt(buffLandSold, orders.landSold)
                && TryParseInt(buffWheatForPeople, orders.wheatForPeople)
                && TryParseInt(buffLandForPlant, orders.landForPlant) );
        if (inputCorrect == false) {
            cout << "О, повелитель, пощади нас! Мы не понимаем твои указания\n";
        }
    }
}

bool ValidateOrdersNonNegative(const Orders &orders) {
    return orders.landForPlant >= 0 && orders.wheatForPeople >= 0 && orders.landBought >= 0 && orders.landSold >= 0;
}

bool ValidateOrdersHasEnoughLand(const Orders &orders, int land_in_acre) {
    const int newLand = landInAcre + orders.landBought - orders.landSold;
    return !(orders.landSold > land_in_acre || newLand < 0 ||  orders.landForPlant > newLand);
}

bool ValidateOrdersPeopleLimit(const Orders &orders, int population) {
    const int maxPlantable = population * kMaxAcrePlantByOnePerson;
    return orders.landForPlant <= maxPlantable;
}

bool ValidateOrdersHasEnoughWheat(const Orders &orders, int wheat, int price_per_acre) {
    const int wheatAfterTrade = wheat + price_per_acre * (orders.landSold - orders.landBought);
    if (wheatAfterTrade < 0)
        return false;
    const int seeds = orders.landForPlant / 20;
    const int wheatAfterFoodAndSeeds = wheatAfterTrade - orders.wheatForPeople - seeds;
    return wheatAfterFoodAndSeeds >= 0;
}

bool IsUprisingThisYear() {
    return diedFromHungryPercentRound > 0.45f;
}

void ComputeRoundConstants(Random& random, int& collectWheatPerAcre, float& wheatRattedPercent, bool& wasPlague) {
    collectWheatPerAcre = random.GetRandomInt(1, 6);
    wheatRattedPercent = random.GetRandomFloat(0.f, 0.07f);
    wasPlague = random.GetRandomFloat(0, 1) < 0.15;
}

void PrintResult(int completedRounds) {
    cout << "Игра окончена!" << endl;
    float acreForPerson = static_cast<float>(landInAcre) / static_cast<float>(population);
    diedFromHungryPercent = diedFromHungryPercent / 10;

    if (diedFromHungryPercent > 33 && acreForPerson < 7 || completedRounds < kRoundCount)
    {
        cout << "Из-за вашей некомпетентности в управлении, народ устроил бунт, и изгнал вас их города. Теперь вы вынуждены влачить жалкое существование в изгнании" << endl;
    }
    else if (diedFromHungryPercent > 10 && acreForPerson < 9)
    {
        cout << "Вы правили железной рукой, подобно Нерону и Ивану Грозному. Народ вздохнул с облегчением, и никто больше не желает видеть вас правителем" << endl;
    }
    else if (diedFromHungryPercent > 3 && acreForPerson < 10)
    {
        cout << "Вы справились вполне неплохо, у вас, конечно, есть недоброжелатели, но многие хотели бы увидеть вас во главе города снова" << endl;
    }
    else
    {
        cout << "Фантастика! Карл Великий, Дизраэли и Джефферсон вместе не справились бы лучше" << endl;
    }
}
