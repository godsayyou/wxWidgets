/////////////////////////////////////////////////////////////////////////////
// Name:        slider.cpp
// Purpose:     wxSlider
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "slider.h"
#endif

#include "wx/slider.h"
#include "wx/mac/uma.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxSlider, wxControl)

BEGIN_EVENT_TABLE(wxSlider, wxControl)
END_EVENT_TABLE()
#endif

 // The dimensions of the different styles of sliders (From Aqua document)
#define wxSLIDER_DIMENSIONACROSS 15
#define wxSLIDER_DIMENSIONACROSS_WITHTICKMARKS 24
#define wxSLIDER_DIMENSIONACROSS_ARROW 18
 
// Distance between slider and text
#define wxSLIDER_BORDERTEXT 5
 
/* NB!  The default orientation for a slider is horizontal however if the user specifies
 * some slider styles but dosen't specify the orientation we have to assume he wants a
 * horizontal one.  Therefore in this file when testing for the sliders orientation
 * vertical is tested for if this is not set then we use the horizontal one
 * eg.  if(GetWindowStyle() & wxSL_VERTICAL) {}  else { horizontal case }>  
 */

 // Slider
 wxSlider::wxSlider()
{
    m_pageSize = 1;
    m_lineSize = 1;
    m_rangeMax = 0;
    m_rangeMin = 0;
    m_tickFreq = 0;
}

extern ControlActionUPP wxMacLiveScrollbarActionUPP ;

bool wxSlider::Create(wxWindow *parent, wxWindowID id,
                      int value, int minValue, int maxValue,
                      const wxPoint& pos,
                      const wxSize& size, long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    m_macIsUserPane = FALSE ;
    
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    SInt16 procID;
    
    m_macMinimumStatic = NULL ;
    m_macMaximumStatic = NULL ;
    m_macValueStatic = NULL ;
    
    
    m_lineSize = 1;
    m_tickFreq = 0;
    
    m_rangeMax = maxValue;
    m_rangeMin = minValue;
    
    m_pageSize = (int)((maxValue-minValue)/10);
    
    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;    
    
    procID = kControlSliderProc + kControlSliderLiveFeedback;
    if(style & wxSL_AUTOTICKS) {
        procID += kControlSliderHasTickMarks;
    }
    
    
    m_macControl = (WXWidget) ::NewControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds, "\p", true,
        value, minValue, maxValue, procID, (long) this);
    
    wxASSERT_MSG( (ControlRef) m_macControl != NULL , wxT("No valid mac control") ) ;
    
    ::SetControlAction( (ControlRef) m_macControl , wxMacLiveScrollbarActionUPP ) ;
        
    if(style & wxSL_VERTICAL) {
        SetSizeHints(10, -1, 10, -1);  // Forces SetSize to use the proper width
    }
    else {
        SetSizeHints(-1, 10, -1, 10);  // Forces SetSize to use the proper height
    }
    // NB!  SetSizeHints is overloaded by wxSlider and will substitute 10 with the
    // proper dimensions, it also means other people cannot bugger the slider with
    // other values
    
    if(style & wxSL_LABELS)
    {
        m_macMinimumStatic = new wxStaticText( parent, -1, wxEmptyString );
        m_macMaximumStatic = new wxStaticText( parent, -1, wxEmptyString );
        m_macValueStatic = new wxStaticText( parent, -1, wxEmptyString );
        SetRange(minValue, maxValue);
        SetValue(value);
    }

    MacPostControlCreate(pos,size) ;
    
    return true;
}

wxSlider::~wxSlider()
{
    delete m_macMinimumStatic ;
    delete m_macMaximumStatic ;
    delete m_macValueStatic ;
}

int wxSlider::GetValue() const
{
    return GetControl32BitValue( (ControlRef) m_macControl) ;
}

