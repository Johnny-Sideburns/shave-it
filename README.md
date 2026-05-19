# shave-it
virtual shaving program in cpp and open gl
.exe file in /Shave-IT/

in sceneViewerApplication.cpp constructor set m_use_compute to true or false to switch between compute and geometry shader pipeline, respectively.

controls:

    left mouse  : shave
    m           : mirror shave
    0-9         : set shave level
    +/-         : additional shaving precision
    mousewheel  : shave radius
    b           : grow beard
    h           : grow hair

space to activate/deactivate camera controls:

    wasd        : movement
    mousexy     : look