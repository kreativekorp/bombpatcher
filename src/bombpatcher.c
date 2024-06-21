#include <Quickdraw.h>
#include <Dialogs.h>
#include <Fonts.h>

#define ICON_SIZE        128
#define MESSAGE_SIZE      31
#define MAX_DSAT_ICONS     4
#define MAX_DSAT_MESSAGES  4
#define MAX_DSAT_RESOURCES 4

#ifndef TARGET_API_MAC_CARBON
#define NewUserItemUPP NewUserItemProc
#endif

pascal void WarningProc(DialogRef dlg, DialogItemIndex itemNo) {
  DialogItemType type;
  Handle itemH;
  Rect box;
  GetDialogItem(dlg, itemNo, &type, &itemH, &box);
  Str255 warning;
  GetIndString(warning, 128, 1);
  TextSize(24);
  TETextBox(warning+1, *warning, &box, teCenter);
  TextSize(0);
}

Str255 patchInfoString = "";
pascal void PatchInfoProc(DialogRef dlg, DialogItemIndex itemNo) {
  DialogItemType type;
  Handle itemH;
  Rect box;
  GetDialogItem(dlg, itemNo, &type, &itemH, &box);
  FrameRect(&box);
  box.top++; box.left++; box.bottom--; box.right--;
  TextFont(4); TextSize(9);
  TETextBox(patchInfoString+1, *patchInfoString, &box, teFlushDefault);
  TextFont(0); TextSize(0);
}

pascal void BoxProc(DialogRef dlg, DialogItemIndex itemNo) {
  DialogItemType type;
  Handle itemH;
  Rect box;
  GetDialogItem(dlg, itemNo, &type, &itemH, &box);
  FrameRect(&box);
}

short iconProcIconID = 0;
pascal void IconProc(DialogRef dlg, DialogItemIndex itemNo) {
  DialogItemType type;
  Handle itemH;
  Rect box;
  GetDialogItem(dlg, itemNo, &type, &itemH, &box);
  EraseRect(&box);
  if (iconProcIconID) {
    Handle iconH = GetIcon(iconProcIconID);
    if (iconH) {
      PlotIcon(&box, iconH);
      ReleaseResource(iconH);
    }
  }
}

pascal void SeparatorProc(DialogRef dlg, DialogItemIndex itemNo) {
  DialogItemType type;
  Handle itemH;
  Rect box;
  GetDialogItem(dlg, itemNo, &type, &itemH, &box);
  FillRect(&box, &qd.gray);
}

ControlHandle openButton;
ControlHandle patchButton;
ControlHandle closeButton;
ControlHandle quitButton;
Handle filePathText;
Rect patchInfoBox;
Rect iconListBox;
Rect messageListBox;
ListHandle iconList;
ListHandle messageList;
ControlHandle previewCheckbox;
Rect previewIconBox;
Handle previewText;

void setPreviewIcon(Handle resH) {
  if (resH) {
    long resType;
    Str255 resName;
    GetResInfo(resH, &iconProcIconID, &resType, resName);
    ReleaseResource(resH);
  } else {
    iconProcIconID = 0;
  }
  InvalRect(&previewIconBox);
}

void setPreviewText(Handle resH) {
  Str255 text;
  text[0] = 0;
  if (resH) {
    HLock(resH);
    while (text[0] < MESSAGE_SIZE) {
      char ch = (*resH)[text[0]];
      if (!ch) break;
      text[++text[0]] = ch;
    }
    HUnlock(resH);
    ReleaseResource(resH);
  }
  SetDialogItemText(previewText, text);
}