void wxSlider::SetValue(int value)
{
    wxString valuestring ;
    valuestring.Printf( wxT("%d") , value ) ;    
    if ( m_macValueStatic )
        m_macValueStatic->SetLabel( valuestring ) ;
    SetControl32BitValue( (ControlRef) m_macControl , value ) ;
}

void wxSlider::SetRange(int minValue, int maxValue)
{
    wxString value;
    
    m_rangeMin = minValue;
    m_rangeMax = maxValue;
    
    SetControl32BitMinimum( (ControlRef) m_macControl, m_rangeMin);
    SetControl32BitMaximum( (ControlRef) m_macControl, m_rangeMax);
    
    if(m_macMinimumStatic) {
        value.Printf(wxT("%d"), m_rangeMin);
        m_macMinimumStatic->SetLabel(value);
    }
    if(m_macMaximumStatic) {
        value.Printf(wxT("%d"), m_rangeMax);
        m_macMaximumStatic->SetLabel(value);
    }
    SetValue(m_rangeMin);
}

// For trackbars only
void wxSlider::SetTickFreq(int n, int pos)
{
    // TODO
    m_tickFreq = n;
}

void wxSlider::SetPageSize(int pageSize)
{
    // TODO
    m_pageSize = pageSize;
}

int wxSlider::GetPageSize() const
{
    return m_pageSize;
}

void wxSlider::ClearSel()
{
    // TODO
}

void wxSlider::ClearTicks()
{
    // TODO
}

void wxSlider::SetLineSize(int lineSize)
{
    m_lineSize = lineSize;
    // TODO
}

int wxSlider::GetLineSize() const
{
    // TODO
    return 0;
}

int wxSlider::GetSelEnd() const
{
    // TODO
    return 0;
}

int wxSlider::GetSelStart() const
{
    // TODO
    return 0;
}

void wxSlider::SetSelection(int minPos, int maxPos)
{
    // TODO
}

void wxSlider::SetThumbLength(int len)
{
    // TODO
}

int wxSlider::GetThumbLength() const
{
    // TODO
    return 0;
}

void wxSlider::SetTick(int tickPos)
{
    // TODO
}

void wxSlider::Command (wxCommandEvent & event)
{
    SetValue (event.GetInt());
    ProcessCommand (event);
}

void wxSlider::MacHandleControlClick( WXWidget control , wxInt16 controlpart, bool mouseStillDown ) 
{
    SInt16 value = ::GetControl32BitValue( (ControlRef) m_macControl ) ;
    
    SetValue( value ) ;        
    
    wxEventType scrollEvent = wxEVT_NULL ;
    
   if ( mouseStillDown )
        scrollEvent = wxEVT_SCROLL_THUMBTRACK;
    else
        scrollEvent = wxEVT_SCROLL_THUMBRELEASE;
    
    wxScrollEvent event(scrollEvent, m_windowId);
    event.SetPosition(value);
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent(event);
    
    wxCommandEvent cevent( wxEVT_COMMAND_SLIDER_UPDATED, m_windowId );
    cevent.SetInt( value );
    cevent.SetEventObject( this );
    
    GetEventHandler()->ProcessEvent( cevent );
}

/* This is overloaded in wxSlider so that the proper width/height will always be used
* for the slider different values would cause redrawing and mouse detection problems */
void wxSlider::SetSizeHints( int minW, int minH,
                            int maxW , int maxH ,
                            int incW , int incH )
{
    wxSize size = GetBestSize();
    
    if(GetWindowStyle() & wxSL_VERTICAL) {
        wxWindow::SetSizeHints(size.x, minH, size.x, maxH, incW, incH);
    }
    else {
        wxWindow::SetSizeHints(minW, size.y, maxW, size.y, incW, incH);
    }
}

