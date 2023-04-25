#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <random>
using namespace std;

int generate_id(set<int> generated_ids){
        random_device rd;
        default_random_engine  gen(rd());
        uniform_int_distribution<int> dist(100,1000);

        int id{dist(gen)};
        while (generated_ids.count(id)){
            id = dist(gen);
        }
        return id;
}

template<typename T>
void erase_element(vector<T> &v, T val){
    auto it = find(v.begin(),v.end(),val);
    if(it == v.end()){
        cout<<"Warning:: Element Doesn't exist";
        return;
    }
    v.erase(it);
}

vector<string> ReadFile(string file_path){
    vector<string> lines;
    fstream file(file_path.c_str());
    
    if (file.fail()) {
		cout << "\n\nERROR: Can't open the file\n\n";
		return lines;
	}

    string line;
    while(getline(file,line)){
        lines.push_back(line);
    }
    file.close();
    return lines;
}

void WriteFile(string file_path,vector <string> lines, bool append = true){
    auto status = ios::in | ios::out | ios::app;;
    if(!append)
        status = ios::in | ios::out | ios:: trunc;
    
    fstream file (file_path.c_str(),status);

    if(file.fail()) {
        cout<<"Couldn't open file"<<endl;
        return;
    }
    
    for(auto line:lines){
        file<<line<<"\n";
    }
    file.close();
}

vector<string> split_string(string &str, char delimeter = ','){
    vector <string> res;
    string temp = "";
    for(auto ch : str){
        if(ch == delimeter){ 
            res.push_back(temp);
            temp = "";
        }
        else{
            temp +=ch;
        }
    }
    return res;
}
int ReadInt(int low,int high){
    int val;
    cout<<"Enter a value between "<<low<<" - "<<high<<endl;
    cin>>val;
    if(low<=val && val<= high) return val;
    else{
        cout<<"Error: input is out of range"<<endl;
        return ReadInt(low,high);
    }
}

int ShowMenu(vector<string> choices){
    cout<<"Menu"<<endl;
    for(int i = 0;i<(int)choices.size();i++){
        cout<<"\t"<<i + 1<<": "<<choices[i]<<endl;
    }
    return ReadInt(1,(int)choices.size());
}


int ToInt(string &str){
    int num = 0;
    int sign = 1;

    int i = 0;
    if(str[0] == '-'){
        sign = -1;
        i = 1;
    }

    for (i; i < str.size(); i++) {
        if (isdigit(str[i])) {
            num *= 10;
            num += str[i] - '0';
        } else {
            cout<<"TypeError: ToInt requires numeric values"<<endl;
            return 0;
        }
    }
    return num * sign;
}

string ToString(int num){
    string str = "";
    if(num == 0) return "0";
    else{
        while(num){
            str += num%10 + '0';
            num/= 10;
        }
    }

    char temp;
    for(int i = 0;i<str.size()/2;i++){
        temp = str[i];
        str[i] =  str[str.size() - 1 - i];
        str[str.size() - 1 - i] = temp;
    }
    return str;
}

struct Question{
    int id{};
    int parent_id{}; //if has no parent: -1
    int to_id{};
    int from_id{};
    string question{};
    string answer{};
    bool AQ{};

    Question(int _id, int _parent_id, int _to_id, int _from_id, 
    string _q, string _ans, bool _AQ){
        id = _id;
        parent_id = _parent_id;
        to_id = _to_id;
        from_id = _from_id;
        question = _q;
        answer = _ans;
        AQ = _AQ;
    }

    Question(){
        id = to_id = from_id = 0;
        parent_id = -1;
        question = answer = "";
    }

    Question(vector <string> splitted_str){
        id = ToInt(splitted_str[0]);
        parent_id = ToInt(splitted_str[1]);
        from_id = ToInt(splitted_str[2]);
        to_id = ToInt(splitted_str[3]);
        question = splitted_str[4];
        answer = splitted_str[5];
        AQ = ToInt(splitted_str[6]);
    }
    string ToString(){
        string str = "";
        ostringstream oss(str);
        oss<<id<<","<<parent_id<<","<<from_id<<","<<to_id<<","<<question<<","<<answer<<","<<AQ<<",";
        return oss.str();
    }
};

struct User{
    int id{};
    string name{};
    string email{};
    string password{};
    bool allow_AQ;
    vector <int> question_id_from_me;
    vector <int> parent_id_to_me;

    User(){
        id = 0;
        name = email = password = "";
        question_id_from_me.clear();
        parent_id_to_me.clear();
    }

    User(int _id, string _name, string _email, string _pass, bool _allow){
        id = _id;
        name = _name;
        email = _email;
        password = _pass;
        allow_AQ = _allow;
        question_id_from_me.clear();
        parent_id_to_me.clear();
    }

    User(vector<string> splitted_str){
        id = ToInt(splitted_str[0]);
        name = splitted_str[1];
        email = splitted_str[2];
        password = splitted_str[3];
        allow_AQ = ToInt(splitted_str[4]);
    }
    string ToString(){
        string str ="";
        ostringstream oss(str);
        oss<<id<<","<<name<<","<<email<<","<<password<<","<<allow_AQ<<",";
        return oss.str();
    }
};

