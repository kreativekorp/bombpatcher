#include "Types.r"

resource 'DLOG' (128) {
  {35, 48, 328, 464},
  dBoxProc,
  invisible,
  noGoAway,
  0,
  128,
  "",
  centerMainScreen
};

resource 'DITL' (128) {
  {
    /* Item 1: Warning text */
    {0, 0, 36, 416},
    UserItem {enabled};
    /* Item 2: Open button */
    {39, 10, 61, 103},
    Button {enabled, "Open..."};
    /* Item 3: Patch button */
    {39, 111, 61, 204},
    Button {enabled, "Patch"};
    /* Item 4: Close button */
    {39, 212, 61, 305},
    Button {enabled, "Close"};
    /* Item 5: Quit button */
    {39, 313, 61, 406},
    Button {enabled, "Quit"};
    /* Item 6: File path label */
    {67, 10, 85, 90},
    StaticText {enabled, "Patching:"};
    /* Item 7: File path text */
    {67, 90, 85, 406},
    StaticText {enabled, ""};
    /* Item 8: Patch info text */
    {89, 10, 124, 406},
    UserItem {enabled};
    /* Item 9: Icon list label */
    {135, 10, 153, 90},
    StaticText {enabled, "Icon:"};
    /* Item 10: Message list label */
    {135, 216, 153, 296},
    StaticText {enabled, "Message:"};
    /* Item 11: Icon list */
    {153, 10, 235, 200},
    UserItem {enabled};
    /* Item 12: Message list */
    {153, 216, 235, 406},
    UserItem {enabled};
    /* Item 13: Preview checkbox */
    {259, 8, 281, 101},
    CheckBox {enabled, "Preview"};
    /* Item 14: Preview icon */
    {254, 104, 286, 136},
    UserItem {enabled};
    /* Item 15: Preview text */
    {252, 150, 270, 406},
    StaticText {enabled, ""};
    /* Item 16: Separator */
    {130, 8, 131, 408},
    UserItem {enabled};
    /* Item 17: Separator */
    {242, 8, 243, 408},
    UserItem {enabled};
  }
};

resource 'DLOG' (129) {
  {100, 86, 210, 414},
  dBoxProc,
  invisible,
  noGoAway,
  0,
  129,
  "",
  centerMainScreen
};

resource 'DITL' (129) {
  {
    /* Item 1: OK button */
    {79, 223, 99, 314},
    Button {enabled, "OK"};
    /* Item 2: Message text */
    {11, 9, 65, 314},
    StaticText {enabled, "^0"};
  }
};

resource 'STR#' (128) {
  {
    /* 1 */ "Always work on a copy!";
    /* 2 */ "DSAT ";
    /* 3 */ " Icon:";
    /* 4 */ " Message:";
    /* 5 */ ",";
  }
};

resource 'STR#' (129) {
  {
    /* 1 */ "Could not open the selected file because it is not available or it is not a System file.";
    /* 2 */ "Could not open the selected file because it is not a System file.";
    /* 3 */ "Could not open the selected file because it has been modified in a way this application does not recognize.";
    /* 4 */ "Could not patch the selected file because no file has been selected.";
    /* 5 */ "Could not patch the selected file because no icon has been selected.";
    /* 6 */ "Could not patch the selected file because no message has been selected.";
    /* 7 */ "Could not patch the selected file because it is locked or it is not available.";
    /* 8 */ "Could not patch the selected file because it is the current System file. I told you not to do that.";
    /* 9 */ "The selected file was patched successfully.";
  }
};

resource 'SIZE' (-1) {
  reserved,
  acceptSuspendResumeEvents,
  reserved,
  canBackground,
  doesActivateOnFGSwitch,
  backgroundAndForeground,
  dontGetFrontClicks,
  ignoreChildDiedEvents,
  is32BitCompatible,
#ifdef TARGET_API_MAC_CARBON
  isHighLevelEventAware,
#else
  notHighLevelEventAware,
#endif
  onlyLocalHLEvents,
  notStationeryAware,
  dontUseTextEditServices,
  reserved,
  reserved,
  reserved,
#ifdef TARGET_API_MAC_CARBON
  500 * 1024,
  500 * 1024
#else
  100 * 1024,
  100 * 1024
#endif
};
