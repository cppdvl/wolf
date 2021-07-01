//
// Created by User on 6/26/2021.
//

#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>


//WolfEngine
#include <wolf/renderer/shader.hpp>
#include <signalslot.h>

#include "../singleton.h"
#include "../dgetypes.h"
#include "../colors.h"
#include "../rendermanager.h"
#include "../windowsmanager.h"
#include "../mesh.h"
#include "../platform/platform.h"
#include "../inputmanager.h"
#include "../application.h"

class TriangleApplication : public DGE::Application{

protected:
    virtual void Init() override;
    virtual void Load() override;
    virtual void MainLoop() override;
    virtual void ShutDown() override;
};