void updatePreview() {
  short showPreview = GetControlValue(previewCheckbox);
  if (showPreview) {
    Point iconCell = {0, 0};
    if (LGetSelect(1, &iconCell, iconList)) {
      Handle resH = Get1IndResource('ICON', iconCell.v+1);
      setPreviewIcon(resH);
    } else {
      setPreviewIcon(NULL);
    }
    Point messageCell = {0, 0};
    if (LGetSelect(1, &messageCell, messageList)) {
      Handle resH = Get1IndResource('SORY', messageCell.v+1);
      setPreviewText(resH);
    } else {
      setPreviewText(NULL);
    }
  } else {
    setPreviewIcon(NULL);
    setPreviewText(NULL);
  }
}

void myListClick(EventRecord* event, Rect* box, ListHandle list) {
  Point where;
  where = event->where;
  SetPort((*list)->port);
  GlobalToLocal(&where);
  if (PtInRect(where, box)) {
    Point prevCell = {0, 0};
    if (!LGetSelect(1, &prevCell, list)) prevCell.v = -1;
    LClick(where, event->modifiers, list);
    Point nextCell = {0, 0};
    if (!LGetSelect(1, &nextCell, list)) nextCell.v = -1;
    if (prevCell.v != nextCell.v) updatePreview();
  }
}

pascal signed char MyEventFilter(DialogRef dlg, EventRecord* event, short* item) {
  if (event->what == mouseDown) {
    myListClick(event, &iconListBox, iconList);
    myListClick(event, &messageListBox, messageList);
  }
  if (event->what == updateEvt) {
    SetPort((*iconList)->port);
    LUpdate((*iconList)->port->visRgn, iconList);
    SetPort((*messageList)->port);
    LUpdate((*messageList)->port->visRgn, messageList);
  }
  return 0;
}

void addResNames(ListHandle list, short rowIndex, long resType) {
  short resCount, i;
  Handle resH;
  short resID;
  Str255 resName;
  Point cell;
  resCount = Count1Resources(resType);
  for (i = 1; i <= resCount; i++) {
    resH = Get1IndResource(resType, i);
    GetResInfo(resH, &resID, &resType, resName);
    rowIndex = LAddRow(1, rowIndex, list);
    SetPt(&cell, 0, rowIndex);
    LSetCell(resName+1, *resName, cell, list);
    ReleaseResource(resH);
    rowIndex++;
  }
}

void selectOne(ListHandle list, short rowIndex) {
  short startIndex = ((*list)->dataBounds).top;
  short endIndex = ((*list)->dataBounds).bottom;
  Point cell = {0, 0};
  for (cell.v = startIndex; cell.v < endIndex; cell.v++) {
    LSetSelect(!rowIndex, cell, list);
    rowIndex--;
  }
  LAutoScroll(list);
}

void showMessage(short index) {
  DialogPtr dlg = GetNewDialog(129, 0, (WindowPtr)-1);
  Str255 message;
  GetIndString(message, 129, index);
  ParamText(message, NULL, NULL, NULL);
  ShowWindow(dlg);
  short item;
  for (;;) {
    ModalDialog(NULL, &item);
    if (item == 1) break;
  }
  DisposeDialog(dlg);
  ParamText(NULL, NULL, NULL, NULL);
}

typedef struct DSATInfo {
  short id;
  long size;
  Handle resH;
  short iconCount;
  long iconLoc[MAX_DSAT_ICONS];
  short messageCount;
  long messageLoc[MAX_DSAT_MESSAGES];
} DSATInfo;

short loadedVRefNum = 0;
Str255 loadedFName = "";
short loadedDSATCount = 0;
DSATInfo loadedDSAT[MAX_DSAT_RESOURCES];

void releaseDSATs() {
  while (loadedDSATCount) {
    loadedDSATCount--;
    DisposeHandle(loadedDSAT[loadedDSATCount].resH);
  }
}

short bytesEq(char * p1, char * p2, long num) {
  while (num--) if (*p1++ != *p2++) return 0;
  return 1;
}

