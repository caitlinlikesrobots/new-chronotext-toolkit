/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE MODIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

/*
 * STUB FOR RUNNING A CinderSketch
 * ON THE DESKTOP (OSX OR WINDOW)
 */

#include "chronotext/cinder/CinderApp.h"

#include "Sketch.h"

using namespace std;
using namespace ci;
using namespace app;
using namespace chr;

class Application : public CinderApp
{
public:
    Application();
    void prepareSettings(Settings *settings);
};

Application::Application()
{
    sketch = new Sketch(this);
}

void Application::prepareSettings(Settings *settings)
{
    settings->enableHighDensityDisplay();

#if defined(CINDER_MAC) || defined(CINDER_MSW)
//  settings->setWindowSize(320, 480); // IPHONE
    settings->setWindowSize(640, 960); // IPHONE RETINA
//  settings->setWindowSize(768, 1024); // IPAD
//  settings->setWindowSize(480, 800); // ANDROID WVGA800
#endif
}

CINDER_APP_NATIVE(Application, RendererGl(RendererGl::AA_NONE))
