/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "SeriousSam/StdH.h"

#include <Engine/CurrentVersion.h>
#include "CmdLine.h"

CTString cmd_strWorld = "";  // world to load
INDEX cmd_iGoToMarker = -1;  // marker to go to
CTString cmd_strScript = ""; // script to execute
CTString cmd_strServer = ""; // server to connect to
INDEX cmd_iPort = -1;     // port to connect to
CTString cmd_strPassword = ""; // network password
CTString cmd_strOutput = ""; // output from parsing command line
BOOL cmd_bServer = FALSE;  // set to run as server
BOOL cmd_bQuickJoin = FALSE; // do not ask for players and network settings

static CTString _strCmd;

// get first next word or quoted string
CTString GetNextParam(void)
{
  // strip leading spaces/tabs
  _strCmd.TrimSpacesLeft();
  // if nothing left
  if (_strCmd=="") {
    // no word to return
    return "";
  }

  // if the first char is quote
  if (_strCmd[0]=='"') {
    // find first next quote
    char *pchClosingQuote = strchr(_strCmd+1, '"');
    // if not found
    if (pchClosingQuote==NULL) {
      // error in command line
      cmd_strOutput+=CTString(0, TRANS("Command line error!\n"));
      // finish parsing
      _strCmd = "";
      return "";
    }
    INDEX iQuote = pchClosingQuote-_strCmd;

    // get the quoted string
    CTString strWord;
    CTString strRest;
    _strCmd.Split(iQuote, strWord, strRest);
    // remove the quotes
    strWord.DeleteChar(0);
    strRest.DeleteChar(0);
    // get the word
    _strCmd = strRest;
    return strWord;

  // if the first char is not quote
  } else {
    // find first next space
    INDEX iSpace;
    INDEX ctChars = strlen(_strCmd);
    for(iSpace=0; iSpace<ctChars; iSpace++) {
      if (isspace(_strCmd[iSpace])) {
        break;
      }
    }
    // get the word string
    CTString strWord;
    CTString strRest;
    _strCmd.Split(iSpace, strWord, strRest);
    // remove the space
    strRest.DeleteChar(0);
    // get the word
    _strCmd = strRest;
    return strWord;
  }
}

// parse command line parameters and set results to static variables
void ParseCommandLine(CTString strCmd)
{
  cmd_strOutput = "";
  cmd_strOutput+=CTString(0, TRANS("Command line: '%s'\n"), (const char *) strCmd);
  // if no command line
  if (strlen(strCmd) == 0) {
    // do nothing
    return;
  }
  _strCmd = strCmd;

  FOREVER {
    CTString strWord = GetNextParam();
    if (strWord=="") {
      cmd_strOutput+="\n";
      return;
    } else if (strWord=="+level") {
      cmd_strWorld = GetNextParam();
    } else if (strWord=="+server") {
      cmd_bServer = TRUE;
    } else if (strWord=="+quickjoin") {
      cmd_bQuickJoin = TRUE;
    } else if (strWord=="+game") {
      CTString strMod = GetNextParam();
#if _SE_DEMO
      if (strMod!="SeriousSam" && strMod!="Warped") {
        FatalError(TRANS("This MOD is not allowed in demo version!"));
        return;
      }
#endif
      if (strMod!="SeriousSam") { // (we ignore default mod - always use base dir in that case)
        _fnmMod = "Mods\\"+strMod+"\\";
      }
    } else if (strWord=="+cdpath") {
      _fnmCDPath = GetNextParam();
    } else if (strWord=="+password") {
      cmd_strPassword = GetNextParam();
    } else if (strWord=="+connect") {
      cmd_strServer = GetNextParam();
      const char *pcColon = strchr(cmd_strServer, ':');
      if (pcColon!=NULL) {
        CTString strServer;
        CTString strPort;
        cmd_strServer.Split(pcColon-cmd_strServer, strServer, strPort);
        cmd_strServer = strServer;
        strPort.ScanF(":%d", &cmd_iPort);
      }
    } else if (strWord=="+script") {
      cmd_strScript = GetNextParam();
    } else if (strWord=="+goto") {
      GetNextParam().ScanF("%d", &cmd_iGoToMarker);
    } else if (strWord=="+logfile") {
      _strLogFile = GetNextParam();
    } else {
      cmd_strOutput+=CTString(0, TRANS("  Unknown option: '%s'\n"), (const char *) strWord);
    }
  }
}
