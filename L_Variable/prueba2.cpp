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

void ReadFile(int inicio, string table_name, string column_types)
{
    
    ifstream archivo2(table_name + ".bin", ios::binary);
    archivo2.seekg(inicio, ios::beg);
    int temp = 0;
    string aux = "";
    int count = 0;
    for (auto i : column_types)
    {
        if (i == '-')
        {
            break;
        }
        else
            count++;
    }
    string types = column_types.substr(count + 1, column_types.size() - count);
    string result = "",aux3="";
    ostringstream oss;
    int pos = 0;
    for (auto i : types)
    {
        if (i == '-')
        {
            archivo2.seekg(pos,ios::beg);
            if (aux == "str")
            {
                float v1;
                archivo2.read(reinterpret_cast<char *>(&v1), sizeof(v1));
                pos += sizeof(v1);
                oss << v1;
                aux3 += oss.str();
                string aux1="";
                string aux2="";
                int j=0;
                for(j;j<aux3.size();j++){
                    if(aux3[j] == '.'){
                        break;
                    }
                    else aux1 += aux3[j];
                }
                aux2 = aux3.substr(j+1,aux3.size()-(j+1)-1);
                oss.str(""); // Establecer una cadena vacía
                oss.clear();
                aux3 = "";
                int ml = stoi(aux1), ml2 = stoi(aux2);
                archivo2.seekg(inicio+ml,ios::beg);
                char * w = new char[ml2+1];
                w[ml2] = '\0';
                archivo2.read(w,ml2);
                result += w;
                result += " ";
                archivo2.seekg(pos,ios::beg);    
                
            }
            else if (aux == "int" || aux == "float")
            {
                float v1;
                archivo2.read(reinterpret_cast<char *>(&v1), sizeof(v1));
                pos += sizeof(v1);
                oss<<v1;
                result += oss.str() + " ";
                oss.str(""); // Establecer una cadena vacía
                oss.clear();
            }
            else if (aux == "double")
            {
                double v1;
                archivo2.read(reinterpret_cast<char *>(&v1), sizeof(v1));
                pos += sizeof(v1);
                oss<<v1;
                result += oss.str() + " ";
                oss.str(""); // Establecer una cadena vacía
                oss.clear();
            }
            aux = "";
        }
        else
            aux += i;
    }

    char byte;
    archivo2.read(&byte, 1);
    bitset<4> bits(byte);
    string binaryString = bits.to_string();
    cout<<"bitmap: "<<binaryString<<endl;
    cout<<types<<endl;
    cout<<result<<endl;
}

int main()
{
    string table_name = "titanic";
    string esquemabd = cargar_esquema(table_name);
    string column_names = "", column_types = "";
    getInfoEsquema(esquemabd, column_names, column_types);

    // ReadFile(0, table_name, column_types);
    string db = "titanic";
    ifstream archivo(db+".csv",ios::binary);
    ofstream archivo2(db+".bin",ios::binary|ios::app);


    vector<string> types;
    stringstream ss(column_types); // Crea un objeto stringstream con el string
    string token;

    while (getline(ss, token, '-')) { // Utiliza getline para dividir el string en tokens separados por '-'
        types.push_back(token); // Agrega cada token al typestor
    }
    ss.clear();
    string linea,token2;
    getline(archivo,linea);
    cout<<"linea: "<<linea<<endl;
    stringstream ss2(linea);    
    string bitmap ="";
    
    for(int i=0;i<types.size()-1;i++){
        getline(ss2,token2,'#');
        if(token2.size()>0)
            bitmap += "0";
        else bitmap += "1";
    }
    cout<<"Bitmap: "<<bitmap<<endl;
    int res = 0;
    for(int i=0;i< bitmap.size();i++){
        //if(bitmap[i] == '0'){
            if(types[i+1] == "int"){
                res+= 4;
            }
            if(types[i+1] == "float"){
                res+= 4;
            }
            if(types[i+1] == "double"){
                res+= 8;
            }
            if(types[i+1] == "str"){
                res+= 4;
            }
        
    }
    //string linea,token2;
    archivo.seekg(0,ios::beg);
    getline(archivo,linea);
    cout<<"linea: "<<linea<<endl;
    stringstream ss3(linea);    
    string totalStr = "";
    int pos = 0;
    archivo2.write(bitmap.c_str(),bitmap.size());
    res += bitmap.size();
    for(int i=0;i<types.size()-1;i++){
        getline(ss3,token2,'#');
        if(types[i+1] == "int"){
            if(token2.size()  > 0){
                int tok = stoi(token2);
                archivo2.write(reinterpret_cast<char*>(&tok),sizeof(tok));
                cout<<"int: "<<token2<<endl;
                pos += 4;
            }

        }
        else if(types[i+1] == "float"){
            if(token2.size() > 0){
                float tok = stof(token2);
                archivo2.write(reinterpret_cast<char*>(&tok),sizeof(tok));
                cout<<"float: "<<token2<<endl;
                pos += 4;
            }
        }
        else if(types[i+1] == "double"){
            if(token2.size() > 0){
                double tok = stod(token2);
                archivo2.write(reinterpret_cast<char*>(&tok),sizeof(tok));
                cout<<"double: "<<token2<<endl;
                pos += 4;
            }
        }
        else if(types[i+1] == "str"){
            if(token2.size() > 0){
                 token2.erase(std::remove_if(token2.begin(), token2.end(),
                                     [](char c) { return c == '\n' || c == '\0'; }),
                      token2.end());
                bitmap += "0";
                float aux;
                string aux2="";
                aux2 += to_string(res) + "." + to_string(token2.size()) + "2";
                res += token2.size();
                totalStr += token2;
                aux = stof(aux2);
                cout<<"strrr: "<<aux<<endl;
                float q = aux;
                archivo2.write(reinterpret_cast<char*>(&q),sizeof(q));
                pos += 4;
            }

        }
    }
    archivo2.write(totalStr.c_str(),totalStr.size());
    archivo2.write("\n",1);
    return 0;
}