struct QuestionManager{
    map<int,vector<int>> map_parent_id_to_childs_ids{};
    map <int,Question> map_id_question_object;
    set <int> generated_ids;

    string QA_file_path = "QA.txt";

    QuestionManager(){
        map_id_question_object.clear();
        map_parent_id_to_childs_ids.clear();
        generated_ids.clear();
    }

    void LoadDataBase(){
        map_id_question_object.clear();
        map_parent_id_to_childs_ids.clear();
        generated_ids.clear();

        vector<string> lines = ReadFile(QA_file_path);
        for(auto line : lines){
            vector<string> splitted = split_string(line);
            Question Q(splitted);
            map_id_question_object[Q.id] = Q;
            generated_ids.insert(Q.id);
            if (Q.parent_id != -1){
                map_parent_id_to_childs_ids[Q.parent_id].push_back(Q.id);
            }
            else {
                    map_parent_id_to_childs_ids[Q.id] = vector<int>();
            }
        }
    }
    void LoadUserQuestions(User &user){
        user.parent_id_to_me.clear();
        user.question_id_from_me.clear();
        for (auto &pair : map_id_question_object){
            if (pair.second.to_id == user.id){
                user.parent_id_to_me.push_back(pair.first);
            }
            else if (pair.second.from_id == user.id){
                user.question_id_from_me.push_back(pair.first);
            }
        }
    }
    void UpdateDataBase(){
        vector<string> lines;
        for(auto&pair : map_id_question_object){
            lines.push_back(pair.second.ToString());
        }
        WriteFile(QA_file_path,lines,false);
    }

    void printQ(Question q){
        string from = "AQ";
        string first = "Question";
        if (q.parent_id !=  -1) first = "\tThread";
        if (q.AQ == 0) from = ToString (q.from_id);

        cout<<first <<" ("<<q.id<<"): "<<"From "<<from<<" Q: "<<q.question<<" ANS: "<<q.answer<<endl;
    }

    void PrintQ_to_me(User user){
        if (user.parent_id_to_me.size() == 0){
            cout<<"No questions"<<endl;
            return;
        }
        for(auto parent_id : user.parent_id_to_me){
            printQ(map_id_question_object[parent_id]);
            for (auto child_id : map_parent_id_to_childs_ids[parent_id])
                printQ(map_id_question_object[child_id]);
        }
    }

    void PrintQ_from_me(User user){
        if (user.question_id_from_me.size() == 0){
            cout<<"No questions"<<endl;
            return;
        }
        for (auto QID : user.question_id_from_me)
            printQ(map_id_question_object[QID]);
    }

    void AnswerQ(User user){
        int QID;
        cout<<"Enter question id: ";
        cin>>QID;
        while (!map_id_question_object.count(QID) || map_id_question_object[QID].to_id != user.id){
            cout<<"Enter a valid question id"<<endl;
            cin>>QID;
        }

        bool overwrite = 0;
        if (map_id_question_object[QID].answer != ""){
            cout<<"Question is already answered. Overwrite it? (1/0): "<<endl;
            cin>>overwrite;
            if(!overwrite) return;
        }

        string answer;
        cout<<"Enter your answer: ";
        cin.ignore();
        getline(cin,answer);
        map_id_question_object[QID].answer = answer;
    }

    void AskQ(User user,pair<int,int> to_user){
        char choice;
        int QID;
        string question;
        bool AQ;
        cout<<"Thread or Question ? (Q/T): ";
        cin>>choice;
        
        if (choice == 'T' || choice == 't'){
            cout<<"Enter the question ID: ";
            cin>>QID;

            while (!map_id_question_object.count(QID)){
                cout<<"Enter a valid ID: ";
                cin>>QID;
            }
            
            int temp;
            if (to_user.second == 0 && AQ){
                cout<<"user isn't allowing AQ enter -1 to exit or continue?";
                cin>>temp;
                if(temp == -1) return;
                AQ = 0;
            }
            int threadID = generate_id(generated_ids);
            Question q(threadID,QID,to_user.first,user.id,question,"",AQ);
            map_id_question_object[threadID] = q;
            map_parent_id_to_childs_ids[QID].push_back(threadID);
        }
        else if(choice == 'Q' || choice == 'q'){
            QID = generate_id(generated_ids);

            cout<<"AQ?";
            cin>>AQ;

            int temp;
            if (to_user.second == 0 && AQ){
                cout<<"user isn't allowing AQ enter -1 to exit or continue?";
                cin>>temp;
                if(temp == -1) return;
                AQ = 0;
            }

            cout<<"Enter the Question: ";
            cin.ignore();
            getline(cin,question);


            
            Question q(QID,QID,to_user.first,user.id,question,"",AQ);
            map_id_question_object[QID] = q;
        }
    }

