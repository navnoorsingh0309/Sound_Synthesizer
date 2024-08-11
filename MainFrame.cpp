#include "MainFrame.h"
#include <vector>
#include "Sound.h"
#include <string>
#include <cmath>
#include <thread>
#include <chrono>
#include <mutex>
using namespace std;

bool Wave[4] = { true, false, false, false };

// Wave Properties
double octaveBaseFreqs[4] = { 110.0, 440.0, 440.0, 440.0 };
double amps[4] = { 0.0, 0.0, 0.0, 0.0 };
int tunes[4] = { 0, 0, 0, 0 };
int types[4] = { 0, -1, -1, -1 };

// Octave Implementation for better tunes
double d12thRootof2 = pow(2.0, 1.0 / 12.0);

double waveFunctionReturner(double dTime)
{
    double returning = 0;
    for (int i = 0; i <= 3; i++)
    {
        if (Wave[i] == false)
            continue;
        switch (types[i])
        {
            // Sin Wave
            case 0:
                returning += (sin(Sound<short>::Hz2W(440.0) * dTime) * amps[i]);
                break;
            // Square Wave
            case 1:
                returning += ((sin(Sound<short>::Hz2W(octaveBaseFreqs[i] * pow(d12thRootof2, tunes[i])) * dTime) > 0.0 ? 1.0 : -1.0) * amps[i]);
                break;
            //Triangluar Wave
            case 2:
                returning += ((asin(sin(Sound<short>::Hz2W(octaveBaseFreqs[i] * pow(d12thRootof2, tunes[i])) * dTime)) * 2.0 / M_PI) * amps[i]);
                break;
            // Saw Wave
            case 3:
            {
                double dOutput = 0.0;

                for (double n = 1.0; n < 40.0; n++)
                    dOutput += (sin(n * Sound<short>::Hz2W(octaveBaseFreqs[i] * pow(d12thRootof2, tunes[i])) * dTime)) / n;

                returning += dOutput * (2.0 / M_PI) * amps[i];
                break;
            }
        }
    }
    return returning;
}

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
{
    // Sound Object
    Sound<short>* soundObj = new Sound<short>();
    vector<wstring> audioDevices = soundObj->getSoundDevices();
    soundObj->initializeDevice(audioDevices[0], 1, 44100, 16, 8192);
    soundObj->setWaveFunc(waveFunctionReturner);


    // Header Panel
    wxPanel* headerPanel = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(800, 50));
    // Audio Devices Static Text
    wxStaticText* audioDevicesLabel = new wxStaticText(headerPanel, wxID_ANY, "Audio Devices:", wxPoint(10, 10));
    audioDevicesLabel->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    // Audio Devices Combo Box
    wxArrayString audioDevicesArray;
    for (auto d : soundObj->getSoundDevices())
        audioDevicesArray.Add(wxString(d));
    wxComboBox* audioDevicesCombo = new wxComboBox(headerPanel, wxID_ANY, wxEmptyString, wxPoint(160, 10),
        wxSize(200, -1), audioDevicesArray, wxCB_DROPDOWN | wxCB_READONLY);
    audioDevicesCombo->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_NORMAL, false));
    audioDevicesCombo->SetSelection(0);

    // Title of Software
    wxStaticText* titleText = new wxStaticText(headerPanel, wxID_ANY, "Audio Synthesizer", wxPoint(600, 10),
        wxDefaultSize);
    titleText->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));

    //--------------------------------------------------- Panels --------------------------------------------------
    // Panel for Waveforms
    wxPanel* waveFormsPanel = new wxPanel(this, wxID_ANY, wxPoint(0, 50), wxSize(800, 350), wxSIMPLE_BORDER);
    // Panel for different Wave
    wxPanel* firstWavePanel = new wxPanel(waveFormsPanel, wxID_ANY, wxPoint(0, 0), wxSize(400, 175), wxSIMPLE_BORDER);
    firstWavePanel->SetBackgroundColour(wxColour(68, 68, 68));
    wxPanel* secondWavePanel = new wxPanel(waveFormsPanel, wxID_ANY, wxPoint(400, 0), wxSize(400, 175), wxSIMPLE_BORDER);
    secondWavePanel->SetBackgroundColour(wxColour(68, 68, 68));
    wxPanel* thirdWavePanel = new wxPanel(waveFormsPanel, wxID_ANY, wxPoint(0, 175), wxSize(400, 175), wxSIMPLE_BORDER);
    thirdWavePanel->SetBackgroundColour(wxColour(68, 68, 68));
    wxPanel* forthWavePanel = new wxPanel(waveFormsPanel, wxID_ANY, wxPoint(400, 175), wxSize(400, 175), wxSIMPLE_BORDER);
    forthWavePanel->SetBackgroundColour(wxColour(68, 68, 68));
    //-------------------------------------------------------------------------------------------------------------

    // First Wave Panel
    // Disable Button
    wxButton* ENWaveformBtn1 = new wxButton(firstWavePanel, wxID_ANY, "Disable", wxPoint(10, 10), wxSize(80, 30));
    ENWaveformBtn1->Bind(wxEVT_BUTTON, [ENWaveformBtn1](wxCommandEvent& evt) {
        if (Wave[0] == true)
        {
            ENWaveformBtn1->SetLabel("Enable");
            Wave[0] = false;
        }
        else
        {
            ENWaveformBtn1->SetLabel("Disable");
            Wave[0] = true;
        }
    });
    ENWaveformBtn1->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));

    // Play Button
    wxButton* PlayWaveformBtn1 = new wxButton(firstWavePanel, wxID_ANY, "Play", wxPoint(100, 10), wxSize(80, 30));
    PlayWaveformBtn1->Bind(wxEVT_BUTTON, [soundObj, PlayWaveformBtn1](wxCommandEvent& evt) {
        soundObj->setWaveFunc(waveFunctionReturner);
        soundObj->Play();
        PlayWaveformBtn1->SetLabel("Stop");
    });
    PlayWaveformBtn1->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));

    // Waveform Type label
    wxStaticText* waveTypeLabel1 = new wxStaticText(firstWavePanel, wxID_ANY, "Waveform:", wxPoint(10, 50));
    waveTypeLabel1->SetBackgroundColour(wxColour(68, 68, 68));
    waveTypeLabel1->SetForegroundColour(*wxWHITE);
    waveTypeLabel1->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    // Waveform Type Combobox
    wxString waveformComboItems[] = {
        wxT("Sine Wave"),
        wxT("Square Wave"),
        wxT("Triangular Wave"),
        wxT("Saw Wave")
    };
    wxComboBox* waveTypeCombo1 = new wxComboBox(firstWavePanel, wxID_ANY, wxT("Sine Wave"), wxPoint(120, 50),
        wxSize(150, -1), 4, waveformComboItems, wxCB_DROPDOWN | wxCB_READONLY);
    waveTypeCombo1->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_NORMAL, false));
    waveTypeCombo1->Bind(wxEVT_COMBOBOX, [waveTypeCombo1, soundObj](wxCommandEvent& cmd) {
        types[0] = waveTypeCombo1->GetSelection();
        soundObj->setWaveFunc(waveFunctionReturner);
     });

    // Waveform Volume label
    wxStaticText* waveVolLabel1 = new wxStaticText(firstWavePanel, wxID_ANY, "Volume:", wxPoint(10, 83));
    waveVolLabel1->SetBackgroundColour(wxColour(68, 68, 68));
    waveVolLabel1->SetForegroundColour(*wxWHITE);
    waveVolLabel1->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    waveVolLabel1->SetPosition(wxPoint(10 + (waveTypeLabel1->GetSize().x - waveVolLabel1->GetSize().x), 83));
    // Volume Slider value for waveform 1
    wxStaticText* waveVolValLabel1 = new wxStaticText(firstWavePanel, wxID_ANY, "0.0", wxPoint(250, 83));
    waveVolValLabel1->SetBackgroundColour(wxColour(68, 68, 68));
    waveVolValLabel1->SetForegroundColour(*wxWHITE);
    waveVolValLabel1->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    // Volume Slider for waveform 1
    wxSlider* volSlider1 = new wxSlider(firstWavePanel, wxID_ANY, 0, 0, 10, wxPoint(120, 80), wxSize(130, -1));
    volSlider1->Bind(wxEVT_SCROLL_THUMBTRACK, [waveVolValLabel1, volSlider1, soundObj](wxCommandEvent& evt) {
        if (volSlider1->GetValue() < 10)
            waveVolValLabel1->SetLabelText("0." + wxString::Format("%i", volSlider1->GetValue()));
        else
            waveVolValLabel1->SetLabelText("1." + wxString::Format("%i", volSlider1->GetValue()).substr(1, wxString::Format("%i", volSlider1->GetValue()).length()));
        waveVolValLabel1->GetLabel().ToDouble(&amps[0]);
        soundObj->setWaveFunc(waveFunctionReturner);
    });
    volSlider1->Bind(wxEVT_SCROLL_CHANGED, [waveVolValLabel1, volSlider1, soundObj](wxCommandEvent& evt) {
        if (volSlider1->GetValue() < 10)
            waveVolValLabel1->SetLabelText("0." + wxString::Format("%i", volSlider1->GetValue()));
        else
            waveVolValLabel1->SetLabelText("1." + wxString::Format("%i", volSlider1->GetValue()).substr(1, wxString::Format("%i", volSlider1->GetValue()).length()));
        waveVolValLabel1->GetLabel().ToDouble(&amps[0]);
        soundObj->setWaveFunc(waveFunctionReturner);
    });

    // Waveform Tune label
    wxStaticText* waveTuneLabel1 = new wxStaticText(firstWavePanel, wxID_ANY, "Tune:", wxPoint(10, 110));
    waveTuneLabel1->SetBackgroundColour(wxColour(68, 68, 68));
    waveTuneLabel1->SetForegroundColour(*wxWHITE);
    waveTuneLabel1->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    waveTuneLabel1->SetPosition(wxPoint(10 + (waveTypeLabel1->GetSize().x - waveTuneLabel1->GetSize().x), 110));
    // Tune Slider value for waveform 1
    wxStaticText* waveTuneValLabel1 = new wxStaticText(firstWavePanel, wxID_ANY, "0", wxPoint(250, 113));
    waveTuneValLabel1->SetBackgroundColour(wxColour(68, 68, 68));
    waveTuneValLabel1->SetForegroundColour(*wxWHITE);
    waveTuneValLabel1->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    // Tune Slider for waveform 1
    wxSlider* tuneSlider1 = new wxSlider(firstWavePanel, wxID_ANY, 0, 0, 12, wxPoint(120, 110), wxSize(130, -1));
    tuneSlider1->Bind(wxEVT_SCROLL_THUMBTRACK, [waveTuneValLabel1, tuneSlider1](wxCommandEvent& evt) {
        waveTuneValLabel1->SetLabelText(wxString::Format("%i", tuneSlider1->GetValue()));
        tunes[0] = tuneSlider1->GetValue();
    });
    tuneSlider1->Bind(wxEVT_SCROLL_CHANGED, [waveTuneValLabel1, tuneSlider1](wxCommandEvent& evt) {
        if (tuneSlider1->GetValue() < 10)
            waveTuneValLabel1->SetLabelText("0." + wxString::Format("%i", tuneSlider1->GetValue()));
        else
            waveTuneValLabel1->SetLabelText("1." + wxString::Format("%i", tuneSlider1->GetValue()).substr(1, wxString::Format("%i", tuneSlider1->GetValue()).length()));
    });

   

    // Second Wave Panel
    // Disable Button
    wxButton* ENWaveformBtn2 = new wxButton(secondWavePanel, wxID_ANY, "Enable", wxPoint(10, 10), wxSize(80, 30));
    ENWaveformBtn2->Bind(wxEVT_BUTTON, [ENWaveformBtn2](wxCommandEvent& evt) {
        if (Wave[1] == true)
        {
            ENWaveformBtn2->SetLabel("Enable");
            Wave[1] = false;
        }
        else
        {
            ENWaveformBtn2->SetLabel("Disable");
            Wave[1] = true;
        }
    });
    ENWaveformBtn2->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));

    // Play Button
    wxButton* PlayWaveformBtn2 = new wxButton(secondWavePanel, wxID_ANY, "Play", wxPoint(100, 10), wxSize(80, 30));
    PlayWaveformBtn2->Bind(wxEVT_BUTTON, [soundObj, PlayWaveformBtn2](wxCommandEvent& evt) {
        soundObj->Play();
        PlayWaveformBtn2->SetLabel("Stop");
    });
    PlayWaveformBtn2->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));

    // Waveform Type label
    wxStaticText* waveTypeLabel2 = new wxStaticText(secondWavePanel, wxID_ANY, "Waveform:", wxPoint(10, 50));
    waveTypeLabel2->SetBackgroundColour(wxColour(68, 68, 68));
    waveTypeLabel2->SetForegroundColour(*wxWHITE);
    waveTypeLabel2->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    wxComboBox* waveTypeCombo2 = new wxComboBox(secondWavePanel, wxID_ANY, wxT("Sine Wave"), wxPoint(120, 50),
        wxSize(150, -1), 4, waveformComboItems, wxCB_DROPDOWN | wxCB_READONLY);
    waveTypeCombo2->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_NORMAL, false));

    // Waveform Volume label
    wxStaticText* waveVolLabel2 = new wxStaticText(secondWavePanel, wxID_ANY, "Volume:", wxPoint(10, 83));
    waveVolLabel2->SetBackgroundColour(wxColour(68, 68, 68));
    waveVolLabel2->SetForegroundColour(*wxWHITE);
    waveVolLabel2->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    waveVolLabel2->SetPosition(wxPoint(10 + (waveTypeLabel2->GetSize().x - waveVolLabel2->GetSize().x), 83));
    // Volume Slider value for waveform 2
    wxStaticText* waveVolValLabel2 = new wxStaticText(secondWavePanel, wxID_ANY, "0.0", wxPoint(250, 83));
    waveVolValLabel2->SetBackgroundColour(wxColour(68, 68, 68));
    waveVolValLabel2->SetForegroundColour(*wxWHITE);
    waveVolValLabel2->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    // Volume Slider for waveform 1
    wxSlider* volSlider2 = new wxSlider(secondWavePanel, wxID_ANY, 0, 0, 10, wxPoint(120, 80), wxSize(130, -1));
    volSlider2->Bind(wxEVT_SCROLL_THUMBTRACK, [waveVolValLabel2, volSlider2](wxCommandEvent& evt) {
        if (volSlider2->GetValue() < 10)
            waveVolValLabel2->SetLabelText("0." + wxString::Format("%i", volSlider2->GetValue()));
        else
            waveVolValLabel2->SetLabelText("1." + wxString::Format("%i", volSlider2->GetValue()).substr(1, wxString::Format("%i", volSlider2->GetValue()).length()));
        waveVolValLabel2->GetLabel().ToDouble(&amps[1]);
    });
    volSlider2->Bind(wxEVT_SCROLL_CHANGED, [waveVolValLabel2, volSlider2](wxCommandEvent& evt) {
        if (volSlider2->GetValue() < 10)
            waveVolValLabel2->SetLabelText("0." + wxString::Format("%i", volSlider2->GetValue()));
        else
            waveVolValLabel2->SetLabelText("1." + wxString::Format("%i", volSlider2->GetValue()).substr(1, wxString::Format("%i", volSlider2->GetValue()).length()));
        waveVolValLabel2->GetLabel().ToDouble(&amps[1]);
    });

    // Waveform Tune label
    wxStaticText* waveTuneLabel2 = new wxStaticText(secondWavePanel, wxID_ANY, "Tune:", wxPoint(10, 110));
    waveTuneLabel2->SetBackgroundColour(wxColour(68, 68, 68));
    waveTuneLabel2->SetForegroundColour(*wxWHITE);
    waveTuneLabel2->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    waveTuneLabel2->SetPosition(wxPoint(10 + (waveTypeLabel2->GetSize().x - waveTuneLabel2->GetSize().x), 110));
    // Tune Slider value for waveform 2
    wxStaticText* waveTuneValLabel2 = new wxStaticText(secondWavePanel, wxID_ANY, "0.0", wxPoint(250, 113));
    waveTuneValLabel2->SetBackgroundColour(wxColour(68, 68, 68));
    waveTuneValLabel2->SetForegroundColour(*wxWHITE);
    waveTuneValLabel2->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
   // Tune Slider for waveform 2
    wxSlider* tuneSlider2 = new wxSlider(secondWavePanel, wxID_ANY, 0, 0, 12, wxPoint(120, 110), wxSize(130, -1));
    tuneSlider2->Bind(wxEVT_SCROLL_THUMBTRACK, [waveTuneValLabel2, tuneSlider2](wxCommandEvent& evt) {
        if (tuneSlider2->GetValue() < 10)
            waveTuneValLabel2->SetLabelText("0." + wxString::Format("%i", tuneSlider2->GetValue()));
        else
            waveTuneValLabel2->SetLabelText("1." + wxString::Format("%i", tuneSlider2->GetValue()).substr(1, wxString::Format("%i", tuneSlider2->GetValue()).length()));
    });
    tuneSlider2->Bind(wxEVT_SCROLL_CHANGED, [waveTuneValLabel2, tuneSlider2](wxCommandEvent& evt) {
        if (tuneSlider2->GetValue() < 10)
            waveTuneValLabel2->SetLabelText("0." + wxString::Format("%i", tuneSlider2->GetValue()));
        else
            waveTuneValLabel2->SetLabelText("1." + wxString::Format("%i", tuneSlider2->GetValue()).substr(1, wxString::Format("%i", tuneSlider2->GetValue()).length()));
    });



    // Third Wave Panel
    // Disable Button
    wxButton* ENWaveformBtn3 = new wxButton(thirdWavePanel, wxID_ANY, "Enable", wxPoint(10, 10), wxSize(80, 30));
    ENWaveformBtn3->Bind(wxEVT_BUTTON, [ENWaveformBtn3](wxCommandEvent& evt) {
        if (Wave[2] == true)
        {
            ENWaveformBtn3->SetLabel("Enable");
            Wave[2] = false;
        }
        else
        {
            ENWaveformBtn3->SetLabel("Disable");
            Wave[2] = true;
        }
    });
    ENWaveformBtn3->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));

    // Play Button
    wxButton* PlayWaveformBtn3 = new wxButton(thirdWavePanel, wxID_ANY, "Play", wxPoint(100, 10), wxSize(80, 30));
    PlayWaveformBtn3->Bind(wxEVT_BUTTON, [soundObj, PlayWaveformBtn3](wxCommandEvent& evt) {
        soundObj->Play();
        PlayWaveformBtn3->SetLabel("Stop");
    });
    PlayWaveformBtn3->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));

    // Waveform Type label
    wxStaticText* waveTypeLabel3 = new wxStaticText(thirdWavePanel, wxID_ANY, "Waveform:", wxPoint(10, 50));
    waveTypeLabel3->SetBackgroundColour(wxColour(68, 68, 68));
    waveTypeLabel3->SetForegroundColour(*wxWHITE);
    waveTypeLabel3->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    wxComboBox* waveTypeCombo3 = new wxComboBox(thirdWavePanel, wxID_ANY, wxT("Sine Wave"), wxPoint(120, 50),
        wxSize(150, -1), 4, waveformComboItems, wxCB_DROPDOWN | wxCB_READONLY);
    waveTypeCombo3->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_NORMAL, false));

    // Waveform Volume label
    wxStaticText* waveVolLabel3 = new wxStaticText(thirdWavePanel, wxID_ANY, "Volume:", wxPoint(10, 83));
    waveVolLabel3->SetBackgroundColour(wxColour(68, 68, 68));
    waveVolLabel3->SetForegroundColour(*wxWHITE);
    waveVolLabel3->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    waveVolLabel3->SetPosition(wxPoint(10 + (waveTypeLabel3->GetSize().x - waveVolLabel3->GetSize().x), 83));
    // Volume Slider value for waveform 3
    wxStaticText* waveVolValLabel3 = new wxStaticText(thirdWavePanel, wxID_ANY, "0.0", wxPoint(250, 83));
    waveVolValLabel3->SetBackgroundColour(wxColour(68, 68, 68));
    waveVolValLabel3->SetForegroundColour(*wxWHITE);
    waveVolValLabel3->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    // Volume Slider for waveform 1
    wxSlider* volSlider3 = new wxSlider(thirdWavePanel, wxID_ANY, 0, 0, 10, wxPoint(120, 80), wxSize(130, -1));
    volSlider3->Bind(wxEVT_SCROLL_THUMBTRACK, [waveVolValLabel3, volSlider3](wxCommandEvent& evt) {
        if (volSlider3->GetValue() < 10)
            waveVolValLabel3->SetLabelText("0." + wxString::Format("%i", volSlider3->GetValue()));
        else
            waveVolValLabel3->SetLabelText("1." + wxString::Format("%i", volSlider3->GetValue()).substr(1, wxString::Format("%i", volSlider3->GetValue()).length()));
        waveVolValLabel3->GetLabel().ToDouble(&amps[2]);
    });
    volSlider3->Bind(wxEVT_SCROLL_CHANGED, [waveVolValLabel3, volSlider3](wxCommandEvent& evt) {
        if (volSlider3->GetValue() < 10)
            waveVolValLabel3->SetLabelText("0." + wxString::Format("%i", volSlider3->GetValue()));
        else
            waveVolValLabel3->SetLabelText("1." + wxString::Format("%i", volSlider3->GetValue()).substr(1, wxString::Format("%i", volSlider3->GetValue()).length()));
        waveVolValLabel3->GetLabel().ToDouble(&amps[2]);
    });

    // Waveform Tune label
    wxStaticText* waveTuneLabel3 = new wxStaticText(thirdWavePanel, wxID_ANY, "Tune:", wxPoint(10, 110));
    waveTuneLabel3->SetBackgroundColour(wxColour(68, 68, 68));
    waveTuneLabel3->SetForegroundColour(*wxWHITE);
    waveTuneLabel3->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    waveTuneLabel3->SetPosition(wxPoint(10 + (waveTypeLabel3->GetSize().x - waveTuneLabel3->GetSize().x), 110));
    // Tune Slider value for waveform 2
    wxStaticText* waveTuneValLabel3 = new wxStaticText(thirdWavePanel, wxID_ANY, "0.0", wxPoint(250, 113));
    waveTuneValLabel3->SetBackgroundColour(wxColour(68, 68, 68));
    waveTuneValLabel3->SetForegroundColour(*wxWHITE);
    waveTuneValLabel3->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    // Tune Slider for waveform 2
    wxSlider* tuneSlider3 = new wxSlider(thirdWavePanel, wxID_ANY, 0, 0, 12, wxPoint(120, 110), wxSize(130, -1));
    tuneSlider3->Bind(wxEVT_SCROLL_THUMBTRACK, [waveTuneValLabel3, tuneSlider3](wxCommandEvent& evt) {
        if (tuneSlider3->GetValue() < 10)
            waveTuneValLabel3->SetLabelText("0." + wxString::Format("%i", tuneSlider3->GetValue()));
        else
            waveTuneValLabel3->SetLabelText("1." + wxString::Format("%i", tuneSlider3->GetValue()).substr(1, wxString::Format("%i", tuneSlider3->GetValue()).length()));
    });
    tuneSlider3->Bind(wxEVT_SCROLL_CHANGED, [waveTuneValLabel3, tuneSlider3](wxCommandEvent& evt) {
        if (tuneSlider3->GetValue() < 10)
            waveTuneValLabel3->SetLabelText("0." + wxString::Format("%i", tuneSlider3->GetValue()));
        else
            waveTuneValLabel3->SetLabelText("1." + wxString::Format("%i", tuneSlider3->GetValue()).substr(1, wxString::Format("%i", tuneSlider3->GetValue()).length()));
    });


    // Fourth Wave Panel
    // Disable Button
    wxButton* ENWaveformBtn4 = new wxButton(forthWavePanel, wxID_ANY, "Enable", wxPoint(10, 10), wxSize(80, 30));
    ENWaveformBtn4->Bind(wxEVT_BUTTON, [ENWaveformBtn4](wxCommandEvent& evt) {
        if (Wave[3] == true)
        {
            ENWaveformBtn4->SetLabel("Enable");
            Wave[3] = false;
        }
        else
        {
            ENWaveformBtn4->SetLabel("Disable");
            Wave[3] = true;
        }
    });
    ENWaveformBtn4->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));

    // Play Button
    wxButton* PlayWaveformBtn4 = new wxButton(forthWavePanel, wxID_ANY, "Play", wxPoint(100, 10), wxSize(80, 30));
    PlayWaveformBtn4->Bind(wxEVT_BUTTON, [soundObj, PlayWaveformBtn4](wxCommandEvent& evt) {
        soundObj->Play();
        PlayWaveformBtn4->SetLabel("Stop");
    });
    PlayWaveformBtn4->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));

    // Waveform Type label
    wxStaticText* waveTypeLabel4 = new wxStaticText(forthWavePanel, wxID_ANY, "Waveform:", wxPoint(10, 50));
    waveTypeLabel4->SetBackgroundColour(wxColour(68, 68, 68));
    waveTypeLabel4->SetForegroundColour(*wxWHITE);
    waveTypeLabel4->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    wxComboBox* waveTypeCombo4 = new wxComboBox(forthWavePanel, wxID_ANY, wxT("Sine Wave"), wxPoint(120, 50),
        wxSize(150, -1), 4, waveformComboItems, wxCB_DROPDOWN | wxCB_READONLY);
    waveTypeCombo4->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_NORMAL, false));

    // Waveform Volume label
    wxStaticText* waveVolLabel4 = new wxStaticText(forthWavePanel, wxID_ANY, "Volume:", wxPoint(10, 83));
    waveVolLabel4->SetBackgroundColour(wxColour(68, 68, 68));
    waveVolLabel4->SetForegroundColour(*wxWHITE);
    waveVolLabel4->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    waveVolLabel4->SetPosition(wxPoint(10 + (waveTypeLabel4->GetSize().x - waveVolLabel4->GetSize().x), 83));
    // Volume Slider value for waveform 4
    wxStaticText* waveVolValLabel4 = new wxStaticText(forthWavePanel, wxID_ANY, "0.0", wxPoint(250, 83));
    waveVolValLabel4->SetBackgroundColour(wxColour(68, 68, 68));
    waveVolValLabel4->SetForegroundColour(*wxWHITE);
    waveVolValLabel4->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    // Volume Slider for waveform 1
    wxSlider* volSlider4 = new wxSlider(forthWavePanel, wxID_ANY, 0, 0, 10, wxPoint(120, 80), wxSize(130, -1));
    volSlider4->Bind(wxEVT_SCROLL_THUMBTRACK, [waveVolValLabel4, volSlider4](wxCommandEvent& evt) {
        if (volSlider4->GetValue() < 10)
            waveVolValLabel4->SetLabelText("0." + wxString::Format("%i", volSlider4->GetValue()));
        else
            waveVolValLabel4->SetLabelText("1." + wxString::Format("%i", volSlider4->GetValue()).substr(1, wxString::Format("%i", volSlider4->GetValue()).length()));
        waveVolValLabel4->GetLabel().ToDouble(&amps[3]);
    });
    volSlider4->Bind(wxEVT_SCROLL_CHANGED, [waveVolValLabel4, volSlider4](wxCommandEvent& evt) {
        if (volSlider4->GetValue() < 10)
            waveVolValLabel4->SetLabelText("0." + wxString::Format("%i", volSlider4->GetValue()));
        else
            waveVolValLabel4->SetLabelText("1." + wxString::Format("%i", volSlider4->GetValue()).substr(1, wxString::Format("%i", volSlider4->GetValue()).length()));
        waveVolValLabel4->GetLabel().ToDouble(&amps[3]);
    });
    // Waveform Tune label
    wxStaticText* waveTuneLabel4 = new wxStaticText(forthWavePanel, wxID_ANY, "Tune:", wxPoint(10, 110));
    waveTuneLabel4->SetBackgroundColour(wxColour(68, 68, 68));
    waveTuneLabel4->SetForegroundColour(*wxWHITE);
    waveTuneLabel4->SetFont(wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    waveTuneLabel4->SetPosition(wxPoint(10 + (waveTypeLabel4->GetSize().x - waveTuneLabel4->GetSize().x), 110));
    // Tune Slider value for waveform 4
    wxStaticText* waveTuneValLabel4 = new wxStaticText(forthWavePanel, wxID_ANY, "0.0", wxPoint(250, 113));
    waveTuneValLabel4->SetBackgroundColour(wxColour(68, 68, 68));
    waveTuneValLabel4->SetForegroundColour(*wxWHITE);
    waveTuneValLabel4->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD, false));
    // Tune Slider for waveform 4
    wxSlider* tuneSlider4 = new wxSlider(forthWavePanel, wxID_ANY, 0, 0, 12, wxPoint(120, 110), wxSize(130, -1));
    tuneSlider4->Bind(wxEVT_SCROLL_THUMBTRACK, [waveTuneValLabel4, tuneSlider4](wxCommandEvent& evt) {
        if (tuneSlider4->GetValue() < 10)
            waveTuneValLabel4->SetLabelText("0." + wxString::Format("%i", tuneSlider4->GetValue()));
        else
            waveTuneValLabel4->SetLabelText("1." + wxString::Format("%i", tuneSlider4->GetValue()).substr(1, wxString::Format("%i", tuneSlider4->GetValue()).length()));
    });
    tuneSlider4->Bind(wxEVT_SCROLL_CHANGED, [waveTuneValLabel4, tuneSlider4](wxCommandEvent& evt) {
        if (tuneSlider4->GetValue() < 10)
            waveTuneValLabel4->SetLabelText("0." + wxString::Format("%i", tuneSlider4->GetValue()));
        else
            waveTuneValLabel4->SetLabelText("1." + wxString::Format("%i", tuneSlider4->GetValue()).substr(1, wxString::Format("%i", tuneSlider4->GetValue()).length()));
    });
}