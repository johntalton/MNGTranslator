/*******************************************************
*   MNGTranslator©
*
*   @author  YNOP (ynop@acm.org)
*   @version 1.1
*   @date    Jun 23 2001
*******************************************************/
#ifndef MNG_CONFIG_VIEW_H
#define MNG_CONFIG_VIEW_H

class MNGConfigView : public BView {
public:
   MNGConfigView(BMessage *prefs);
   virtual ~MNGConfigView();
   void SaveState(BMessage *prefs);
   virtual void AttachedToWindow();
   virtual void MessageReceived(BMessage*);
private:   
   BCheckBox *cb;
};
#endif
