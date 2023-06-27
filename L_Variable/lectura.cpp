#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <string>
#include <iomanip> // Incluir la biblioteca iomanip para setprecision
#include <bitset>

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
    string table_name = "instructor";
    string esquemabd = cargar_esquema(table_name);
    string column_names = "", column_types = "";
    getInfoEsquema(esquemabd, column_names, column_types);

    ReadFile(0, table_name, column_types);
    // ReadFile(47, table_name, column_types);
    // ReadFile(94, table_name, column_types);
    // ReadFile(141, table_name, column_types);

    return 0;
}