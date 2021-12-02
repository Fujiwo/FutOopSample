#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <initializer_list>
using namespace std;

namespace Helpers {
    class Helper
    {
    public:
        template<class T>
        static bool Contains(const string& text, const T& partText) {
            return text.find(partText) != std::string::npos;
        }
    };

    class UserInterface
    {
    public:
        static void Show(string message)
        {
            cout << message << endl;
        }

        static void ShowSeparator()
        {
            const auto serarator = "---------------------------";
            Show(serarator);
        }

        static string QueryText(string message)
        {
            for (; ;) {
                cout << message + ':';
                string userInputText;
                cin >> userInputText;
                if (!userInputText.empty())
                    return userInputText;
            }
        }

        static int QueryInteger(string message)
        {
            for (; ;) {
                cout << message + ':';
                string userInputText;
                cin >> userInputText;
                try {
                    return stoi(userInputText);
                } catch (const std::invalid_argument&) {
                    continue;
                } catch (const std::out_of_range&) {
                    continue;
                }
            }
        }
    };
} // namespace Helpers

namespace Models {
    class Department
    {
        int    code;
        string name;

    public:
        int Code() const
        {
            return code;
        }

        string Name() const
        {
            return name;
        }

        Department(int code = 1, string name = "") : code(code), name(name)
        {}
    };

    class Staff
    {
        int               number;
        string            name;
        const Department* department;

    public:
        int Number() const
        {
            return number;
        }

        string Name() const
        {
            return name;
        }

        const Department* Department() const
        {
            return department;
        }

        Staff(int number = 1, string name = "", const Models::Department* department = nullptr)
            : number(number), name(name), department(department)
        {}
    };

    class DepartmentList
    {
        map<int, Department> departments;

    public:
        DepartmentList()
        {}

        DepartmentList(initializer_list<Department> init)
        {
            for (auto department : init)
                push_back(department);
        }

        void push_back(const Department& department)
        {
            departments[department.Code()] = department;
        }

        class iterator
        {
            map<int, Department>::const_iterator mapIterator;

        public:
            iterator(map<int, Department>::const_iterator mapIterator) : mapIterator(mapIterator)
            {}

            bool operator !=(const iterator& another) const
            {
                return mapIterator != another.mapIterator;
            }

            iterator& operator ++()
            {
                ++mapIterator;
                return *this;
            }

            const Department& operator *()
            {
                return mapIterator->second;
            }
        };

        Department& operator[](int code)
        {
            return departments[code];
        }

        const Department* find(int code) const
        {
            auto iterator = departments.find(code);
            return iterator == departments.end() ? nullptr : &iterator->second;
        }

        iterator begin() const
        {
            return iterator(departments.begin());
        }

        iterator end() const
        {
            return iterator(departments.end());
        }
    };

    class StaffList
    {
        vector<Staff> staffs;

    public:
        StaffList()
        {}

        StaffList(initializer_list<Staff> init) : staffs(init)
        {}

        void push_back(const Staff& staff)
        {
            staffs.push_back(staff);
        }

        using iterator = vector<Staff>::const_iterator;

        iterator begin() const
        {
            return staffs.begin();
        }

        iterator end() const
        {
            return staffs.end();
        }
    };

    class Document
    {
        DepartmentList departmentList;
        StaffList      staffList;

    public:
        Document()
        {
            departmentList = {
                Department(121, "住宅商品開発室"),
                Department(315, "建設インフラ事業部"),
                Department(320, "測量地籍事業部"),
                Department(550, "ITS事業部")
            };

            staffList = {
                Staff(1641, "青島 幸雄", &departmentList[121]),
                Staff(1371, "東野 圭吾", &departmentList[315]),
                Staff(1397, "鈴木 善幸", &departmentList[320]),
                Staff(1151, "西野 隆子", &departmentList[320]),
                Staff(8951, "木田 優希", &departmentList[550]),
                Staff(1484, "白井 正人", &departmentList[550])
            };
        }

        DepartmentList& DepartmentList()
        {
            return departmentList;
        }

        StaffList& StaffList()
        {
            return staffList;
        }
    };
} // namespace Models

namespace Views {
    using namespace Helpers;
    using namespace Models;

    class StaffListView
    {
        const StaffList& staffList;

    public:
        StaffListView(const StaffList& staffList) : staffList(staffList)
        {}

        void Show(string searchText = "") const
        {
            for (const Staff& staff : staffList) {
                if (searchText.empty() || Helper::Contains(ToString(staff), searchText))
                    Show(staff);
            }
        }

