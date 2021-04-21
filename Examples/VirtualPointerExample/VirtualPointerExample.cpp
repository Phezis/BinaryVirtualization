#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <VirtualPointer.h>

int main() {
    try
    {
        using std::size_t;
        const size_t src1Size = 6;
        const size_t src2Size = 2;
        size_t src1[src1Size] = {};
        size_t src2[src2Size] = {};
        auto vptr = VirtualPointer<size_t>();
        vptr.addChunk(src1, 4);
        vptr.addChunk(src2, 2);

        /*
            scr1 - 0 1 2 3 4 5 | 0 1 - src2
                   0 0 0 0 0 0 | 0 0
        */
        memset(vptr, 9, 6);
        /*
            scr1 - 0 1 2 3 4 5 | 0 1 - src2
                   9 9 9 9 0 0 | 9 9
        */
        *vptr = 1; // запишем в самое начало 1
        /*
            scr1 - 0 1 2 3 4 5 | 0 1 - src2
                   1 9 9 9 0 0 | 9 9
        */
        ++vptr; // сместимся вперед
        *vptr = 1;
        /*
            scr1 - 0 1 2 3 4 5 | 0 1 - src2
                   1 1 9 9 0 0 | 9 9
        */
        vptr += 3;
        *vptr = 5;
        /*
            scr1 - 0 1 2 3 4 5 | 0 1 - src2
                   1 1 9 9 0 0 | 5 9
        */

        vptr -= 4; // вернемся в начало для вывода

        std::cout << "Virtual:" << "\n\t";
        for (int i = 0; i < 6; ++i) {
            std::cout << *vptr << "; ";
            ++vptr;
        }

        std::cout << std::endl << "src1:" << "\n\t";
        for (size_t i = 0; i < src1Size; ++i) {
            std::cout << src1[i] << "; ";
            ++vptr;
        }

        std::cout << std::endl << "src2:" << "\n\t";
        for (size_t i = 0; i < src2Size; ++i) {
            std::cout << src2[i] << "; ";
            ++vptr;
        }
    }
	catch (const std::exception& e)
	{
        std::cerr << "Unexpected exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}