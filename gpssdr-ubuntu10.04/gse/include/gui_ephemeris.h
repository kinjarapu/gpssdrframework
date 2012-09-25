#ifndef GUI_EPHEMERIS_H_
#define GUI_EPHEMERIS_H_

#include "gui.h"

/*----------------------------------------------------------------------------------------------*/
class GUI_Ephemeris: public iGUI_Ephemeris, public GUI_Object
{

  private:

    /* Add all the "subwindows" here */
    int32_t sv;
    int32_t loaded;
    int32_t tic;

  public:

    GUI_Ephemeris();
    ~GUI_Ephemeris();

    void onClose(wxCloseEvent& evt);
    void render(wxDC& dc);
    void paintNow();

    void onMouse(wxMouseEvent& event);
    void onSave(wxCommandEvent& event);
    void onLoad(wxCommandEvent& event);
    void onRefresh(wxCommandEvent& event);
    void renderDecoded();
    void renderSV();

    DECLARE_EVENT_TABLE()

};
/*----------------------------------------------------------------------------------------------*/

#endif
