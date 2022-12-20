#include <string.h>

#include <fdeep/fdeep.hpp>

#include "include/canvas.h"
#include "include/preprocess.h"

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        std::cout << "Invalid number of arguments" << std::endl;
        return 1;
    }

    for(int i = 1; i<argc; i++)
    {
        if (strcmp(argv[i], "--process") == 0) 
        {

            std::cout << "Preprocess Data" << std::endl;
            createPreprocessedData();
            return 0;
        }
        if (strcmp(argv[i], "--canvas") == 0) 
        {
            std::cout << "Load model" << std::endl;
            const fdeep::model model = fdeep::load_model("./model/doodles.json");
            
            std::cout << "Starting Canvas" << std::endl;
            startCanvas(model);
            return 0;
        }
        else 
        {
            std::cout << "Invalid Command" << std::endl;
        }
    }
    
    return 0;
}
