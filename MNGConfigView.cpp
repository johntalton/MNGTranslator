/*******************************************************
*   Inferno©
*   
*   This file may be used under the license as
*   stated below. For more information about License
*   types see the included LICENSE file. If you did 
*   not recive a copy of that file contact the author.
*   
*   @author  YNOP (ynop@acm.org)
*   @version beta
*   @date    Feb 5 2000
*   @license Inferno Core Code
*******************************************************/
#include <InterfaceKit.h>

#include <stdio.h>

#include "MNGConfigView.h"
//#include "MNGTranslator.h"

struct MNG_settings{
   bool transPNG;
   bool touched;
};

extern BLocker g_settings_lock;
extern MNG_settings g_settings;

extern char translatorName[];
extern char translatorInfo[];

#define TOGGLE_PNG 'tgpg'

/*******************************************************
*   
*******************************************************/
MNGConfigView::MNGConfigView(BMessage *prefs):BView(BRect(0,0,200,200), "", B_FOLLOW_ALL_SIDES, 0){
   #if (B_BEOS_VERSION > B_BEOS_VERSION_5)
    SetViewUIColor(B_UI_PANEL_BACKGROUND_COLOR);
   #else
    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
   #endif
   
   BStringView *v = new BStringView(BRect(8,8,0,0),"",translatorName,B_FOLLOW_TOP|B_FOLLOW_LEFT);
   v->SetFont(be_bold_font);
   v->ResizeToPreferred();
   AddChild(v);

   BStringView *ver = new BStringView(BRect(8,50,0,0),"","YNOP © 2000-2002",B_FOLLOW_TOP|B_FOLLOW_LEFT);
   ver->ResizeToPreferred();
   AddChild(ver);


   BStringView *copy = new BStringView(BRect(8,70,0,0),"",translatorInfo,B_FOLLOW_TOP|B_FOLLOW_LEFT);
   copy->ResizeToPreferred();
   AddChild(copy);
   
   cb = new BCheckBox(BRect(8,90,0,0),"","Handle PNG files also?",new BMessage(TOGGLE_PNG),B_FOLLOW_LEFT);
   cb->ResizeToPreferred();
   AddChild(cb);
   
   if(g_settings.transPNG){
      cb->SetValue(B_CONTROL_ON);
   }else{
      cb->SetValue(B_CONTROL_OFF);
   }
   
   // Check for overrides
  /* bool dopng = false;
   if(prefs && (prefs->FindBool("transPNG",&dopng) == B_OK)){
      if(dopng){
         cb->SetValue(B_CONTROL_ON);
      }else{
         (new BAlert(NULL,"overrid off",""))->Go();
         cb->SetValue(B_CONTROL_OFF);
      }
   }
   
   */
}

/*******************************************************
*  
*******************************************************/
MNGConfigView::~MNGConfigView(){
   // should we dump our own prefs?
}

/*******************************************************
*  
*******************************************************/
void MNGConfigView::AttachedToWindow(){
   cb->SetTarget(this);
   BView::AttachedToWindow();
}

/*******************************************************
*
*******************************************************/
void MNGConfigView::MessageReceived(BMessage *msg){
   switch(msg->what){
   case TOGGLE_PNG:
      g_settings_lock.Lock();
      g_settings.transPNG = !g_settings.transPNG;
      g_settings.touched = true;
      g_settings_lock.Unlock();
      break;
   default:
      BView::MessageReceived(msg);
      break;
   }
}











