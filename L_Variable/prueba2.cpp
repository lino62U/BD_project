#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <string>
#include <iomanip> // Incluir la biblioteca iomanip para setprecision
#include <bitset>
#include <algorithm>
using namespace std;
// unidad basica, solo almacena referencias al .txt

string cargar_esquema(string table_name)
{
    ifstream archivo("esquema.txt");
    string aux = "";
    while (getline(archivo, aux))
    {
        string aux2 = "";
        for (auto i : aux)
        {
            if (i == '#')
            {
                if (aux2 == table_name){
                    archivo.close();
                    return aux;
                }
                aux2 = "";
            }
            else
                aux2 += i;
        }
    }
    return "";
}

void getInfoEsquema(string esquemabd, string &column_names, string &column_types)
{
    int temp = 0;
    string aux = "";
    for (auto i : esquemabd)
    {
        if (i == '#')
        {
            if (temp)
            {
                column_names += aux + "-";
                aux = "";
                temp = 0;
            }
            else
            {
                column_types += aux + "-";
                aux = "";
                temp = 1;
            }
        }
        else
            aux += i;
    }
}

void generateFileVarible(string table_name, string column_types){
    ifstream archivo(table_name+".txt",ios::binary);
    ofstream archivo2(table_name+"10.txt");

    vector<string> types;
    stringstream ss(column_types); // Crea un objeto stringstream con el string
    string token;

    while (getline(ss, token, '-')) { // Utiliza getline para dividir el string en tokens separados por '-'
        types.push_back(token); // Agrega cada token al typestor
    }
    int resI = 0;
    for(int i=0;i< types.size()-1;i++){
        if(types[i+1] == "int"){
            resI+= 8;
        }
        if(types[i+1] == "float"){
            resI+= 8;
        }
        if(types[i+1] == "double"){
            resI+= 8;
        }
        if(types[i+1] == "str"){
            resI+= 8;
        }
    }

    string linea;
    while(getline(archivo,linea)){
        int res = resI;
        stringstream ss2(linea);    
        string bitmap ="",token2;
        for(int i=0;i<types.size()-1;i++){
            getline(ss2,token2,'#');
            //ss2>>token2;
            if(token2.size()>0)
                bitmap += "0";
            else bitmap += "1";
        }
        stringstream ss3(linea);    
        string totalStr = "";
        archivo2<<bitmap;
        res += bitmap.size();
        for(int i=0;i<types.size()-1;i++){
            getline(ss3,token2,'#');
            if(types[i+1] == "int"){
                if(token2.size()  > 0){
                    string aa = token2;
                    for(int h=aa.size();h<8;h++) aa+= " ";
                    archivo2<<aa;
                }

            }
            else if(types[i+1] == "float"){
                if(token2.size() > 0){
                    string aa = token2;
                    for(int h=aa.size();h<8;h++) aa+= " ";
                    archivo2<<aa;
                }
            }
            else if(types[i+1] == "double"){
                if(token2.size() > 0){
                    string aa = token2;
                    for(int h=aa.size();h<8;h++) aa+= " ";
                    archivo2<<aa;
                }
            }
            else if(types[i+1] == "str"){
                if(token2.size() > 0){
                    string aa = token2;
                    string ww = to_string(res) + "," + to_string(token2.size());
                    for(int h=ww.size();h<8;h++) ww += " ";
                    totalStr += token2;
                    archivo2<<ww;
                    res += token2.size();
                }
            }
        }
        archivo2<<totalStr<<endl;
        bitmap.clear();
    }
}

int getSizeRecord(string table_name, string column_types,int record){
    ifstream archivo(table_name+"10.txt",ios::in);
    string linea;
    for(int i=0;i<record;i++){
        getline(archivo,linea);
    }
    vector<string> types;
    stringstream ss(column_types); // Crea un objeto stringstream con el string
    string token;

    while (getline(ss, token, '-')) { // Utiliza getline para dividir el string en tokens separados por '-'
        types.push_back(token); // Agrega cada token al typestor
    }
    string bitmap = linea.substr(0,types.size()-1);
    int size = 0;
    int pos = bitmap.size();
    for(int i=0;i<bitmap.size();i++){
        if(bitmap[i] == '0'){
            if(types[i+1] == "int" | types[i+1] == "float"){
                size+=4;
                pos +=8;
            }
            else if(types[1+i] == "double"){
                size += 8;
                pos += 8;
            }
            else if(types[1+i] == "str"){
                string temp = linea.substr(pos,8);
                string temp2;
                stringstream ss(temp);
                getline(ss, temp2, ',');
                getline(ss, temp2, ',');
                size += stoi(temp2);
                pos += 8;
            }
        }
    }
    cout<<size<<endl;
    return size;
}

int main()
{
    string table_name = "titanic";
    string esquemabd = cargar_esquema(table_name);
    string column_names = "", column_types = "";
    getInfoEsquema(esquemabd, column_names, column_types);
    //generateFileVarible(table_name,column_types);
    getSizeRecord(table_name,column_types,2);
    return 0;
}
