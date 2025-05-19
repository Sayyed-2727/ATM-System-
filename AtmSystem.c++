#include <string>
#include <iostream>
#include <cctype>
#include <vector>
#include <fstream>
#include <cmath>
#include <iomanip>
using namespace std;

enum enMainMenuOptions {QuickWithdraw = 1, NormalWithdraw = 2, Deposit = 3, CheckBalance = 4, Logout = 5};
enum enWithdrawAmount {Twenty = 1, Fifty = 50, OneHundred = 3, TwoHundreds = 4, FiveHundreds = 5, OneThousand = 6, TwoThousands = 7, TenThousands = 8, Exit = 9};

struct stClient
{
    string AccountNumber = "";
    string PinCode = "";
    string Name = "";
    string PhoneNumber = "";
    double AccountBalance = 0;
    bool MarkToDelete = false;
};

const string ClientsNameFile = "ClientsNameFile.txt";
stClient CurrentClient;

void ShowMainMenu();
void Login();

string ReadString(string Message)
{
    string info = "";
    cout << Message;
    getline(cin >> ws, info);

    return info;
}

int ReadANumber(string Message)
{
    int option = 0;
    cout << Message;
    cin >> option;

    return option;
}

vector <string> SplitString(string DataLine, string Delimiter)
{
    vector <string> vwords;
    string word = ""; // define a string variable  
    short pos = 0;

    // use find() function to get the position of the delimiters  
    while ((pos = DataLine.find(Delimiter)) != std::string::npos) 
    {
        word = DataLine.substr(0,pos);  // store the word 
        if (word != "")
        {
            vwords.push_back(word);
        }

        DataLine.erase(0, pos + Delimiter.length());    /* erase() until positon and move to next word. */
    }
    if (DataLine!= "")
    {
        vwords.push_back(DataLine); // it adds last word of the string.
    }
    
    return vwords;
}

void WelcomeMessage()
{
    vector <string> ClientName = SplitString(CurrentClient.Name, " ");
    printf("Welcome %s,", ClientName[0].c_str());
}

stClient ConvertDataLineToRecord(string DataLine, string Delimiter = "#//#")
{
    stClient Client;
    vector <string> vClients = SplitString(DataLine, Delimiter);

    Client.AccountNumber = vClients[0];
    Client.PinCode = vClients[1];
    Client.Name = vClients[2];
    Client.PhoneNumber = vClients[3];
    Client.AccountBalance = stod(vClients[4]);

    return Client;
}

vector <stClient> LoadClientsDataFromFile(string FileName)
{
    vector <stClient> vClients;
    fstream MyFile;

    MyFile.open(FileName, ios::in); // ReadMode

    if (MyFile.is_open())
    {
        string Line;
        stClient Client;
        while (getline(MyFile, Line))
        {
            Client = ConvertDataLineToRecord(Line);
            vClients.push_back(Client);
        }
    }
    MyFile.close();

    return vClients;
}

bool CheckAccoutNumberAndPincodeOfClient(vector <stClient> vClients, string AccoutNumber, string PinCode, stClient& Client)
{
    for(stClient c : vClients)
    {
        if ((c.AccountNumber == AccoutNumber) && (c.PinCode == PinCode))
        {
            Client = c;
            return true;
        }
    }

    return false;
}

void GoBackToMainMenu()
{
    cout << "\n\nPress any key to go back to main menu...... ";
    system("pause>0");
    ShowMainMenu();
}

int CheckBalanceOfAClient(vector <stClient> vclient)
{
    for(stClient& c: vclient)
    {
        if (c.AccountNumber == CurrentClient.AccountNumber)
        {
            return c.AccountBalance;
        }
    }
}

void ShowCheckBalanceScreen()
{
    vector <stClient> vClients = LoadClientsDataFromFile(ClientsNameFile);

    cout << "======================================================";
    cout << "\n\t\tCheck Balance Screen\n";
    cout << "======================================================\n\n";

    cout << "Your balance is: " << CheckBalanceOfAClient(vClients) << endl << endl;
}

string ConvertRecordToLine(stClient Client, string Delimiter = "#//#")
{
    
    string stClientRecord = "";

    stClientRecord += Client.AccountNumber + Delimiter;
    stClientRecord += Client.PinCode + Delimiter;
    stClientRecord += Client.Name + Delimiter;
    stClientRecord += Client.PhoneNumber + Delimiter;
    stClientRecord += to_string(Client.AccountBalance);

    return stClientRecord;

}

