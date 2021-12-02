#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Department
{
    int    code;
    string name;
};

struct Staff
{
    int               number;
    string            name;
    const Department* department;
};

int main()
{
    Department department1 = { 121, "住宅商品開発室"     };
    Department department2 = { 315, "建設インフラ事業部" };
    Department department3 = { 320, "測量地籍事業部"     };
    Department department4 = { 550, "ITS事業部"          };
    vector<Department> departmentList = { department1, department2, department3, department4 };

    Staff staff1 = { 1641, "青島 幸雄", &departmentList[0] };
    Staff staff2 = { 1371, "東野 圭吾", &departmentList[1] };
    Staff staff3 = { 1397, "鈴木 善幸", &departmentList[2] };
    Staff staff4 = { 1151, "西野 隆子", &departmentList[2] };
    Staff staff5 = { 8951, "木田 優希", &departmentList[3] };
    Staff staff6 = { 1484, "白井 正人", &departmentList[3] };
    vector<Staff> staffList = { staff1, staff2, staff3, staff4, staff5, staff6 };

    cout << "[社員名簿]" << endl;

    for (bool exitFlag = false; !exitFlag;) {
        char shortcut = '\0';
        for (; ;) {
            cout << "---------------------------" << endl;
            cout << "社員追加(a) 一覧(q) 検索(s) 終了(x) :" << endl;

            string userInputText;
            cin >> userInputText;
            if (!userInputText.empty()) {
                shortcut = tolower(userInputText[0]);
                break;
            }
        }

        switch (shortcut) {
            case 'a': {
                cout << "---------------------------" << endl;
                cout << "【社員追加】" << endl;

                int staffNumber = 0;
                for (; ;) {
                    cout << "番号:";
                    string userInputText;
                    cin >> userInputText;
                    try {
                        staffNumber = stoi(userInputText);
                        break;
                    }
                    catch (const std::invalid_argument&) {
                        continue;
                    }
                    catch (const std::out_of_range&) {
                        continue;
                    }
                }

                string staffName;
                for (; ;) {
                    cout << "名前:";
                    cin >> staffName;
                    if (!staffName.empty())
                        break;
                }

                int departmentCode = 0;
                for (; ;) {
                    cout << "部署コード:";
                    string userInputText;
                    cin >> userInputText;
                    try {
                        departmentCode = stoi(userInputText);
                        break;
                    }
                    catch (const std::invalid_argument&) {
                        continue;
                    }
                    catch (const std::out_of_range&) {
                        continue;
                    }
                }

                const Department* staffDepartment = nullptr;
                for (const Department& department : departmentList) {
                    if (department.code == departmentCode)
                        staffDepartment = &department;
                }

                Staff staff = { staffNumber, staffName, staffDepartment };
                staffList.push_back(staff);

                for (Staff staff : staffList) {
                    string staffText = staff.name + "\t[" + to_string(staff.number) + "]" + (staff.department == nullptr ? "" : "\t(" + staff.department->name + ")");
                    cout << staffText << endl;
                }
            }
            break;

        case 'q':
            cout << "---------------------------" << endl;
            cout << "【一覧】" << endl;
            for (Staff staff : staffList) {
                string staffText = staff.name + "\t[" + to_string(staff.number) + "]" + (staff.department == nullptr ? "" : "\t(" + staff.department->name + ")");
                cout << staffText << endl;
            }
            break;

        case 's':
            {
                cout << "---------------------------" << endl;
                cout << "【検索】" << endl;
                string searchText;
                for (; ;) {
                    cout << "検索文字列:";
                    cin >> searchText;
                    if (!searchText.empty())
                        break;
                }
                for (Staff staff : staffList) {
                    string staffText = staff.name + "\t[" + to_string(staff.number) + "]" + (staff.department == nullptr ? "" : "\t(" + staff.department->name + ")");
                    if (staffText.find(searchText) != std::string::npos)
                        cout << staffText << endl;
                }
            }
            break;

        case 'x':
            exitFlag = true;
            break;

        default:
            break;
        }
    }
}
