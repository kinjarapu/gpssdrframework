/*
 * gui_Channel.cpp
 *
 *  Created on: Nov 4, 2008
 *      Author: gheckler
 */

#include "gui.h"

extern wxColor red;
extern wxColor green;
extern wxColor blue;
extern wxColor yellow;
extern wxColor white;
extern wxColor black;
extern wxColor grey;
extern wxColor htext;

DECLARE_APP(GUI_App)

/*----------------------------------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(GUI_Channel, wxFrame)
    EVT_CLOSE(GUI_Channel::onClose)
END_EVENT_TABLE()
/*----------------------------------------------------------------------------------------------*/

GUI_Channel::GUI_Channel():iGUI_Channel(NULL, wxID_ANY, wxT("Channel"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL)
{

  int32_t strwidth;
  wxString str;

  str = wxT("Ch#  SV   CL       Faccel          Doppler     CN0   BE       Locks        Power   Active\n");
  strwidth = str.Length()+2;
  tChannel->AppendText(str);
  SetSize(tChannel->GetCharWidth()*strwidth, tChannel->GetCharHeight()*(MAX_CHANNELS+3));


}

GUI_Channel::~GUI_Channel()
{


}

void GUI_Channel::onClose(wxCloseEvent& evt)
{
  wxCommandEvent cevt;
  evt.Veto();
  pToplevel->onChannel(cevt);
}

void GUI_Channel::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

void GUI_Channel::render(wxDC& dc)
{

  SPS_M *pNav = &p->sps;
  Channel_M *pchan;
  wxTextAttr style;
    wxString str, str2, bstr;
  int32_t lcv, lcv2;
  int32_t start, stop, lines;
  int32_t strwidth;
  float cn0;
  float w;
  float z;

  tChannel->Clear();
  style = tChannel->GetDefaultStyle();
  style.SetBackgroundColour(white);
  tChannel->SetDefaultStyle(style);

  str = wxT("Ch#  SV   CL       Faccel          Doppler     CN0   BE       Locks        Power   Active\n");
  strwidth = str.Length()+1;
  tChannel->AppendText(str);

  str.Printf(wxT("\n"));
  tChannel->AppendText(str);

  for(lcv = 0; lcv < MAX_CHANNELS; lcv++)
  {
    pchan = &p->channel[lcv];

    if((lcv+1) & 0x1)
      style.SetBackgroundColour(htext);
    else
      style.SetBackgroundColour(white);

    tChannel->SetDefaultStyle(style);

    if(pchan->count > 2000)
    {

      str2.Clear();

      /*Flag buffer*/
      str2 += ' ';
      ((int32_t)pchan->bit_lock) ?      str2 += 'B' : str2 += '-';
      ((int32_t)pchan->frame_lock) ?    str2 += 'F' : str2 += '-';
      (pNav->nsvs >> lcv) & 0x1 ?     str2 += 'N' : str2 += '-';

      for(lcv2 = 1; lcv2 < 6; lcv2++)
      {
        if((int32_t)pchan->subframe == lcv2)
          str2 += (char)((int32_t)pchan->subframe + 48);
        else
          str2 += '-';
      }

      cn0 = icn0_2_fcn0(pchan->cn0);
      w = pchan->w/4096.0;
      //z = (pchan->carrier_nco - (int32_t)NCO_CARR_INCR_NOM)/2097.152;
      z = (pchan->carrier_nco)/2097.152;

      str.Printf(wxT("%2d   %2d   %2d   %10.3f   %14.3f   %5.2f   %2d   %s   %10d   %6d"),
        lcv,
        (int32_t)pchan->sv+1,
        (int32_t)pchan->len,
        w,
        z,
        cn0,
        (int32_t)pchan->best_epoch,
        str2.c_str(),
        pchan->p_avg,
        (int32_t)pchan->count/1000);

      if(str.Length() < strwidth)
        tChannel->AppendText(str);
      else
      {
        bstr.Printf(wxT("%2d                                                                                       "),lcv);
        tChannel->AppendText(bstr);
      }
    }
    else
    {
      bstr.Printf(wxT("%2d                                                                                       "),lcv);
      tChannel->AppendText(bstr);
    }

    if(lcv < (MAX_CHANNELS-1))
      tChannel->AppendText(wxT("\n"));
  }

}