vector <stClient> SaveClientsDataToFile(string FileName, vector <stClient>& vClient)
{
    fstream MyFile;
    string Dataline;
    MyFile.open(FileName, ios::out); // Overwrite mode

    if (MyFile.is_open())
    {
        for (stClient c: vClient)
        {
            if (c.MarkToDelete == false)
            {
                //we only write records that are not marked for delete.  
                Dataline = ConvertRecordToLine(c);
                MyFile << Dataline << endl;
            }
            
        }

        MyFile.close();
    }

    return vClient;
}

bool CheckSufficientBalance(vector <stClient> vClients, short AmountToWithdraw, short OperationType = 1)
{
    for(stClient c : vClients)
    {
        if (c.AccountNumber == CurrentClient.AccountNumber)
        {
            return (c.AccountBalance >= (AmountToWithdraw * OperationType));   
        }
    }
    return false;
}

void WithdrawOrDepositAnAmount(int Amount, vector <stClient>& vClients, short OperationType = 1)
{
    if (!CheckSufficientBalance(vClients, Amount, OperationType))
    {
        cout << "\n\nWe're sorry, you don't have sufficient balance to proceed with this transaction.\nPlease try again later....";
        return;
    }
    else
    {
        char UserAnswer = 'n';
        int OriginalBalance =0;
        cout << "\n\nAre you sure you want to perform this transaction? ";
        cin >> UserAnswer;
        if (tolower(UserAnswer) != 'y')
        {
            cout << "\n\nDon't worry! The transaction has been cancelled.\n";
            return;
        }
        else
        {
            /* We can't change the current client directly cuz it will not be saved for being a variable
            So the solution is to itierate in the DDBB in the file using the info of the current variable
            That way it's going to be saved for being the DDBB itself
            */
            for (stClient& c: vClients)
            {
                if (c.AccountNumber == CurrentClient.AccountNumber)
                {    
                    CurrentClient.AccountBalance = c.AccountBalance;    
                    OriginalBalance = c.AccountBalance;
                    c.AccountBalance -= (Amount * OperationType);
                    if (OperationType == 1)
                    {
                        printf("\n\nDone Successfully.\n\nYour Originl Balance Is: %d\nAmount To Withdraw: %d\nYour New Balance is: %d ", OriginalBalance, Amount ,OriginalBalance-Amount);
                    }
                    else if(OperationType == -1)
                    {
                        printf("\n\nDone Successfully.\n\nOriginal Balance: %d\nAmount To Deposite: %d\nYour New Balance: %d", OriginalBalance, Amount, OriginalBalance + Amount);
                    }
                    
                }
            }
            
            SaveClientsDataToFile(ClientsNameFile, vClients);
        }
    }    
}

void PerformQuickWithdrawScreen(enWithdrawAmount WithDrawAmount, vector <stClient>& vClients)
{
    switch (WithDrawAmount)
    {
    case enWithdrawAmount::Twenty:
        WithdrawOrDepositAnAmount(20, vClients);
        break;
    
    case enWithdrawAmount::Fifty:
        WithdrawOrDepositAnAmount(50, vClients);
        break;
    
    case enWithdrawAmount::OneHundred:
        WithdrawOrDepositAnAmount(100, vClients);
        break;
    
    case enWithdrawAmount::TwoHundreds:
        WithdrawOrDepositAnAmount(200, vClients);
        break;
    
    case enWithdrawAmount::FiveHundreds:
        WithdrawOrDepositAnAmount(500, vClients);
        break;
    
    case enWithdrawAmount::OneThousand:
        WithdrawOrDepositAnAmount(1000, vClients);
        break;
    
    case enWithdrawAmount::TwoThousands:
        WithdrawOrDepositAnAmount(2000, vClients);
        break;
    
    case enWithdrawAmount::TenThousands:
        WithdrawOrDepositAnAmount(10000, vClients);
        break;
    
    case enWithdrawAmount::Exit:
        break;
    }


}

void QuickWithdrawScreen()
{
    vector <stClient> vClients = LoadClientsDataFromFile(ClientsNameFile);      
    cout << "=================================================";
    cout << "\n\t\tQuick Withdraw\n";
    cout << "=================================================\n";
    
    cout << "\t[1] 20\t\t[2] 50\n";
    cout << "\t[3] 100\t\t[4] 200\n";
    cout << "\t[5] 500\t\t[6] 1,000\n";
    cout << "\t[7] 2,000\t[8] 10,000\n";
    cout << "\t[9] Exit\t";

    cout << "\n================================================\n";
    cout << "Your Balance is: " << CheckBalanceOfAClient(vClients); ;
    PerformQuickWithdrawScreen((enWithdrawAmount) ReadANumber("\n\nPlease choice what to withdraw from 1 to 8 .... "), vClients);
}

