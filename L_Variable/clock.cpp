#include <iostream>
#include <vector>

struct Page {
    int id;
    bool referenceBit;
};

class ClockAlgorithm {
private:
    std::vector<Page> frames;
    int pointer;

public:
    ClockAlgorithm(int numFrames) {
        frames.resize(numFrames);
        pointer = 0;
    }

    void accessPage(int pageId) {
        bool pageFound = false;

        // Buscar la página en los frames
        for (int i = 0; i < frames.size(); ++i) {
            if (frames[i].id == pageId) {
                frames[i].referenceBit = true;  // Establecer el bit de referencia en 1
                pageFound = true;
                break;
            }
        }

        // Si la página no está en los frames, reemplazar una página
        if (!pageFound) {
            while (true) {
                if (!frames[pointer].referenceBit) {
                    frames[pointer].id = pageId;
                    frames[pointer].referenceBit = true;
                    pointer++;  // Avanzar el puntero
                    if(pointer == frames.size()) pointer = 0;
                    break;
                } else {
                    frames[pointer].referenceBit = false;  // Establecer el bit de referencia en 0
                    pointer++;  // Avanzar el puntero
                    if(pointer == frames.size()) pointer = 0;  // Avanzar el puntero
                }
            }
        }
    }

    void printFrames() {
        std::cout << "Frames:" << std::endl;
        for (const auto& frame : frames) {
            std::cout << "Page ID: " << frame.id << ", Reference Bit: " << frame.referenceBit << std::endl;
        }
        std::cout<<"Pointer: "<<pointer<<std::endl;
    }
};

int main() {
    ClockAlgorithm clock(4);

    clock.accessPage(1);
    clock.accessPage(2);
    clock.accessPage(2);
    clock.accessPage(3);
    clock.accessPage(4);
    clock.accessPage(5);
    clock.accessPage(2);
    clock.accessPage(1);
    clock.accessPage(7);
    clock.accessPage(3);
    clock.accessPage(2);

    clock.printFrames();

    return 0;
}