short bytesPos(char * haystack, char * needle, long haystackSize, long needleSize, short * counter, long * locPtr, short locSize) {
  short found = 0;
  long maxPos = haystackSize - needleSize;
  for (long pos = 0; pos <= maxPos; pos++) {
    if (bytesEq(&haystack[pos], needle, needleSize)) {
      (*counter)++;
      (*locPtr) = pos;
      locPtr++;
      found++;
      if (found >= locSize) break;
    }
  }
  return found;
}

void str255cpy63(Str255 dst, Str63 src) {
  unsigned char len = dst[0] = src[0];
  while (len) { dst[len] = src[len]; len--; }
}

void str255catp(Str255 dst, char * src) {
  unsigned char dlen = dst[0], slen = src[0];
  while (dlen < 255 && slen) { dst[++dlen] = *++src; slen--; }
  dst[0] = dlen;
}

void str255catc(Str255 dst, char * src) {
  unsigned char dlen = dst[0];
  while (dlen < 255 && *src) dst[++dlen] = *src++;
  dst[0] = dlen;
}

void str255catd(Str255 dst, long value) {
  Str255 buf;
  NumToString(value, buf);
  str255catp(dst, buf);
}

void str255cath(Str255 dst, long value, short len) {
  Str255 buf;
  buf[0] = 0;
  while (value) {
    short d = value & 15;
    if (d < 10) d += '0';
    else d += 'A' - 10;
    buf[++buf[0]] = d;
    value >>= 4;
  }
  while (buf[0] < len) {
    buf[++buf[0]] = '0';
  }
  unsigned char dlen = dst[0];
  while (dlen < 255 && buf[0]) dst[++dlen] = buf[buf[0]--];
  dst[0] = dlen;
}

short getSysWDHFS(short *wdRefNum) {
  /* From Apple Tech Note #77 */
  /* Get the vRefNum of the startup disk */
  FCBPBRec myFCB;
  myFCB.ioNamePtr = NULL;
  myFCB.ioVRefNum = 0;
  myFCB.ioRefNum = *((short *)0xA58);
  myFCB.ioFCBIndx = 0;
  if (PBGetFCBInfoSync(&myFCB)) return 0;
  /* Get the dirID of the system folder */
  HParamBlockRec myHPB;
  myHPB.volumeParam.ioNamePtr = NULL;
  myHPB.volumeParam.ioVRefNum = myFCB.ioFCBVRefNum;
  myHPB.volumeParam.ioVolIndex = 0;
  if (PBHGetVInfoSync(&myHPB)) return 0;
  /* Get the WDRefNum of the system folder */
  WDPBRec myWDPB;
  myWDPB.ioNamePtr = NULL;
  myWDPB.ioVRefNum = myHPB.volumeParam.ioVRefNum;
  myWDPB.ioWDProcID = 'ERIK';
  myWDPB.ioWDDirID = myHPB.volumeParam.ioVFndrInfo[0];
  if (PBOpenWDSync(&myWDPB)) return 0;
  *wdRefNum = myWDPB.ioVRefNum;
  return 1;
}
short getSysWD() {
  if (*((short *)0x3F6) > 0) {
    /* Running on an HFS-capable machine */
    /* Documented in Apple Tech Note #66 */
    short wdRefNum;
    if (getSysWDHFS(&wdRefNum)) {
      return wdRefNum;
    }
  }
  /* Under MFS, 0x210 contains vRefNum of startup disk */
  /* Documented in Apple Tech Note #77 */
  return *((short *)0x210);
}

