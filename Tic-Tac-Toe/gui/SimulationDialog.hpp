/*
 *
 * Murat Yirci - Copyright 2020
 *
 * Contact: myirci@gmail.com
 *
 *
*/

#pragma once

#include <wx/dialog.h>

class wxCheckBox;
class wxTextCtrl;

enum
{
    wxID_CHECK_BOX = 10000,
    wxID_TEXT_CTRL
};

class SimulationDialog : public wxDialog
{
public:
    SimulationDialog(wxWindow* parent, const wxString& title);
    long GetNumSimulations();
    bool SimulateInGui();

private:
    wxCheckBox* m_cb;
    wxTextCtrl* m_tc;

    bool m_gui_sim;
    long m_num_sim;

    void OnCheckBox(wxCommandEvent& event);
    void OnTextCtrl(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};
