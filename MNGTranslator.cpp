/*******************************************************
*   MNGTranslator©
*
*   @author  YNOP (ynop@acm.org)
*   @version 1.1
*   @date    Jun 23 2001
*******************************************************/
#include <TranslatorAddOn.h>
#include <ByteOrder.h>
#include <View.h>
#include <Alert.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "MNGTranslator.h"
#include "libmng.h"
#include "callback.h"
#include "MNGConfigView.h"

BLocker g_settings_lock("MNG settings lock");
MNG_settings g_settings;

#define SETTINGS_FILE "MNG_translator_settings"

class PrefsLoader{
public:
   PrefsLoader();
   ~PrefsLoader();
};
PrefsLoader::PrefsLoader(){
   // Set up defaults :)
   g_settings.touched = false;
   g_settings.transPNG = false;
   
   BPath path;
   if (find_directory(B_USER_SETTINGS_DIRECTORY, &path)) {
      path.SetTo("/tmp");
   }
   path.Append(SETTINGS_FILE);
   
   FILE * f = fopen(path.Path(), "r");
   if(f){
      char line[1024];
      char name[32];
      char * ptr = NULL;
      while (true){
         line[0] = 0;
         fgets(line, 1024, f);
         if (!line[0]){
            break;
         }
         
         /* remember: line ends with \n, so printf()s don't have to */
         ptr = line;
         while(isspace(*ptr)) {
            ptr++;
         }
         
         if(*ptr == '#' || !*ptr) {	/* comment or blank */
            continue;
         }
         if (sscanf(ptr, "%31[a-zA-Z_0-9] =", name) != 1) {
            fprintf(stderr, "unknown MNGTranslator settings line: %s", line);
         }else{
            // If
            if(!strcmp(name, "handlePNG")){
               while (*ptr != '=') {
                  ptr++;
               }
               ptr++;
               if (sscanf(ptr,"%hi", &g_settings.transPNG) != 1) {
                  fprintf(stderr, "illegal setting in MNGTranslator settings: %s", ptr);
               }
            //}else if(!strcmp(name, "some_setting_name")){
            }else{
               fprintf(stderr, "unknown MNGTranslator setting: %s", line);
            }// end handle line
         }//end read line
      }// end read file
      fclose(f);
   }// end if file
}
PrefsLoader::~PrefsLoader(){
   // Prevents apps that do not change settings
   // from saveing old settings on shutdown if another
   // app did change them earlyer and has already writen
   // the settings :)
   if(g_settings.touched){
      BPath path;
      if (find_directory(B_USER_SETTINGS_DIRECTORY, &path)) {
         path.SetTo("/tmp");
      }
      path.Append(SETTINGS_FILE);
      FILE * f = fopen(path.Path(), "w");
      if(f){
         fprintf(f, "#MNGTranslator settings version %.2f\n", (float)translatorVersion/100.0);
         fprintf(f, "handlePNG = %hi\n", g_settings.transPNG);
         fclose(f);
      }
   }
}

PrefsLoader g_prefs_loader;