bool isInteger(double num) 
{
    return num == std::floor(num); // Checks if num is equal to its floored value
    // Alternatives:
    // return num == std::trunc(num);
    // return std::fmod(num, 1.0) == 0.0;
}

bool CheckIfANumberIfAMultiplicationOf10(int Number)
{
    return (Number %10 == 0);
}

void NormalWithdrawScreen()
{
    vector <stClient> vClients = LoadClientsDataFromFile(ClientsNameFile);      
    cout << "=================================================";
    cout << "\n\t\tNormal Withdraw\n";
    cout << "=================================================\n";
    
    double AmountToWithdraw;
    bool validInput = false;

    do {
        cout << "\nPlease enter 10 or its multiplication: ";
        
        // Check if input is numeric
        if (!(cin >> AmountToWithdraw)) {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear(); // Clear error flags
            continue;
        }
        
        // Check if it's integer and multiple of 10
        if (!isInteger(AmountToWithdraw)) {
            cout << "Amount must be a whole number!\n";
            cin >> AmountToWithdraw;
        }
        else if (!CheckIfANumberIfAMultiplicationOf10(AmountToWithdraw)) {
            cout << "Amount must be a multiple of 10!\n";
            cin >> AmountToWithdraw;
        }
        else {
            validInput = true;
        }
        
    } while (!validInput);
        
    WithdrawOrDepositAnAmount(static_cast<int>(AmountToWithdraw), vClients);    
}

void ShowDepositScreen()
{
    vector <stClient> vClients = LoadClientsDataFromFile(ClientsNameFile);      
    cout << "=================================================";
    cout << "\n\t\tDeposite Screen\n";
    cout << "=================================================\n";

    int OrignalBalance = CheckBalanceOfAClient(vClients);
    int AmountToDeposit = 0;
    char UserAnswer = 'y';

    cout << "\nYour balance is: " << OrignalBalance;
    cout << "\n\nPlease enter the amount you want to deposit: ";
    cin >> AmountToDeposit;
    
    if (tolower(UserAnswer) != 'y')
    {
        cout << "\n\nDon't worry! The transaction has been cancelled.";
        return;
    }

    WithdrawOrDepositAnAmount(AmountToDeposit, vClients, -1);
}

void PerformMainMenuScreen(enMainMenuOptions MainMenuOption)
{
    switch (MainMenuOption)
    {
    case enMainMenuOptions::QuickWithdraw:
        system("cls");
        QuickWithdrawScreen();
        GoBackToMainMenu();
        break;

    case enMainMenuOptions::NormalWithdraw:
        system("cls");
        NormalWithdrawScreen();
        GoBackToMainMenu();
        break;
    
    case enMainMenuOptions::Deposit:
        system("cls");
        ShowDepositScreen();
        GoBackToMainMenu();
        break;
    
    case enMainMenuOptions::CheckBalance:
        system("cls");    
        ShowCheckBalanceScreen();  
        GoBackToMainMenu();
        break;

    case enMainMenuOptions::Logout:
        system("cls");
        Login();
        break;
    }
}

void ShowMainMenu()
{
    system("cls");

    cout << "\n=====================================================";
    cout << "\n\t\tAtm Main Menu Screen";
    cout << "\n=====================================================\n\n";

    cout << "\t[1] Quick Withdraw.\n";
    cout << "\t[2] Normal Withdraw.\n";
    cout << "\t[3] Deposit.\n";
    cout << "\t[4] Check Balance.\n";
    cout << "\t[5] Logout.\n";
    cout << "\n=====================================================\n\n";
    WelcomeMessage();
    PerformMainMenuScreen((enMainMenuOptions) ReadANumber("\nWhat do you want to do? (Please choose from 1 to 5): "));
}

void Login()
{


    vector <stClient> vClients = LoadClientsDataFromFile(ClientsNameFile);
    string AccountNumber , PinCode;
    bool LoginFaild = false;

    do
    {
        system("cls");
        cout << "\n--------------------------------------------";
        cout << "\n\t\tLogin Screen";
        cout << "\n--------------------------------------------";
        if (LoginFaild)
        {
            cout << "\nInvalid Username/Password!\nPlease try again...\n";
        }
        
        AccountNumber = ReadString("\nEnter account number: ");
        PinCode = ReadString("\nEnter pin code: ");
        
        LoginFaild = !CheckAccoutNumberAndPincodeOfClient(vClients,AccountNumber, PinCode, CurrentClient);
    
    } while (LoginFaild);
    

    ShowMainMenu();
}

int main()
{

    Login();

    return 0;
}