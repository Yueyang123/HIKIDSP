#include "DIALOG.h"
#include "DSO_Win.h"
#include "ADC.h"
#include "MyDSP.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0         (GUI_ID_USER + 0x00)
#define ID_GRAPH_0         (GUI_ID_USER + 0x01)
#define ID_TEXT_0         (GUI_ID_USER + 0x03)
#define ID_TEXT_1         (GUI_ID_USER + 0x04)
#define ID_TEXT_2         (GUI_ID_USER + 0x05)

// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 128, 128, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 0, 0, 128, 118, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_0, 0, 117, 65, 10, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_1, 66, 117, 65, 10, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_2, 80, 0, 48, 10, 0, 0x64, 0 },
};

GRAPH_DATA_Handle pdataadc0,pdataadc1;
GRAPH_SCALE_Handle hScaleV,hScaleH;
u32 vfact=38;//数值相对于屏幕的比例
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, GUI_FONT_10_ASCII);
		TEXT_SetTextColor(hItem,GUI_RED);
    TEXT_SetText(hItem, "CH1:0Hz 0v");
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetText(hItem, "CH2:0Hz 0v");
		TEXT_SetTextColor(hItem,GUI_YELLOW);
    TEXT_SetFont(hItem, GUI_FONT_10_ASCII);
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
	
	  hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
		TEXT_SetText(hItem, "SR:0.5KHz");
		TEXT_SetTextColor(hItem,GUI_GREEN);
    TEXT_SetFont(hItem, GUI_FONT_10_ASCII);
		
	
  hItem =WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
	GRAPH_SetBorder(hItem,0, 0, 0, 0);
	GRAPH_SetGridVis(hItem,1);
	GRAPH_SetGridFixedX(hItem,10);
	GRAPH_SetGridDistY(hItem,20);
	
	hScaleV =GRAPH_SCALE_Create(25, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 10);
	GRAPH_SCALE_SetTextColor(hScaleV,GUI_RED);   
	GRAPH_SCALE_SetFactor(hScaleV,vfact);
	GRAPH_AttachScale(hItem,hScaleV);
	//时间轴
	hScaleH =GRAPH_SCALE_Create(-10, GUI_TA_RIGHT, GRAPH_SCALE_CF_HORIZONTAL, 10);
	GRAPH_SCALE_SetTextColor(hScaleH,GUI_DARKGREEN);   
	GRAPH_SCALE_SetFactor(hScaleH, 1);
	GRAPH_AttachScale(hItem,hScaleH); 
	
	pdataadc0 =GRAPH_DATA_YT_Create(GUI_RED, 128, 0, 0);
	GRAPH_DATA_YT_SetAlign(pdataadc0, GRAPH_ALIGN_LEFT);
	GRAPH_AttachData(hItem,pdataadc0); 
	pdataadc1 =GRAPH_DATA_YT_Create(GUI_YELLOW, 128, 0, 0);
	GRAPH_DATA_YT_SetAlign(pdataadc1, GRAPH_ALIGN_LEFT);
	GRAPH_AttachData(hItem,pdataadc1); 
	break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateWindow
*/
WM_HWIN CreateWindow(void);
WM_HWIN CreateWindow(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

void DSO_Win(void)
{
	WM_HWIN hWin;
	int i=0;
	float freq0=0,freq1=0;
	while(1)
	{
//		GRAPH_DATA_YT_Clear(pdataadc0);
//		GRAPH_DATA_YT_Clear(pdataadc1);
//		for(i=0;i<128;i++)GRAPH_DATA_YT_AddValue(pdataadc0,ADC_SourceData[i][0]/vfact);
//		for(i=0;i<128;i++)GRAPH_DATA_YT_AddValue(pdataadc1,ADC_SourceData[i][1]/vfact);

		//		freq0=Get_Signal_Freq(0);
		//		freq1=Get_Signal_Freq(1);
		for(i=0;i<128;i++)
		printf("%d\t",ADC_SourceData[i][1]);
		
		GUI_Delay(100);
	}
}