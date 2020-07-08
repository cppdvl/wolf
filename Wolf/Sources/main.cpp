#include <iostream>
#include <wolf/engine/engine.hpp>
#include <wolf/engine/resourcemanager.hpp>
#include <wolf/import/resources/importer.hpp>



int main (int argc, const char **argv){
    
    Wolf::Engine::Init(argc, argv);
    //Load the components of cameras.
    //Load a Scene out of a Reosurce.
    //Now. Let's build a default Scene
    //For example a Camera is a Container of 3 components: A Kinematic Header, An Orientaition / Scale / Translation Header, A Projection Header.
    //A Cube has an Orientaition / Scale / Translation Header, A Mesh Header, A Kinematic Header.
    //A Light has a Radiation Header, A Kinematic Header
     
    
    std::cout << "-- FINISHED OK --" << std::endl;
    return 0;
}