wxSize wxSlider::DoGetBestSize() const
{
    wxSize size;
    int textwidth, textheight;
    
    if(GetWindowStyle() & wxSL_LABELS)
    {
        wxString text;
        int ht, wd;
        
        // Get maximum text label width and height
        text.Printf(wxT("%d"), m_rangeMin);
        GetTextExtent(text, &textwidth, &textheight);
        text.Printf(wxT("%d"), m_rangeMax);
        GetTextExtent(text, &wd, &ht);
        if(ht > textheight) {
            textheight = ht;
        }
        if (wd > textwidth) {
            textwidth = wd;
        }
    }
    
    if(GetWindowStyle() & wxSL_VERTICAL)
    {
        if(GetWindowStyle() & wxSL_AUTOTICKS) {
            size.x = wxSLIDER_DIMENSIONACROSS_WITHTICKMARKS;
        }
        else {
            size.x = wxSLIDER_DIMENSIONACROSS_ARROW;
        }
        if(GetWindowStyle() & wxSL_LABELS) {
            size.x += textwidth + wxSLIDER_BORDERTEXT;
        }
        size.y = 150;
    }
    else
    {
        if(GetWindowStyle() & wxSL_AUTOTICKS) {
            size.y = wxSLIDER_DIMENSIONACROSS_WITHTICKMARKS;
        }
        else {
            size.y = wxSLIDER_DIMENSIONACROSS_ARROW;
        }
        if(GetWindowStyle() & wxSL_LABELS) {
            size.y += textheight + wxSLIDER_BORDERTEXT;
        }
        size.x = 150;
    }
    return size;
}

void wxSlider::DoSetSize(int x, int y, int w, int h, int sizeFlags)
{
    int  xborder, yborder;
    int  minValWidth, maxValWidth, textwidth, textheight;
    int  sliderBreadth;
    
    xborder = yborder = 0;
    
    if (GetWindowStyle() & wxSL_LABELS)
    {
        wxString text;
        int ht;
        
        // Get maximum text label width and height
        text.Printf(wxT("%d"), m_rangeMin);
        GetTextExtent(text, &minValWidth, &textheight);
        text.Printf(wxT("%d"), m_rangeMax);
        GetTextExtent(text, &maxValWidth, &ht);
        if(ht > textheight) {
            textheight = ht;
        }
        textwidth = (minValWidth > maxValWidth ? minValWidth : maxValWidth);
        
        xborder = textwidth + wxSLIDER_BORDERTEXT;
        yborder = textheight + wxSLIDER_BORDERTEXT;
        
        // Get slider breadth
        if(GetWindowStyle() & wxSL_AUTOTICKS) {
            sliderBreadth = wxSLIDER_DIMENSIONACROSS_WITHTICKMARKS;
        }
        else {
            sliderBreadth = wxSLIDER_DIMENSIONACROSS_ARROW;
        }
        
        if(GetWindowStyle() & wxSL_VERTICAL)
        {
            
            if ( m_macMinimumStatic )
                m_macMinimumStatic->Move(x + sliderBreadth + wxSLIDER_BORDERTEXT,
                y + h - yborder - textheight);
            if ( m_macMaximumStatic )
                m_macMaximumStatic->Move(x + sliderBreadth + wxSLIDER_BORDERTEXT, y + 0);
            if ( m_macValueStatic )
                m_macValueStatic->Move(0, y + h - textheight);
            h = h - yborder ;
        }
        else
        {
            if ( m_macMinimumStatic )
                m_macMinimumStatic->Move(x + 0, y + sliderBreadth + wxSLIDER_BORDERTEXT);
            if ( m_macMaximumStatic )
                m_macMaximumStatic->Move(x + w - xborder - maxValWidth / 2,
                y + sliderBreadth + wxSLIDER_BORDERTEXT);
            if ( m_macValueStatic )
                m_macValueStatic->Move(x + w - textwidth, y + 0);
            w = w - xborder ;
        }
    }
    
    wxControl::DoSetSize( x, y , w , h ,sizeFlags ) ;
}

void wxSlider::DoMoveWindow(int x, int y, int width, int height)
{
    wxControl::DoMoveWindow(x,y,width,height) ;
}
