/*******************************************************
*   MNGTranslator©
*
*   @author  YNOP (ynop@acm.org)
*   @version 1
*   @date    Jun 23 2001
*******************************************************/
#include <TranslatorAddOn.h>
#include <Window.h>
#include <Application.h>
#include <Alert.h>
#include <Screen.h>

#include <stdio.h>
#include "libmng.h"

int main(){
   BApplication app("application/x-vnd.YNOP-MNG-translator");
   (new BAlert(NULL,"MNG Translator - MNG version :"MNG_VERSION_TEXT,"Ok"))->Go();
   
   //app.Run();
   return B_OK;
}

