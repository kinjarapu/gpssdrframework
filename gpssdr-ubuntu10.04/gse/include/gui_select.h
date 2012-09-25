#ifndef GUI_SELECT_H_
#define GUI_SELECT_H_

#include "gui.h"

/*----------------------------------------------------------------------------------------------*/
class GUI_Select: public iGUI_Select, public GUI_Object
{

  private:

    /* Add all the "subwindows" here */
    int32_t sv;
    int32_t loaded;
    int32_t tic;

  public:

    GUI_Select();
    ~GUI_Select();

    void onClose(wxCloseEvent& evt);
    void render(wxDC& dc);
    void paintNow();

    void onMouse(wxMouseEvent& event);
    void onRefresh(wxCommandEvent& event);
    void renderVisible();
    void renderMode();
    void renderSV();

    DECLARE_EVENT_TABLE()

};
/*----------------------------------------------------------------------------------------------*/

#endif
