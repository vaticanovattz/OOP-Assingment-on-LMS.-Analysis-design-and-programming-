#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <type_traits>

using namespace std;

const string USERS_FILE = "users.txt";
const string UNITS_FILE = "units.txt";
const string REG_FILE = "registrations.txt";
const string MATERIALS_FILE = "materials.txt";
const string MARKS_FILE = "marks.txt";

struct User {
    string username, password, role, fullName, regNo;
};
struct Unit {
    string code, name, lecturer;
};
struct Registration {
    string regNo, unitCode, semester;
};
struct Material {
    string unitCode, type, title, description;
};
struct Mark {
    string regNo, unitCode;
    double assignment = -1, cat = -1, exam = -1;
};

vector<string> split(const string& line, char delim='|') {
    vector<string> out;
    string item;
    stringstream ss(line);
    while (getline(ss, item, delim)) out.push_back(item);
    if (!line.empty() && line.back() == delim) out.push_back("");
    return out;
}
string clean(string s) {
    replace(s.begin(), s.end(), '|', '/');
    replace(s.begin(), s.end(), '\n', ' ');
    replace(s.begin(), s.end(), '\r', ' ');
    return s;
}
void pauseScreen() {
    cout << "\nPress ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
string readLine(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return clean(s);
}
int readInt(const string& prompt, int minVal, int maxVal) {
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);
        stringstream ss(line);
        int value;
        char extra;
        if (ss >> value && !(ss >> extra) && value >= minVal && value <= maxVal)
            return value;
        cout << "Invalid input. Enter a number from " << minVal << " to " << maxVal << ".\n";
    }
}
double readMark(const string& prompt) {
    while (true) {
        cout << prompt << " (0-100, or -1 to leave unchanged): ";
        string line;
        getline(cin, line);
        stringstream ss(line);
        double v; char extra;
        if ((ss >> v) && !(ss >> extra) && v >= -1 && v <= 100) return v;
        cout << "Invalid mark. Use -1 to leave unchanged, or a value from 0 to 100.\n";
    }
}
vector<User> loadUsers() {
    vector<User> v; ifstream f(USERS_FILE); string line;
    while (getline(f,line)) {
        auto p=split(line);
        if(p.size()>=5) v.push_back({p[0],p[1],p[2],p[3],p[4]});
    } return v;
}
vector<Unit> loadUnits() {
    vector<Unit> v; ifstream f(UNITS_FILE); string line;
    while(getline(f,line)){auto p=split(line); if(p.size()>=3)v.push_back({p[0],p[1],p[2]});}
    return v;
}
vector<Registration> loadRegs() {
    vector<Registration> v; ifstream f(REG_FILE); string line;
    while(getline(f,line)){auto p=split(line); if(p.size()>=3)v.push_back({p[0],p[1],p[2]});}
    return v;
}
vector<Material> loadMaterials() {
    vector<Material> v; ifstream f(MATERIALS_FILE); string line;
    while(getline(f,line)){auto p=split(line); if(p.size()>=4)v.push_back({p[0],p[1],p[2],p[3]});}
    return v;
}
vector<Mark> loadMarks() {
    vector<Mark> v; ifstream f(MARKS_FILE); string line;
    while(getline(f,line)){
        auto p=split(line); if(p.size()>=5){
            try { v.push_back({p[0],p[1],stod(p[2]),stod(p[3]),stod(p[4])}); } catch(...) {}
        }
    } return v;
}
template<typename T> void saveAll(const string& file, const vector<T>& data) {
    ofstream f(file);
    if(!f){cout<<"Error: could not write "<<file<<".\n"; return;}
    for(const auto& x:data) {
        if constexpr (is_same<T,User>::value) f<<x.username<<'|'<<x.password<<'|'<<x.role<<'|'<<x.fullName<<'|'<<x.regNo<<'\n';
        else if constexpr (is_same<T,Unit>::value) f<<x.code<<'|'<<x.name<<'|'<<x.lecturer<<'\n';
        else if constexpr (is_same<T,Registration>::value) f<<x.regNo<<'|'<<x.unitCode<<'|'<<x.semester<<'\n';
        else if constexpr (is_same<T,Material>::value) f<<x.unitCode<<'|'<<x.type<<'|'<<x.title<<'|'<<x.description<<'\n';
        else if constexpr (is_same<T,Mark>::value) f<<x.regNo<<'|'<<x.unitCode<<'|'<<x.assignment<<'|'<<x.cat<<'|'<<x.exam<<'\n';
    }
}
void seedData() {
    ifstream test(USERS_FILE);
    if(test.good()) return;
    vector<User> users = {
        {"admin","admin123","admin","System Administrator",""},
        {"lecturer","lecturer123","lecturer","Demo Lecturer",""},
        {"student","student123","student","Demo Student","SCT/001/2025"}
    };
    vector<Unit> units = {
        {"DIT0201","Introduction to Computing","lecturer"},
        {"DIT0202","Programming in C++","lecturer"},
        {"DIT0203","Database Management","lecturer"}
    };
    vector<Registration> regs = {{"SCT/001/2025","DIT0201","2026-S1"}};
    vector<Material> mats = {{"DIT0201","Notes","Week 1 Notes","Introduction and course overview"}};
    vector<Mark> marks = {{"SCT/001/2025","DIT0201",75,68,72}};
    saveAll(USERS_FILE,users); saveAll(UNITS_FILE,units);
    saveAll(REG_FILE,regs); saveAll(MATERIALS_FILE,mats); saveAll(MARKS_FILE,marks);
}
bool userExists(const vector<User>& users, const string& username) {
    return any_of(users.begin(),users.end(),[&](const User& u){return u.username==username;});
}
bool unitExists(const vector<Unit>& units, const string& code) {
    return any_of(units.begin(),units.end(),[&](const Unit& u){return u.code==code;});
}
bool isRegistered(const vector<Registration>& regs, const string& reg, const string& code, const string& sem) {
    return any_of(regs.begin(),regs.end(),[&](const Registration& r){return r.regNo==reg && r.unitCode==code && r.semester==sem;});
}
bool lecturerOwns(const Unit& u, const User& current) {
    return current.role=="admin" || u.lecturer==current.username;
}
void showUnitsForStudent(const User& current, bool lmsView) {
    auto regs=loadRegs(); auto units=loadUnits();
    cout << (lmsView ? "\n--- LMS: YOUR REGISTERED UNITS ---\n" : "\n--- STUDENT PORTAL: REGISTERED UNITS ---\n");
    bool found=false;
    for(const auto& r:regs) if(r.regNo==current.regNo) {
        for(const auto& u:units) if(u.code==r.unitCode) {
            cout<<u.code<<" | "<<u.name<<" | Semester: "<<r.semester<<"\n"; found=true;
        }
    }
    if(!found) cout<<"No registered units found.\n";
}
void showMarks(const User& current) {
    auto marks=loadMarks(); auto units=loadUnits();
    cout<<"\n--- MARKS (shared records for portal and LMS) ---\n";
    bool found=false;
    for(const auto& m:marks) if(m.regNo==current.regNo) {
        string name=m.unitCode;
        for(const auto& u:units) if(u.code==m.unitCode) name=u.name;
        cout<<m.unitCode<<" - "<<name<<"\n";
        cout<<"  Assignment: "<<(m.assignment<0?string("Not entered"):to_string(m.assignment))<<"\n";
        cout<<"  CAT: "<<(m.cat<0?string("Not entered"):to_string(m.cat))<<"\n";
        cout<<"  Examination: "<<(m.exam<0?string("Not entered"):to_string(m.exam))<<"\n";
        found=true;
    }
    if(!found) cout<<"No marks have been recorded yet.\n";
}
void studentRegisterUnit(const User& current) {
    auto units=loadUnits(); auto regs=loadRegs();
    string code=readLine("Enter unit code: ");
    auto it=find_if(units.begin(),units.end(),[&](const Unit& u){return u.code==code;});
    if(it==units.end()){cout<<"Unit not found.\n"; return;}
    string sem=readLine("Enter semester (e.g. 2026-S1): ");
    if(isRegistered(regs,current.regNo,code,sem)){cout<<"You are already registered for this unit in that semester.\n";return;}
    regs.push_back({current.regNo,code,sem}); saveAll(REG_FILE,regs);
    cout<<"Unit registered successfully. It will now appear in the LMS because both views use the same records.\n";
}
void showMaterialsForStudent(const User& current) {
    auto regs=loadRegs(); auto mats=loadMaterials();
    vector<string> codes;
    for(const auto& r:regs) if(r.regNo==current.regNo) codes.push_back(r.unitCode);
    cout<<"\n--- LMS MATERIALS FOR YOUR UNITS ---\n"; bool found=false;
    for(const auto& m:mats) if(find(codes.begin(),codes.end(),m.unitCode)!=codes.end()) {
        cout<<m.unitCode<<" | "<<m.type<<" | "<<m.title<<"\n  "<<m.description<<"\n"; found=true;
    }
    if(!found) cout<<"No materials posted for your registered units.\n";
}
void studentMenu(const User& current) {
    while(true) {
        cout<<"\n=== STUDENT MENU: "<<current.fullName<<" ===\n"
            <<"1. Student Portal: register a unit\n2. Student Portal: view registered units\n"
            <<"3. Student Portal: view marks\n4. LMS: view my registered units\n"
            <<"5. LMS: view notes/assignments/CATs\n6. LMS: view marks\n0. Log out\n";
        int c=readInt("Choose: ",0,6);
        if(c==0) return;
        if(c==1) studentRegisterUnit(current);
        else if(c==2) showUnitsForStudent(current,false);
        else if(c==3) showMarks(current);
        else if(c==4) showUnitsForStudent(current,true);
        else if(c==5) showMaterialsForStudent(current);
        else if(c==6) showMarks(current);
        pauseScreen();
    }
}
void lecturerPostMaterial(const User& current) {
    auto units=loadUnits(); auto mats=loadMaterials();
    string code=readLine("Unit code: ");
    auto it=find_if(units.begin(),units.end(),[&](const Unit& u){return u.code==code && lecturerOwns(u,current);});
    if(it==units.end()){cout<<"Unit not found or not assigned to your account.\n";return;}
    cout<<"Material type: 1. Notes  2. Assignment  3. CAT\n";
    int t=readInt("Choose type: ",1,3);
    string type=(t==1?"Notes":(t==2?"Assignment":"CAT"));
    string title=readLine("Title: ");
    string desc=readLine("Description or file location: ");
    mats.push_back({code,type,title,desc}); saveAll(MATERIALS_FILE,mats);
    cout<<"Material listing saved.\n";
}
void lecturerEnterMarks(const User& current) {
    auto units=loadUnits(); auto regs=loadRegs(); auto marks=loadMarks(); auto users=loadUsers();
    string code=readLine("Unit code: ");
    auto uit=find_if(units.begin(),units.end(),[&](const Unit& u){return u.code==code && lecturerOwns(u,current);});
    if(uit==units.end()){cout<<"Unit not found or not assigned to your account.\n";return;}
    string reg=readLine("Student registration number: ");
    bool registered=any_of(regs.begin(),regs.end(),[&](const Registration& r){return r.regNo==reg && r.unitCode==code;});
    if(!registered){cout<<"That student is not registered for this unit.\n";return;}
    bool knownStudent=any_of(users.begin(),users.end(),[&](const User& u){return u.role=="student" && u.regNo==reg;});
    if(!knownStudent){cout<<"Student account not found.\n";return;}
    auto it=find_if(marks.begin(),marks.end(),[&](const Mark& m){return m.regNo==reg && m.unitCode==code;});
    if(it==marks.end()){marks.push_back({reg,code,-1,-1,-1});it=prev(marks.end());}
    cout<<"Enter -1 to leave an existing mark unchanged. For a new mark, enter 0-100.\n";
    double a=readMark("Assignment mark");
    double c=readMark("CAT mark");
    double e=readMark("Examination mark");
    if(a>=0) it->assignment=a;
    if(c>=0) it->cat=c;
    if(e>=0) it->exam=e;
    saveAll(MARKS_FILE,marks);
    cout<<"Marks saved. The student portal and LMS will display these same marks.\n";
}
void lecturerViewRoster(const User& current) {
    auto units=loadUnits(); auto regs=loadRegs(); auto users=loadUsers();
    string code=readLine("Unit code: ");
    auto uit=find_if(units.begin(),units.end(),[&](const Unit& u){return u.code==code && lecturerOwns(u,current);});
    if(uit==units.end()){cout<<"Unit not found or not assigned to your account.\n";return;}
    cout<<"\nStudents registered for "<<uit->code<<" - "<<uit->name<<":\n";
    bool found=false;
    for(const auto& r:regs) if(r.unitCode==code) {
        for(const auto& u:users) if(u.role=="student" && u.regNo==r.regNo) {
            cout<<u.regNo<<" | "<<u.fullName<<" | "<<r.semester<<"\n";found=true;
        }
    }
    if(!found)cout<<"No students registered.\n";
}
void lecturerMenu(const User& current) {
    while(true) {
        cout<<"\n=== LECTURER LMS MENU: "<<current.fullName<<" ===\n"
            <<"1. View registered students for a unit\n2. Post notes/assignment/CAT listing\n"
            <<"3. Enter/update assignment, CAT and exam marks\n4. View unit list\n0. Log out\n";
        int c=readInt("Choose: ",0,4); if(c==0)return;
        if(c==1)lecturerViewRoster(current);
        else if(c==2)lecturerPostMaterial(current);
        else if(c==3)lecturerEnterMarks(current);
        else if(c==4){for(const auto& u:loadUnits())if(lecturerOwns(u,current))cout<<u.code<<" | "<<u.name<<"\n";}
        pauseScreen();
    }
}
void adminAddUser() {
    auto users=loadUsers();
    cout<<"1. Student  2. Lecturer\n";
    int type=readInt("Choose account type: ",1,2);
    string username=readLine("Username: ");
    if(username.empty() || userExists(users,username)){cout<<"Username is empty or already exists.\n";return;}
    string password=readLine("Temporary password: ");
    string name=readLine("Full name: ");
    string reg="";
    string role=type==1?"student":"lecturer";
    if(type==1) {
        reg=readLine("Student registration number: ");
        if(reg.empty() || any_of(users.begin(),users.end(),[&](const User& u){return u.regNo==reg && !reg.empty();})){
            cout<<"Registration number is empty or already exists.\n";return;
        }
    }
    users.push_back({username,password,role,name,reg}); saveAll(USERS_FILE,users);
    cout<<"Account created.\n";
}
void adminMenu(const User& current) {
    while(true) {
        cout<<"\n=== ADMIN MENU: "<<current.fullName<<" ===\n1. Add student/lecturer account\n2. List users\n3. List units\n0. Log out\n";
        int c=readInt("Choose: ",0,3); if(c==0)return;
        if(c==1)adminAddUser();
        else if(c==2){for(const auto& u:loadUsers())cout<<u.username<<" | "<<u.role<<" | "<<u.fullName<<(u.regNo.empty()?"":" | "+u.regNo)<<"\n";}
        else if(c==3){for(const auto& u:loadUnits())cout<<u.code<<" | "<<u.name<<" | Lecturer: "<<u.lecturer<<"\n";}
        pauseScreen();
    }
}
int main() {
    seedData();
    cout<<"====================================================\n";
    cout<<" JKUAT STUDENT INFORMATION SYSTEM AND LMS (DEMO)\n";
    cout<<"====================================================\n";
    cout<<"Demo accounts (change before any real use):\n"
        <<" Admin:    admin / admin123\n"
        <<" Lecturer: lecturer / lecturer123\n"
        <<" Student:  student / student123\n";
    while(true) {
        cout<<"\n1. Login\n0. Exit\n";
        int choice=readInt("Choose: ",0,1);
        if(choice==0){cout<<"Goodbye.\n";break;}
        string username=readLine("Username: ");
        string password=readLine("Password: ");
        auto users=loadUsers();
        auto it=find_if(users.begin(),users.end(),[&](const User& u){return u.username==username && u.password==password;});
        if(it==users.end()){cout<<"Invalid username or password.\n";continue;}
        User current=*it;
        if(current.role=="student")studentMenu(current);
        else if(current.role=="lecturer")lecturerMenu(current);
        else if(current.role=="admin")adminMenu(current);
        else cout<<"Unknown account role.\n";
    }
    return 0;
}
