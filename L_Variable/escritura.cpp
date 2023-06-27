/*
#include <iostream>
#include <fstream>

int main() {
    std::ofstream archivo("archivo.bin", std::ios::binary | std::ios::app);

    if (archivo.is_open()) {
        //int numero = 123456789;
        int numero = 65948;
        const char * h = "izoqj";
        // Escribir el número en el archivo
        archivo.write(reinterpret_cast<const char*>(&numero), sizeof(numero));
        archivo.write(h,5);
        archivo.close();

        std::cout << "El número se ha agregado al archivo binario correctamente." << std::endl;
    } else {
        std::cout << "No se pudo abrir el archivo." << std::endl;
    }

    return 0;
}
*/
/*
#include <iostream>
#include <fstream>

int main() {
    std::ifstream archivo("archivo.bin", std::ios::binary | std::ios::in);

    if (archivo.is_open()) {
        // Obtener el tamaño del archivo
        archivo.seekg(0, std::ios::end);
        std::streampos tamano = archivo.tellg();
        archivo.seekg(0, std::ios::beg);

        // Crear un buffer para almacenar los datos
        char* buffer = new char[tamano];

        // Leer el contenido del archivo
        archivo.read(buffer, tamano);

        // Mostrar los datos leídos
        std::cout << "Contenido del archivo binario:" << std::endl;
        std::cout.write(buffer, tamano);

        // Liberar la memoria del buffer
        delete[] buffer;

        archivo.close();
    } else {
        std::cout << "No se pudo abrir el archivo." << std::endl;
    }

    return 0;
}
*/

#include <iostream>
#include <fstream>
#include <bitset>
#include <cstring>
#include <string>
using namespace std;
int main() {
    // std::ifstream archivo("instructor.bin", std::ios::binary);

    // if (archivo.is_open()) {
    //     // Obtener el tamaño del archivo
    //     archivo.seekg(0, std::ios::end);
    //     std::streampos tamano = archivo.tellg();
    //     archivo.seekg(0, std::ios::beg);

    //     // Crear un buffer para almacenar los datos
    //     char* buffer = new char[tamano];

    //     // Leer el contenido del archivo
    //     archivo.read(buffer, tamano);

    //     // Interpretar los bytes como números enteros
    //     float* numeros = reinterpret_cast<float*>(buffer);
    //     float cantidadNumeros = tamano / sizeof(float);

    //     // Mostrar los números leídos
    //     std::cout << "Números enteros leídos del archivo:" << std::endl;
    //     for (int i = 0; i < cantidadNumeros; ++i) {
    //         std::cout << numeros[i] << " ";
    //     }
    //     std::cout << std::endl;

    //     // Liberar la memoria del buffer
    //     delete[] buffer;

    //     archivo.close();
    // } else {
    //     std::cout << "No se pudo abrir el archivo." << std::endl;
    // }

    float a = 21.52;
    float b = 26.102;
    float c = 36.102;
    double d = 65000;
    string bitmap = "0000";
    ofstream archivo("instructor.bin",ios::binary | ios::in);
    bitset<4> bits(bitmap);

    archivo.seekp(0, std::ios::end);

    archivo.write(reinterpret_cast<char*>(&a),sizeof(a));
    archivo.write(reinterpret_cast<char*>(&b),sizeof(b));
    archivo.write(reinterpret_cast<char*>(&c),sizeof(c));
    archivo.write(reinterpret_cast<char*>(&d),sizeof(d));

    unsigned char byte = static_cast<unsigned char>(bits.to_ulong());
    archivo.write(reinterpret_cast<char*>(&byte), sizeof(byte));

    const char * e = "10101";
    const char * f = "Srinivasan";
    const char * g = "Compq. Sci";

    archivo.write(e, strlen(e));
    archivo.write(f, strlen(g));
    archivo.write(g, strlen(g));
    archivo.write("\n",1);

    return 0;
}
