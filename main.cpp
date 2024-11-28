#include "Application.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    Application* app = new Application;

    if (!app->Init(argc, argv)) {
        cerr << "App::Init() failed" << endl;
        return 1;
    }

    app->Run();
    delete app;
    return 0;
}