/*******************************************************
*   
*******************************************************/
status_t Identify(BPositionIO *inSource, const translation_format *inFormat,
                        BMessage *ioExtension, translator_info *outInfo, uint32 outType){
   /* Check that requested format is something we can deal with. */
   if((outType != 0) && 
      (outType != B_TRANSLATOR_BITMAP)
      //(outType != MNG_TYPE) Not yet we don't
     ){
      return B_NO_TRANSLATOR;
   }
   
   mng_handle mng = MNG_NULL;
   
   mngstuff *mymng = new mngstuff;
   mymng->inSource = inSource;
   mymng->bmap = NULL;
   mymng->type = mng_it_unknown;
   g_settings_lock.Lock();
   mymng->settings = g_settings;
   g_settings_lock.Unlock();
   
   // Init the lib so we can do some real work.
   mng = mng_initialize((mng_ptr)mymng,myalloc,myfree,MNG_NULL);
   if(mng == MNG_NULL){
      delete mymng;
      return B_NO_TRANSLATOR;
   }
   
   // install spiffy callbacks... 
   if((mng_setcb_openstream(mng, mymngopenstream) != 0) ||
      (mng_setcb_closestream(mng, mymngclosestream) != 0) ||
      (mng_setcb_readdata(mng, mymngreadstream) != 0) ||
      (mng_setcb_processheader(mng, mymngprocessheader) != 0)
     ){
      return B_NO_TRANSLATOR;
   }
   
   // Handle the read, if it fails (ie this is not
   // a mng file or something else) then bail out.
   mng_retcode mr = mng_read(mng);
   if(mr > MNG_NOERROR){
      mng_cleanup(&mng);
      if(mymng->bmap){ delete mymng->bmap; }
      delete mymng;
      return B_NO_TRANSLATOR;
   }
   
   // Well it must be a MNG,PNG,JNG or one of those thingys
   //printf("LibMNG can handle this file!\n");
   
   mng_cleanup(&mng);
   delete mymng->bmap;
   delete mymng;
   
   // Let the TransKit know what we found and what
   // we are about to do.
   outInfo->group =      B_TRANSLATOR_BITMAP;
   outInfo->type =       MNG_TYPE;
   outInfo->quality =    MNG_INPUT_QUALITY;
   outInfo->capability = MNG_INPUT_CAPABILITY;
   strcpy(outInfo->name, MNG_NAME);
   if(true){
      strcpy(outInfo->MIME, MNG_MIME_IMAGE);
   }else{
      strcpy(outInfo->MIME, MNG_MIME_VIDEO);
   }
   
   return B_OK;
}
/*
   myretcode = mng_create(mng);
   if(myretcode != MNG_NOERROR){
      //error
   }
   myretcode = mng_put1chunk_mhdr(mng,iW,iH,iTicks,iLayercount,iFramecount,iPlaytime,iSimplicity);
   if (myretcode != MNG_NOERROR){
      //error
   }
   MNG_EXT mng_retcode MNG_DECL mng_putchunk_text       (mng_handle       hHandle,mng_uint32       iKeywordsize, mng_pchar        zKeyword,   mng_uint32       iTextsize,     mng_pchar        zText);
   
   
   mng_putchunk_mend(mng);
   myretcode = mng_write(mng);
   if (myretcode != MNG_NOERROR){
      //errors
   }
*/
/*******************************************************
*   
*******************************************************/
status_t Translate(BPositionIO *inSource, const translator_info *inInfo,
                        BMessage *ioExtension, uint32 outType, BPositionIO *outDestination){
   // Do the actuall translation 
   int32 XSIZE = 0,YSIZE = 0;
   int out_rowbytes = 0;
   color_space out_space = B_RGB32;
   
   // setup stuff for libMNG..
   mng_handle mng = MNG_NULL;
   mngstuff *mymng = new mngstuff;   
   mymng->inSource = inSource;
   mymng->bmap = NULL;
   mymng->type = mng_it_unknown;
   g_settings_lock.Lock();
   mymng->settings = g_settings;
   g_settings_lock.Unlock();

  
   mng = mng_initialize(mymng, myalloc, myfree, MNG_NULL);
   if(mng == MNG_NULL){
      return B_NO_TRANSLATOR;
   }
   
   if((mng_setcb_getcanvasline(mng, mymnggetcanvasline) != 0) ||
      (mng_setcb_openstream(mng, mymngopenstream) != 0) ||
      (mng_setcb_closestream(mng, mymngclosestream) != 0) ||
      (mng_setcb_readdata(mng, mymngreadstream) != 0) ||
      (mng_setcb_processheader(mng, mymngprocessheader) != 0) ||
      (mng_setcb_refresh(mng, mymngrefresh) != 0) ||
      (mng_setcb_errorproc(mng, mymngerror) != 0) ||
      (mng_setcb_gettickcount(mng, mymnggetticks) != 0) ||
      (mng_setcb_settimer(mng, mymngsettimer) != 0)
     ){
      return B_NO_TRANSLATOR;
   }
   
   // Heart of MNG reader...
   mng_retcode code = mng_readdisplay(mng);
   // There are all types of error codes that 
   // can be returned here ... its just odd
   // that some are Bad errors and some are
   // only 'not finished yet' warnings
   //MNG_NEEDMOREDATA
   //MNG_NEEDTIMERWAIT
   //MNG_NEEDSECTIONWAIT 
   //MNG_NOERROR
   //MNG_NOTANANIMATION
   if(code == MNG_INVALIDSIG){
      mng_cleanup(&mng);
      if(mymng->bmap){ delete mymng->bmap; }
      delete mymng;
      return B_NO_TRANSLATOR;
   }
   
   // Well in my exp the new for BBitmap never 
   // creats a NULL obj.. its InitCheck() will
   // however return a error.
   if(mymng->bmap == NULL){
      mng_cleanup(&mng);
      delete mymng;
      return B_NO_TRANSLATOR;
   }
   
   if(mymng->bmap->InitCheck() != B_OK){
      mng_cleanup(&mng);
      delete mymng->bmap;
      delete mymng;
      return B_NO_TRANSLATOR;
   }
   
   // Becase MNG files are progressive
   // and also because they can be animated
   // we must loop through each chunk/frame
   int32 count = 0;
   while(mymng->delay && (count++ < 100)) {
      mymng->delay = 0; // just for safty
      mng_display_resume(mng);
   }
      
   XSIZE = mymng->bmap->Bounds().IntegerWidth()+1;
   YSIZE = mymng->bmap->Bounds().IntegerHeight()+1;
   out_rowbytes = mymng->bmap->BytesPerRow();
   
   /* B_TRANSLATOR_BITMAP header */
   TranslatorBitmap hdr;
   hdr.magic = B_HOST_TO_BENDIAN_INT32(B_TRANSLATOR_BITMAP);
   hdr.bounds.left = B_HOST_TO_BENDIAN_FLOAT(0);
   hdr.bounds.top = B_HOST_TO_BENDIAN_FLOAT(0);
   hdr.bounds.right = B_HOST_TO_BENDIAN_FLOAT(XSIZE-1);
   hdr.bounds.bottom = B_HOST_TO_BENDIAN_FLOAT(YSIZE-1);
   hdr.rowBytes = B_HOST_TO_BENDIAN_INT32(out_rowbytes);
   hdr.colors = (color_space)B_HOST_TO_BENDIAN_INT32(out_space);
   hdr.dataSize = B_HOST_TO_BENDIAN_INT32(out_rowbytes*YSIZE);
   // Write header to steam
   outDestination->Write(&hdr, sizeof(hdr));
   
   // Now actually wirthe the Bitmap to the stream..
   outDestination->Write(mymng->bmap->Bits(), mymng->bmap->BitsLength());
   
   // House keeping..
   mng_cleanup(&mng);
   delete mymng->bmap;
   delete mymng;
   
   return B_OK;
}


/*******************************************************
*   
*******************************************************/
status_t MakeConfig(BMessage *ioExtension,BView **outView, BRect *outExtent){
   MNGConfigView *v = new MNGConfigView(ioExtension);
   
   *outView = v;
   *outExtent = v->Bounds();
   return B_OK;
}

/*******************************************************
*   
*******************************************************/
status_t GetConfigMessage(BMessage * ioExtension){
   g_settings_lock.Lock();
   //ioExtension->AddBool("transPNG",g_settings.transPNG);   
   g_settings_lock.Unlock();
   return B_OK;
}

