    void DeleteQ(User &user){
        int QID {};
        cout<<"Enter quiestion ID: ";
        cin>>QID;

        auto it = find(user.question_id_from_me.begin(), user.question_id_from_me.end(), QID);
        while (it == user.question_id_from_me.end()){
            cout<<"Enter a valid ID: ";
            cin>>QID;
            it = find(user.question_id_from_me.begin(), user.question_id_from_me.end(), QID);
        }
    
        bool choice ;
        cout<<"Are you sure you want to delete? (1/0): ";
        cin>>choice;

        if(choice == 0) return;
        
        
        user.question_id_from_me.erase(it);
        map_id_question_object.erase(QID);

        for (auto &pair : map_parent_id_to_childs_ids){
            for(auto & child : pair.second){
                if(map_id_question_object[child].from_id == user.id){
                    erase_element(user.question_id_from_me,child);
                    map_id_question_object.erase(child);
                }
            }
        }
        map_parent_id_to_childs_ids.erase(QID);
    }

    void Feed(){
        for (auto &pair : map_parent_id_to_childs_ids){
            printQ(map_id_question_object[pair.first]);
            for (auto &child : map_parent_id_to_childs_ids[pair.first])
                printQ(map_id_question_object[child]);
        }
    }
};

struct UserManager{
    map<string,User> map_user_name_to_user_object;
    User current_user;

    string users_file_path = "users.txt";
    set <int> generated_ids;

    UserManager(){
        map_user_name_to_user_object.clear();
        generated_ids.clear();
    }

    void LoadDataBase(){
        UserManager();
        vector<string> lines = ReadFile(users_file_path);
        for(auto line : lines){
            vector <string> splitted = split_string(line);
            User U(splitted);
            map_user_name_to_user_object[U.name] = U;
            generated_ids.insert(U.id);
        }
    }

    void UpdateDataBase(){
        vector<string> lines;
        for(auto&pair : map_user_name_to_user_object){
            lines.push_back(pair.second.ToString());
        }
        WriteFile(users_file_path,lines,false);
    }

    void LogIn(){
        string user_name, password;
        cout<<"Enter user name and password: ";
        cin>>user_name>>password;
        if(map_user_name_to_user_object.count(user_name) && 
            map_user_name_to_user_object[user_name].password == password){
                cout<<"Welcome\n";
                current_user = map_user_name_to_user_object[user_name];
                return;
            }
        else{
            cout<<"Invalid Login\n";
            LogIn();
        }
    }
    void SignUp(){
        string user_name,email,password;
        bool allowAQ;
        cout<<"Enter username: ";
        cin>>user_name;

        while(map_user_name_to_user_object.count(user_name)){
            cout<<"User name already exists"<<endl;    
            cout<<"Enter username: ";
            cin>>user_name;
        }

        cout<<"Enter Email and password"<<endl;
        cin>>email>>password;

        cout<<"Allow AQ?: 1: YES, 0: NO ";
        cin>>allowAQ;

        User u(generate_id(generated_ids),user_name,email,password,allowAQ);
        current_user = u;
        map_user_name_to_user_object[u.name] = u;
    }

    pair<int,int> ReadUserID(){
        int id;
        cout<<"Enter user id: ";
        cin>>id;

        for(auto &pair : map_user_name_to_user_object){
            if (pair.second.id == id){
                return make_pair(id,pair.second.allow_AQ);
            }
    
        }
        cout<<"Invalid user id"<<endl;
        ReadUserID(); 
    }

    void ListUsers(){
        for (auto &pair : map_user_name_to_user_object){
            cout<<pair.second.name<<" "<<pair.second.email<<endl;
        }
    }
   
};
struct SystemManager{
    UserManager user_manager;
    QuestionManager question_manager;

    void LoadDataBase(){
        user_manager.LoadDataBase();
        question_manager.LoadDataBase();
    }

    void run(){
        LoadDataBase();
        vector <string> menu {"Login","Signup"};
            int choice = ShowMenu(menu);
            if(choice == 1){
                user_manager.LogIn();
            }
            else if(choice == 2){
                user_manager.SignUp();
                user_manager.UpdateDataBase();
            }

            question_manager.LoadUserQuestions(user_manager.current_user);
            menu = {"Print Q To Me", "Print Q From Me","Answer Q","Delete Q","ASK Q",
            "List System Users", "Feed", "Logout"};
            while (true){
                choice = ShowMenu(menu);
                    if (choice == 1)
                        question_manager.PrintQ_to_me(user_manager.current_user);
                    else if(choice == 2)
                        question_manager.PrintQ_from_me(user_manager.current_user);
                    else if(choice == 3){
                        question_manager.AnswerQ(user_manager.current_user);
                        question_manager.UpdateDataBase();
                    }
                    else if(choice == 4){
                        question_manager.DeleteQ(user_manager.current_user);
                        question_manager.UpdateDataBase();
                    }
                    else if(choice == 5){                    
                        pair <int,int> to_user = user_manager.ReadUserID();
                        question_manager.AskQ(user_manager.current_user,to_user);
                        question_manager.UpdateDataBase();
                    }
                    else if(choice == 6)
                        user_manager.ListUsers();
                    else if(choice == 7)
                        question_manager.Feed();
                    else if(choice == 8){
                        cout<<"You are welcome"<<endl;
                        run();
                    }             
                }
            }
};

int main()
{
    SystemManager sys;
    sys.run();
}