void makePath1(Str255 path, CInfoPBRec* pb, long dirID) {
  if (dirID == 2) return;
  Str255 dirName;
  pb->dirInfo.ioNamePtr = dirName;
  pb->dirInfo.ioDrDirID = dirID;
  if (PBGetCatInfoSync(pb)) return;
  dirID = pb->dirInfo.ioDrParID;
  makePath1(path, pb, dirID);
  str255catp(path, dirName);
  str255catc(path, ":");
}
void makePath0(Str255 path, short vRefNum) {
  WDPBRec wdpb;
  wdpb.ioNamePtr = NULL;
  wdpb.ioVRefNum = vRefNum;
  wdpb.ioWDIndex = 0;
  if (PBGetWDInfoSync(&wdpb)) return;
  CInfoPBRec pb;
  pb.dirInfo.ioVRefNum = wdpb.ioWDVRefNum;
  pb.dirInfo.ioFDirIndex = -1;
  makePath1(path, &pb, wdpb.ioWDDirID);
}
void makePath(Str255 path, SFReply* reply) {
  short oldVRefNum;
  GetVol(path, &oldVRefNum);
  SetVol(NULL, reply->vRefNum);
  short vRefNum;
  GetVol(path, &vRefNum);
  str255catc(path, ":");
  if (*((short *)0x3F6) > 0) {
    /* Running on an HFS-capable machine */
    /* Documented in Apple Tech Note #66 */
    makePath0(path, reply->vRefNum);
  }
  SetVol(NULL, oldVRefNum);
  str255catp(path, reply->fName);
}

void makePatchInfo(Str255 info) {
  Str255 dsatPfx;
  Str255 iconPfx;
  Str255 messagePfx;
  Str255 delimiter;
  GetIndString(dsatPfx, 128, 2);
  GetIndString(iconPfx, 128, 3);
  GetIndString(messagePfx, 128, 4);
  GetIndString(delimiter, 128, 5);
  info[0] = 0;
  for (short i = 0; i < loadedDSATCount; i++) {
    str255catp(info, dsatPfx);
    str255catd(info, loadedDSAT[i].id);
    if (loadedDSAT[i].iconCount) {
      str255catp(info, iconPfx);
      for (short j = 0; j < loadedDSAT[i].iconCount; j++) {
        if (j) str255catp(info, delimiter);
        str255cath(info, loadedDSAT[i].iconLoc[j], 4);
      }
    }
    if (loadedDSAT[i].messageCount) {
      str255catp(info, messagePfx);
      for (short j = 0; j < loadedDSAT[i].messageCount; j++) {
        if (j) str255catp(info, delimiter);
        str255cath(info, loadedDSAT[i].messageLoc[j], 4);
      }
    }
    str255catc(info, "\r");
  }
}

