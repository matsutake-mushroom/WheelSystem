#include "CSV.cpp"
#include <iostream>
#include <map>

using namespace std;
typedef std::vector<std::string> stringArray;
stringArray split(string input, char delim)
{
    stringArray elems;
    string item;
    for (char ch: input) {
        if (ch == delim) {
            if (!item.empty())
                elems.push_back(item);
            item.clear();
        }
        else {
            item += ch;
        }
    }
    if (!item.empty()){
        elems.push_back(item);
    }
    return elems;
}

int TEST(string filename){
    CSV score(filename);
    map<int, double> soundmap;
    int beat_time = stoi(split(score[0][1],':')[1]);
    
    for(auto s :score[1]){
        if(s != string("#")){
            auto line = split(s,':');
            soundmap[stoi(line[0].substr(1))] = stof(line[1].substr(0,line[1].length() - 1));
            cout << line[0] << "," << line[1] << endl;
        }
    }
    
    cout << "*" << flush;
    
    for(int i = 2;i<score.Row; ++i){
        cout << soundmap[stoi(score[i][0])] << "kHz" << stof(score[i][1]) * beat_time << "ms" << endl; 
    }
    
    cout << beat_time << endl;
    return 0;
}

int main(){
    TEST("fast.csv");
    return 0;
}