        static string ToString(const Staff& staff)
        {
            return staff.Name() + "\t[" + to_string(staff.Number()) + "]" + (staff.Department() == nullptr ? "" : "\t(" + staff.Department()->Name() + ")");
        }

    private:
        void Show(const Staff& staff) const
        {
            UserInterface::Show(ToString(staff));
        }
    };
} // namespace Views

namespace Controllers {
    using namespace Helpers;
    using namespace Models;
    using namespace Views;

    class Command
    {
        Document& document;

    public:
        Command(Document& document) : document(document)
        {}

        bool Run()
        {
            Show();
            return Execute();
        }

        virtual char Shortcut() const = 0;
        virtual string Name() const = 0;

    protected:
        Document& GetDocument() const
        {
            return document;
        }

        virtual bool Execute()
        {
            return true;
        }

    private:
        void Show()
        {
            UserInterface::ShowSeparator();
            UserInterface::Show("【" + Name() + "】");
        }
    };

    class ShowStaffsCommand : public Command
    {
    public:
        ShowStaffsCommand(Document& document) : Command(document)
        {}

        virtual char Shortcut() const override
        {
            return 'q';
        }

        virtual string Name() const override
        {
            return "一覧";
        }

        virtual bool Execute() override
        {
            StaffListView(GetDocument().StaffList()).Show();
            return true;
        }
    };

    class SearchCommand : public Command
    {
    public:
        SearchCommand(Document& document) : Command(document)
        {}

        virtual char Shortcut() const override
        {
            return 's';
        }

        virtual string Name() const override
        {
            return "検索";
        }

        virtual bool Execute() override
        {
            auto searchText = UserInterface::QueryText("検索文字列");
            StaffListView(GetDocument().StaffList()).Show(searchText);
            return true;
        }
    };

    class AddStaffCommand : public Command
    {
    public:
        AddStaffCommand(Document& document) : Command(document)
        {}

        virtual char Shortcut() const override
        {
            return 'a';
        }

        virtual string Name() const override
        {
            return "社員追加";
        }

        virtual bool Execute() override
        {
            auto number            = UserInterface::QueryInteger("番号"      );
            auto name              = UserInterface::QueryText   ("氏名"      );
            auto departmentCode    = UserInterface::QueryInteger("部署コード");
            auto department        = GetDocument().DepartmentList().find(departmentCode);
            GetDocument().StaffList().push_back(Staff(number, name, department));

            StaffListView(GetDocument().StaffList()).Show();
            return true;
        }
    };

    class ExitCommand : public Command
    {
    public:
        ExitCommand(Document& document) : Command(document)
        {}

        virtual char Shortcut() const override
        {
            return 'x';
        }

        virtual string Name() const override
        {
            return "終了";
        }

        virtual bool Execute() override
        {
            return false;
        }
    };

    class CommandMenu
    {
        map<char, Command*> commandMap;

    public:
        CommandMenu(Document& document)
        {
            vector<Command*> commands = {
                new ShowStaffsCommand(document),
                new SearchCommand    (document),
                new AddStaffCommand  (document),
                new ExitCommand      (document)
            };

            for (auto  command : commands)
                commandMap[command->Shortcut()] = command;
        }

        virtual ~CommandMenu()
        {
            for (auto pair : commandMap)
                delete pair.second;
        }

        Command& GetCommand() const
        {
            for (; ;) {
                UserInterface::ShowSeparator();
                auto userInput = UserInterface::QueryText(GetMenuText());
                if (userInput.empty())
                    continue;
                auto shortcut = tolower(userInput[0]);
                if (commandMap.count(shortcut) != 0)
                    return *commandMap.at(shortcut);
            }
        }

    private:
        string GetMenuText() const
        {
            string menuText;
            size_t index = 0;
            for (auto pair : commandMap) {
                if (index++ > 0)
                    menuText += ' ';
                auto command = pair.second;
                menuText += command->Name() + "(" + command->Shortcut() + ")";
            }
            return menuText;
        }
    };
} // namespace Controllers

namespace Application {
    using namespace Models;
    using namespace Helpers;
    using namespace Controllers;

    class Program
    {
        Document    document;
        CommandMenu commandMenu;

    public:
        Program() : commandMenu(document)
        {}

        void Run()
        {
            const string applicationName = "[社員名簿]";

            UserInterface::Show(applicationName);
            for (; ;) {
                if (!commandMenu.GetCommand().Run())
                    break;
            }
        }
    };
} // namespace Application

int main()
{
    Application::Program().Run();
}