void loadFile(SFReply* reply) {
  loadedVRefNum = reply->vRefNum;
  str255cpy63(loadedFName, reply->fName);
  releaseDSATs();

  short myRF = CurResFile();
  short resCount, i;
  Handle resH;
  short resID;
  long resSize;
  Str255 resName;

  /* !!! DANGER ZONE !!! */
  /* When it comes to manipulating resources in a System file,       */
  /* I want to get in there and out of there as quickly as possible! */
  SetResLoad(0);
  short theirRF = OpenRFPerm(loadedFName, loadedVRefNum, fsRdPerm);
  if (theirRF == -1) {
    SetResLoad(1);
    showMessage(1); /* not available */
    return;
  }
  resCount = Count1Resources('DSAT');
  if (resCount < 1) {
    CloseResFile(theirRF);
    SetResLoad(1);
    UseResFile(myRF);
    showMessage(2); /* not a System file */
    return;
  }
  loadedDSATCount = 0;
  for (i = 1; i <= resCount; i++) {
    resH = Get1IndResource('DSAT', i);
    GetResInfo(resH, &resID, &resSize, resName);
    resSize = GetResourceSizeOnDisk(resH);
    LoadResource(resH);
    DetachResource(resH);
    loadedDSAT[loadedDSATCount].id = resID;
    loadedDSAT[loadedDSATCount].size = resSize;
    loadedDSAT[loadedDSATCount].resH = resH;
    loadedDSAT[loadedDSATCount].iconCount = 0;
    loadedDSAT[loadedDSATCount].messageCount = 0;
    loadedDSATCount++;
    if (loadedDSATCount >= MAX_DSAT_RESOURCES) break;
  }
  CloseResFile(theirRF);
  SetResLoad(1);
  UseResFile(myRF);
  /* End Danger Zone */

  short iconIndex = 0;
  short messageIndex = 0;
  short j;
  resCount = Count1Resources('ICON');
  for (i = 1; i <= resCount; i++) {
    resH = Get1IndResource('ICON', i);
    GetResInfo(resH, &resID, &resSize, resName);
    HLock(resH);
    for (j = 0; j < loadedDSATCount; j++) {
      HLock(loadedDSAT[j].resH);
      if (bytesPos(
        *(loadedDSAT[j].resH), *resH,
        loadedDSAT[j].size, ICON_SIZE,
        &(loadedDSAT[j].iconCount),
        loadedDSAT[j].iconLoc,
        MAX_DSAT_ICONS
      )) iconIndex = i;
      HUnlock(loadedDSAT[j].resH);
    }
    HUnlock(resH);
    ReleaseResource(resH);
  }
  resCount = Count1Resources('SORY');
  for (i = 1; i <= resCount; i++) {
    resH = Get1IndResource('SORY', i);
    GetResInfo(resH, &resID, &resSize, resName);
    HLock(resH);
    for (j = 0; j < loadedDSATCount; j++) {
      HLock(loadedDSAT[j].resH);
      if (bytesPos(
        *(loadedDSAT[j].resH), *resH,
        loadedDSAT[j].size, MESSAGE_SIZE,
        &(loadedDSAT[j].messageCount),
        loadedDSAT[j].messageLoc,
        MAX_DSAT_MESSAGES
      )) messageIndex = i;
      HUnlock(loadedDSAT[j].resH);
    }
    HUnlock(resH);
    ReleaseResource(resH);
  }
  if (!iconIndex || !messageIndex) {
    releaseDSATs();
    showMessage(3); /* not recognized */
    return;
  }

  HiliteControl(openButton, 255);
  HiliteControl(patchButton, 0);
  HiliteControl(closeButton, 0);
  HiliteControl(quitButton, 255);
  makePath(patchInfoString, reply);
  SetDialogItemText(filePathText, patchInfoString);
  makePatchInfo(patchInfoString);
  InvalRect(&patchInfoBox);
  selectOne(iconList, iconIndex-1);
  selectOne(messageList, messageIndex-1);
  updatePreview();
}

