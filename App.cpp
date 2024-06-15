#include "App.h"
#include "MainFrame.h"
#include "Sound.h"
#include <vector>
#include <string>
//Converts frequency (Hz) to angular velocity
static double Hz2W(double dHertz)
{
    return dHertz * 2.0 * M_PI;
}
double func(double dTime)
{
    return sin(Hz2W(440.0) * dTime) * 0.4;
}
bool App::OnInit()
{

	MainFrame* frame = new MainFrame("Graph Plotting");
    frame->Show(true);    
	return true;
}

wxIMPLEMENT_APP(App);