void patchFile() {
  if (!loadedFName[0] || !loadedDSATCount) {
    showMessage(4); /* no file selected */
    return;
  }
  if (loadedVRefNum == getSysWD()) {
    showMessage(8); /* current System file */
    return;
  }
  Point iconCell = {0, 0};
  if (!LGetSelect(1, &iconCell, iconList)) {
    showMessage(5); /* no icon selected */
    return;
  }
  Point messageCell = {0, 0};
  if (!LGetSelect(1, &messageCell, messageList)) {
    showMessage(6); /* no message selected */
    return;
  }
  Handle iconH = Get1IndResource('ICON', iconCell.v+1);
  if (!iconH) {
    showMessage(5); /* no icon selected */
    return;
  }
  Handle messageH = Get1IndResource('SORY', messageCell.v+1);
  if (!messageH) {
    ReleaseResource(iconH);
    showMessage(6); /* no message selected */
    return;
  }

  short i, j, k;
  HLock(iconH);
  HLock(messageH);
  for (i = 0; i < loadedDSATCount; i++) {
    HLock(loadedDSAT[i].resH);
    for (j = 0; j < loadedDSAT[i].iconCount; j++) {
      char * dst = &(*(loadedDSAT[i].resH))[loadedDSAT[i].iconLoc[j]];
      char * src = *iconH;
      k = ICON_SIZE;
      while (k) { *dst++ = *src++; k--; }
    }
    for (j = 0; j < loadedDSAT[i].messageCount; j++) {
      char * dst = &(*(loadedDSAT[i].resH))[loadedDSAT[i].messageLoc[j]];
      char * src = *messageH;
      k = MESSAGE_SIZE;
      while (k && *src) { *dst++ = *src++; k--; }
      while (k) { *dst++ = ' '; k--; }
    }
    HUnlock(loadedDSAT[i].resH);
  }
  HUnlock(iconH);
  HUnlock(messageH);
  ReleaseResource(iconH);
  ReleaseResource(messageH);

  short myRF = CurResFile();
  Handle resH;

  /* !!! DANGER ZONE !!! */
  /* When it comes to manipulating resources in a System file,       */
  /* I want to get in there and out of there as quickly as possible! */
  SetResLoad(0);
  short theirRF = OpenRFPerm(loadedFName, loadedVRefNum, fsRdWrPerm);
  if (theirRF == -1) {
    SetResLoad(1);
    showMessage(7); /* not available */
    return;
  }
  for (i = 0; i < loadedDSATCount; i++) {
    resH = Get1Resource('DSAT', loadedDSAT[i].id);
    LoadResource(resH);
    HLock(resH);
    HLock(loadedDSAT[i].resH);
    char * dst = *resH;
    char * src = *(loadedDSAT[i].resH);
    k = loadedDSAT[i].size;
    while (k) { *dst++ = *src++; k--; }
    HUnlock(resH);
    HUnlock(loadedDSAT[i].resH);
    ChangedResource(resH);
    WriteResource(resH);
    ReleaseResource(resH);
  }
  CloseResFile(theirRF);
  SetResLoad(1);
  UseResFile(myRF);
  /* End Danger Zone */

  showMessage(9); /* success */
}

void unloadFile() {
  loadedVRefNum = 0;
  loadedFName[0] = 0;
  releaseDSATs();

  HiliteControl(openButton, 0);
  HiliteControl(patchButton, 255);
  HiliteControl(closeButton, 255);
  HiliteControl(quitButton, 0);
  patchInfoString[0] = 0;
  SetDialogItemText(filePathText, patchInfoString);
  InvalRect(&patchInfoBox);
}

int main(void) {
#if !TARGET_API_MAC_CARBON
  MaxApplZone();
  MoreMasters();
  InitGraf(&qd.thePort);
  InitFonts();
  InitWindows();
  InitMenus();
  TEInit();
  InitDialogs(NULL);
#endif

  /* Create main dialog */
  DialogPtr dlg = GetNewDialog(128, 0, (WindowPtr)-1);
  DialogItemType type;
  Handle itemH;
  Rect box;
  Rect dataBounds = {0, 0, 0, 1};
  Point cSize = {0, 0};

  /* Item 1: Warning text; set draw procedure */
  GetDialogItem(dlg, 1, &type, &itemH, &box);
  SetDialogItem(dlg, 1, type, (Handle)NewUserItemUPP(&WarningProc), &box);

  /* Item 2: Open button; get handle for setting hilite, start enabled */
  GetDialogItem(dlg, 2, &type, &itemH, &box);
  openButton = (ControlHandle)itemH;
  HiliteControl(openButton, 0);

  /* Item 3: Patch button; get handle for setting hilite, start disabled */
  GetDialogItem(dlg, 3, &type, &itemH, &box);
  patchButton = (ControlHandle)itemH;
  HiliteControl(patchButton, 255);

  /* Item 4: Close button; get handle for setting hilite, start disabled */
  GetDialogItem(dlg, 4, &type, &itemH, &box);
  closeButton = (ControlHandle)itemH;
  HiliteControl(closeButton, 255);

  /* Item 5: Quit button; get handle for setting hilite, start enabled */
  GetDialogItem(dlg, 5, &type, &itemH, &box);
  quitButton = (ControlHandle)itemH;
  HiliteControl(quitButton, 0);

  /* Item 7: File path text; get handle for setting item text */
  GetDialogItem(dlg, 7, &type, &itemH, &box);
  filePathText = itemH;

  /* Item 8: Patch info text; set draw procedure, get rect for InvalRect */
  GetDialogItem(dlg, 8, &type, &itemH, &box);
  SetDialogItem(dlg, 8, type, (Handle)NewUserItemUPP(&PatchInfoProc), &box);
  patchInfoBox = box;

  /* Item 11: Icon list; set draw procedure for border, get rect */
  /* for hit detection, create list control, add resource names  */
  GetDialogItem(dlg, 11, &type, &itemH, &box);
  SetDialogItem(dlg, 11, type, (Handle)NewUserItemUPP(&BoxProc), &box);
  iconListBox = box;
  box.top++; box.left++; box.bottom--; box.right -= 16;
  iconList = LNew(&box, &dataBounds, cSize, 0, dlg, 1, 0, 0, 1);
  (*iconList)->selFlags = lOnlyOne;
  addResNames(iconList, 0, 'ICON');

  /* Item 12: Message list; set draw procedure for border, get rect */
  /* for hit detection, create list control, add resource names     */
  GetDialogItem(dlg, 12, &type, &itemH, &box);
  SetDialogItem(dlg, 12, type, (Handle)NewUserItemUPP(&BoxProc), &box);
  messageListBox = box;
  box.top++; box.left++; box.bottom--; box.right -= 16;
  messageList = LNew(&box, &dataBounds, cSize, 0, dlg, 1, 0, 0, 1);
  (*messageList)->selFlags = lOnlyOne;
  addResNames(messageList, 0, 'SORY');

  /* Item 13: Preview checkbox; get handle for setting control value */
  GetDialogItem(dlg, 13, &type, &itemH, &box);
  previewCheckbox = (ControlHandle)itemH;

  /* Item 14: Preview icon; set draw procedure, get rect for InvalRect */
  GetDialogItem(dlg, 14, &type, &itemH, &box);
  SetDialogItem(dlg, 14, type, (Handle)NewUserItemUPP(&IconProc), &box);
  previewIconBox = box;

  /* Item 15: Preview text; get handle for setting item text */
  GetDialogItem(dlg, 15, &type, &itemH, &box);
  previewText = itemH;

  /* Item 16: Separator; set draw procedure */
  GetDialogItem(dlg, 16, &type, &itemH, &box);
  SetDialogItem(dlg, 16, type, (Handle)NewUserItemUPP(&SeparatorProc), &box);

  /* Item 17: Separator; set draw procedure */
  GetDialogItem(dlg, 17, &type, &itemH, &box);
  SetDialogItem(dlg, 17, type, (Handle)NewUserItemUPP(&SeparatorProc), &box);

  /* Show dialog */
  ShowWindow(dlg);
  InitCursor();

  short item;
  for (;;) {
    ModalDialog(&MyEventFilter, &item);

    /* Open button */
    if (item == 2) {
      Point dlgorig = {46, 33};
      LocalToGlobal(&dlgorig);
      SFTypeList types = {'ZSYS', 'zsys', 'gbly', 0};
      SFReply reply;
      SFGetFile(dlgorig, " ", NULL, 3, types, NULL, &reply);
      if (reply.good) loadFile(&reply);
    }

    /* Patch button */
    if (item == 3) {
      patchFile();
    }

    /* Close button */
    if (item == 4) {
      unloadFile();
    }

    /* Quit button */
    if (item == 5) {
      break;
    }

    /* Preview checkbox */
    if (item == 13) {
      SetControlValue(previewCheckbox, !GetControlValue(previewCheckbox));
      updatePreview();
    }
  }

  /* Unload list controls, unload dialog, exit */
  LDispose(iconList);
  LDispose(messageList);
  DisposeDialog(dlg);
  FlushEvents(everyEvent, -1);
  return 0;
}
