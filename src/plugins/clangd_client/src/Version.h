/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision: 68 $
 * $Id: Version.h 68 2022-07-18 19:45:22Z pecanh $
 * $HeadURL: http://svn.code.sf.net/p/cb-clangd-client/code/trunk/clangd_client/src/Version.h $
 */

#ifndef VERSION_H
#define VERSION_H
// ---------------------------------------------------------------------------
// Logging / debugging
// ---------------------------------------------------------------------------
//debugging control
#include <wx/log.h>
#include <wx/string.h>

#define LOGIT wxLogDebug
#if defined(LOGGING)
 #define LOGGING 1
 #undef LOGIT
 #define LOGIT wxLogMessage
 #define TRAP asm("int3")
#endif

//-----Release-Feature-Fix------------------
#define VERSION wxT("0.2.31 2022/07/16")
//------------------------------------------
// Release - Current development identifier
// Feature - User interface level
// Fix     - bug fix or non UI breaking addition
// ----------------------------------------------------------------------------
class AppVersion
// ----------------------------------------------------------------------------
{
    public:
        AppVersion() { m_version = VERSION;}
       ~AppVersion(){};

    wxString GetVersion(){return m_version;}

    wxString m_version;
    wxString m_AppName;
    protected:
    private:
};

#endif // VERSION_H
// ----------------------------------------------------------------------------
// Modifications
// ----------------------------------------------------------------------------
//0.2.31    Commit 2022/07/18 rev 68
//          2022/07/16
//          Parser::OnLSP_BatchTimer(): stop all background parsing when debugger is active (code rework).
//              to see if it solves "Invalid AST" notifications after using the debugger.
//          Revert use of ".utf8_string();" in ProcessLanguageClient::LSP_DidOpen()
//              and LSP_DidChange() because it caused empty buffers to be sent to clangd
//              when using chinese chars in editor text.
//              Ref: https: forums.codeblocks.org/index.php/topic,24357.msg170563.html#msg170563
//          2022/07/16 Apply AndrewCo patch #52 CCOptionsDlg changes
//          2022/07/15
//          Apply AndrewCo patch #52 ClangLocator changes
//          2022/07/13
//          Fixed: LSP_SymbolsParser::Parse() not initializing m_pControl to handle empty SemanticTokens.
//          Fixed: OnEditorActivated() crash when using menu/File/New/File...
//          Fixed: LSP messages log being closed before all open projects closed.
//          2022/07/11
//          Fix GetCompletionPopupDocumentation (again) to use completion and semantic token cache.
//          Fix completion to append "()" to functions. Was missing when function
//              was not local.
//          2022/07/9
//          Re-instate "Add function parens" options to DoAutoComplete()
//          2022/07/7
//          More santity checks for GetDocumentation() popup
//          Commited ccmanager fix to CodeBlocks repo to fix ignored completion requests.
//0.2.30    Commit 2022/06/30 rev 67
//          2022/06/29
//          Cut the chaff from hover response in Parser::OnLSP_HoverResponse()
//          Parser::LSP_ParseSemanticTokens() add sanity checks for race condition.
//              The editor requesting SemanticTokens may have been closed or deactivated.
//              Added checks in Parser::OnLSP_RequestedSemanticTokensResponse() for same reason.
//          2022/06/28
//          Create patch to ccmanager.cpp fix ignoring completion triggers with same length.
//          Replace asm("int3") with DebugLogError calls (LSP_ParseSemanticTokens() etal.)
//          Remove all redundant utf bad string usage
//          2022/06/27
//          To allow "clangd-<version>", check the clangd name with StartsWith(clangdexe) instead of <name>==clangdexe
//          Fix Namespace vs function vs other DocumentPopup display (DocumentationHelper::GenerateHTMLbyHover() )
//              Use hover info to distinguish types and add missing items to tokentree so html links display work correctly.
//          Remove non-asci junk in conpletion items and hover items. (ProcessLanguageClient::readJson(json &json))
//              example: \xE2\x80\xA6  \xE2\x86\x92 \xE2\x80\xA2
//          2022/06/22
//          Fix DocumentPopup with clangd data and re-instate GenerateHTML().
//          Use the clangd Hover and SemanticTokens responses to generate a TokenTree entry for GenerateHTML()
//          Implement clangd SemanticTokens request to get data missing in the token tree.
//          using <string>.utf8_string() on data sent to clangd eliminated most non-utf8 chars in responses
//          Eliminate remaining non-utf8 with <string>.Replace() (in completion responses)
//          Bad utf8 bytes are \xE2\x80\xA6 withing empty "()" function/method argements
//          Bad utf8 bytes are "\xE2\x86\x92" clobbering "Type:" in hover responses.
//0.2.29
//          2022/06/17 Commit 2022/06/18 rev 66
//          Produce a simplified Documentation popup until I can understand what doxygen_parser is doing.
//          2022/06/6
//          Fix crash in UpdateClassBrowserView() bec parser was still in parser list after being deleted.
//          Correct some typos (ollydbg ticket #46)
//          Change all "cbMessageBox("Editors file is not yet parsed.");" to InfoWindow::Display(...); (ollydbg ticket #47)
//          2022/06/4
//          ProxyProject was grabbing files that belong to a project.
//              pProxyProject was being placed in the parsers old m_project ptr.
//              Separated use of m_project to m_ProxyProject and m_ParsersProject
//          2022/06/1
//          Use ParserCommon:FileType() instead of Globals::FileTypeOf() to support
//              user specified files.
//          2022/05/31
//          Restrict "~ProxyProject~" to 1 cpu process only (client.cpp)
//              Avoids max cpu spikes because clangd for active project is already using half of all cores.
//0.2.28
//          2022/05/28
//          Guard previous fix with "if not platform::windows" else alot of errs with clangd 13.0.0
//          2022/05/25
//          Apply AndrewCo ticket 39; Match clangd QueryDriver file separaters to  compile_commands.json
//              to assure #include <file> headers are found.
//          2022/05/23
//          if defined cbDEBUG output messagbox on lock max tries exceeded in IncrDebugCallbackOk()
//0.2.27
//			2022/05/16
//          Honor user option "Maximum allowed Code-Completion matches"
//0.2.26
//			2022/05/16
//          Merge MacOS changes to main repo (Thanks AndrewCo)
//			Support case sensitive option for code completion
//          Fix possible crash when editor is closed before IdleTimeCallback OnEditorActivated event
//0.2.25
//          2022/05/11 More MacOS changes  (Thanks AndrewCo)
//          Fix illegal PS --no-heading param used in procutils::GetProcessNameByPid()
//          2022/05/6   Mac changes (Thanks AndrewCo)
//          Merge AndrewCo ccoptionsdlg.cpp wxFileDialog changes for the MAC
//          2022/05/4 ph
//          Change symbol "CodeCompletion" to "ClgdCompletion" to see if it solves the
//              MacOS problem when both old CodeCompletion and Clangd_client are loaded.
//              On Mac, it appears that the first occurance of "CodeCompletion" is being
//              used for both plugins. Causes multiple load errors.
//0.2.24
//          2022/04/28 ph
//          Focus LSP log tab when user saved and  have any log msgs and focus option is true.
//              Never focus LSP log when compiling and focus options is false.
//              Parser::OnLSP_DiagnosticsResponse()
//          2022/04/27 ph
//          Revert the previous changes in codecompletion.cpp,classbrowser.h,classbrowser.cpp, IdleCallbackHandler.h, parser.cpp
//              to 220425_194231.7z .
//              It appears that using the incoming event param is ok. Looks like a deep copy is be made of
//              the event when used to queue an idle call, because the event param address is different and
//               changes to the event are present.
//              And (stupid me) I lost sight that queueing a stack wxCommandEvent is also ok. Seems always done that way.
//          2022/04/26 ph
//          Recode Parser::LSP_ParseDocumentSymbols() to allocate callback params on heap, not stack.
//          2022/04/25 ph
//          Fix crash when IdleCallback uses stale event in OnEditorActivated()
//          New option for max parsers allowed during compiling.
//          Implemented in Parser::OnLSP_BatchTimer()
//0.2.23    commit 2022/04/25
//          2022/04/23-2 ph
//          Code sanity check for calls to Lock/IdleTimeCallbacks to log any loops.
//              It sets the max Lockout/callbacks to 8 and issues a DebugLogError()
//          2022/04/23 ph
//          Force ProxyParser to ReadOptions() not ReReadOptions() else options with
//              no project loaded are incorrect or missing.
//0.2.22
//          2022/04/21 ph
//          Fix a crash when clangd master path is invalid. cf., OnAppStartupDone()
//          Remove a wxSleep() call during clangd allocation. Appears no longer needed.
//          2022/04/20 ph
//          Comment out TimerRealtimeParsing timer and needReparse. Might be useful later.
//          At OnStartupDone() freeze flashing Start page when ProxyProject is closed.
//          Use timer to delay startup done work to allow splash page to close.
//          ParseManager::CreateParser() check for ProxyParser like TempParser
//              in order to update ClassBrowserView() to newly created parser.
//          2022/04/16 ph
//          Create a ProxyProject, ProxyClient, and ProxyParser in OnAppDoneStartup() to
//          use for parsing non-project associated files.
//          Do an idle time callback in OnAppDoneStartup() to allow splash to clear.
//          2022/04/15 ph
//          Revert back to using stand-alone cbProject to avoid workspace and plugin event interference.
//          IE., don't leave a loaded ProxyProject cbProject open. It interferes with the workspace tree.
//          Strategy: 1) create a new raw stand-alone cbProject. 2)Load an empty project 3)Clone (copy)the loaded project to
//              the stand-alone hidden cbProject. 4)Close the loaded project to clear any plugin events and he workspace tree.
//              Use the stand-alone hidden cbProject as a ProxyProject for clangd_client parsing of non-project files.
//          This moves previous ProxyProjects from Parsers to a single ParseManager ProxyProject.
//          The One-and-only hidden ProxyProject is used by all projects to manage non-project files
//              being parsed by clangd_client.
//0.2.21
//          2022/04/9 ph
//          When clangd reports nonexistent files in reference responses
//              if cbDEBUG not defined, write filenames to log, then ignore them.
//              It's caused by opening a file (like cbPlugin.h) with GoToDeclaration() then
//              finding references within that file.
//              Eg.: In Codecompletion.cpp, find declaration of cbPlugin then goto (about)
//              cbPlugin.h line 1030 and find references to PluginRegistrant.
//              For me, I'm getting files that used to exist, but were deleted.
//              The references are neither in the current projects .cache nor compile_command.json .
//              Deleting the .cache and compile_commands.json for the current project didn't help.
//              Solution: delete the .cache and .json from the folder that used to contain the nonexistent references.
//                  This must be caused by clangd walking up the directory structure.
//          2022/04/7 ph
//          For now, don't add proxyProject files to the compile_commands.json clangd database.
//              Let clangd search for the closest file match and use its compile flags.
//              This also avoids broadcasting compile flag changes to all plugins
//              (cbEVT_COMPILER_SET_BUILD_OPTIONS) in CompileCommandGenerator:114 & 151
//          2022/04/4 ph
//          Add SetNotifications(onOrOff); to cbProject class to avoid screwing up
//              other plugins. 'Proxy project' must not broadcast project changes.
//          2022/04/2 ph
//          Set compile flag when adding files to proxy project
//          Copy only ProjectBuildTargets to ProxyProject. Not ProjectFiles;
//              We only need the build compiler info for clangd.
//          Don't allow UpdateClassBrowser() to use ProxyProject files.
//          Check access of TokenTree in ClassBrowserBuilderThread to avoid crash.
//          2022/04/1 ph
//          Reworked avoiding problems when user enables both CodeCompletion and Clangd_client.
//          2022/03/31 ph
//          Create hidden cbProject "proxy project" to manage non-project files
//          The ProxyProject is cloned from the active project with its compile flags.
//0.2.20
//          2022/03/24 ph
//          Rework OnAttach() to handle "Manage plugins" dialog better (when enabling CodeCompletion and Clangd_client).
//          2022/03/23 ph
//          Expand macros in user specified path of clangd.exe aka .conf entry "/LLVM_MasterPath"
//          Remove wxFileExists() check for LLVM_MasterPath since it might contain unknowable macros like $(TARGET_COMPILER_DIR)
//          Add additional sanity checks in ClangLocator.cpp for missing clangd.exe responses.
//          Add check for m_InitDone from cbEVT_APP_STARTUP_DONE event to avoid freezing CB in OnPluginAttached().
//              Waiting MsgBoxes can get stuck behind the splash screen.
//0.2.19
//          2022/03/5 OnLSP_GoTo{Prev|Next}FunctionResponse - add missing functions
//              via Parser::LSP_GetSymbolsByType() & Parser::WalkDocumentSymbols()
//          2022/03/2 Restructure the LSPclient folder removing unnecessary src/include/ dirs
//          2022/03/1
//          Add "how to enable clangd_client" to OnAttach() cbMessageBox.
//0.2.18    Commit 47 2022/02/28
//           ccoptionsdlg - Correct typos Waring to Warning
//0.2.18    Commit 46 2022/02/27
//          2022/02/26 ph
//          Write a DebugLogError when the clangd version is < 13.
//          Add constructor/destructor/namespace to Goto_Prev/next function targets list
//              for OnLSP_GotoPrevFunctionResponse() and OnLSP_GotoNextFunctionRespoinse()
//0.2.17    Commit rev 45
//          2022/02/22  2022/02/24 ph gd_on
//          Set more strings translatable (gd_on)
//0.2.16    commit rev 44
//          2022/02/19
//          Modify strings for translations (gd_on)
//          2022/02/18 ph
//          Reinstate OnEditorActivatedTimer() as an IdleTimeCallback to switch classBrowser view
//              and namespace/function toolbar when activating editor of a non-active project.
//          OnEditorActivated: allow non-active projects to parse newly opened files. cc:3562
//              Changed because non-active projects now have "paused" flag set.
//0.2.15    Commit r42
//          2022/02/17
//          For linux wx30 Add wxOVERRIDE definition to LSPEventCallbackHandler.h
//          2022/02/16 ph
//          Allow user to parse non-ActiveProject files.
//          Check the server log for "Reusing preamble version" msgs to avoid hang conditions
//              when clangd does not respond to didOpen() or other requests. Very annoying clangd bug!
//          Remove necessity for OnEditorActivatedTimer() by using IdleTimeCallback.
//          Fixed crash in OnEditorActivated because Parser/IdleTimeCallbackHandler was not available.
//          Move IdleTimeCallback code to ParseManager
//          2022/02/12 ph
//          Add options to clear and focus the LSP messages tab/log
//          Do not clear LSP messages on each editor save, preserving previous msgs (ollydbg)
//          Recode clangd .cache lockFile to manage use of multiple projects in a workspace (ollydbg)
//          Add check in OnWorkspaceChanged to avoid multiple lockfile access denied messages
//          2022/02/9 ph
//          Pause background parsing of non-active projects (ollydbg)
//          Do not switch away from active compiling build log to LSP messages log (ollydbg)
//          2022/02/8 ph
//          Add TokenTreeLock test & IdleTimeCallback for ReParseProject functions
//          OnLSP_CompletionResponse() - ignore null string completions (labelValue)
//          2022/02/6 ph
//          Erase invalid utf8 chars rather than just blanking them out: DoValidateUTF8data().
//          These usually occur in clangd textDocument/completion symbols from non CB files.
//          2022/02/5 ph
//          parser.cpp - set token.id to LSP symbolKind needed by DoAutocomplete to add parens
//0.2.14
//          2022/02/4 ph
//          Avoid displaying clangd_client config messages when not the selected config page
//          Re-write clangd executable auto detect (using some AndrewCo ideas)
//0.2.13
//          2022/02/2 ph/ac
//          Add CB fix trunk rev 12689 ticket 1152
//0.2.12
//          2022/01/31_14 ph
//          Removed event.Skip() from OnReparseSelectedProject() seems to have stopped
//              unexpected exit after changing clangd.exe path, then moving mouse to logs.
//          Quote clangd command strings if needed.ProcessLanguageClient ctor.
//          Fix disappearing functions in GotoPrev/NextFunction() caused by allowing
//              callback data to be used in default event code.
//          Add messageBox that project must be reloaded/reparsed after (re)setting the clangd
//              location in settings.
//          Fix loading incorrect bitmap for Settings/configuration. Should have been "codecompletion"
//              not "clangd_client" for ConfigManager. It's a type, not a plugin.
//          Retested auto detection with no LLVM path set.
//          2022/01/26 ph
//          Show clangd_client MessageBoxes in front of Plugin manager dlg.
//0.2.11    2022/01/26 ph
//          Add code to check for libcodecompletion.so existence in IsOldCC_Enabled()
//          clangd_client_wx30-unix.cbp for image zipping to:
//              <Add after="cd src/resources && zip -rq9 ../../clangd_client.zip images && cd ../.." />
//              Thanks Andrew.
//          Remove usage/dependency on ccmanager's Settings/Editor/Code Completion checkbox.
//          Beef up verification that old CodeCompletion plugin is loaded/missing/enabled/disabled
//              code in ctor and OnAttach() function. This solves the "enable clangd_client" crash
//              when old CodeCompletion is already enabled or enabled after clangd_client is enabled.
//              Particles cannot possess the same space at the same time. And, it seems, that's also true
//              of information.
//0.2.10
//          2022/01/24 ph
//          Dont output wxUniChar in wxString::Format causing assert in DoValidateUTF8data()
//              when internationalization is enabled.
//          2022/01/22 ph
//          Assure image folder is in first level of clangd_client.zip file
//          Remove some wxSafeShowMessage() that were used for debugging
//          Add more info to 'invalid utf8' msg (thanks ollydbg)
//          Implemented some ollydbg suggestions https://forums.codeblocks.org/index.php/topic,24357.0.html
//          Clean out preamble-*.{tmp|pch} left in temp directory
//          Remove LSP:Hover results debugging msgs
//          ProcessLanguageClient dtor: allow time for pipe/ReadJson threads to terminate
//          Linux lsof: code to avoid dumb "No such file in directory" msg
//          Locks: Record current owner of symbols tree lock for use in error msgs
//0.2.09
//          2022/01/22 ac
//          Update project files to comment out the lines building clangd_client.cbplugin.
//          Update repo clangd_client_wx31_64.cbp from work clangd_client_wx31_64.cbp changes.
//          2022/01/15 ph
//          Removed accidental use of older CodeCompletion image files.
//          Moved image files in clangd_client.zip to the first level.
//          Reworked the repo upload files to accomodate the above two changes
//          Remove .cbPlugin file. It borked my system three time. Your out!
//0.2.08
//          2022/01/14 ph
//          Code to removed invalid utf8 chars from clangd responses
//          cf:client.cpp DoValidateUTF8data()
//0.2.07
//          2022/01/13 ph
//          ReadJson() clear illegal utf8 and show in debug log
//          Change project title of clangd_client-uw to clangd_client-wx31_64
//          Add MakeRepoUpload.bat to preserve clangd_client.zip and to avoid stripping the dll
//0.2.06
//          2022/01/13 AC
//          Doc typo fixed
//          Plugin zip file is deleted on before extra command in the project file to stop recursive zip file creation..
//0.2.05
//          2022/01/12
//          Updated the way the plugin was built
//          Major doc update
//0.2.04
//          2022/01/10
//          Removed old codecompletion xrc files.
//          Fix compilation when -DCB_PRECOMP is specified (needd later when part of C::B truck)
//          Sync version between this file and the plugin manifext.xml file.
//          Remove old codecompletion XRC files as per ticket 13. Also renamed the project files.
//          Sync version in manifest.xml and version.h (ticket 18)
//          Change plugin name back to show as Clangd_Client (camel case) in the plugin manager (ticket 16)
//          Add support for building "third" party plug to be installed via plugin manager (ticket 20)
//          Change clsettings.xrc to change the Clangd executable wxStaticBoxSizer text from "clangd's installation location" to "Specify clangd executable to use" (ticket 11)
//0.2.03
//          2022/01/7
//          Always return the length of a LSP record in the incoming buffer.
//              else it gets stuck there forever.
//          Fix bad length in headers because of LSP invalid utf8 chars
//          Fix json assets because of LSP invalid utf8 chars
//          2022/01/3
//          Fix bad symbol rename in Parser::OnLSP_RenameResponse( )
//          Fix bad string_ref/wxString translation in LSP_RequestRename()
//          Switch from using wxString on incoming clangd data to std::string
//          2021/12/22
//          Modifications to run on Linux
//          2021/12/14
//          Contains untested unix support
//0.0.20
//          2021/11/19
//          Moved clangd-cache lock into the .cache directory
//          2021/11/18
//          Removed hard coded filename for clangd resources folder in CodeCompletion.cpp
//          Show msg when Clangd_Client was unable to auto detect clangd installation.
//          2021/11/15
//          Fixed ReparseCurrentProject using wrong pProject
//          Moved parsing toggle (on/off) from main menu to context menu
//          2021/11/13
//          Implemented PauseParsingForReason() to freeze and un-freeze parsing.
//          Moved UI parsing toogle to ParseParsingForReason()
//          2021/11/12
//          SetInternalParsingPaused() set at ShutdownClient and UpdateClassBrowser view.
//          Fixed to set only once in LSP_ParseDocumentSymbols()
//          2021/11/11
//          Fixed choosing wrong parser/project combination in OnLSP_Event();
//          2021/11/9
//          To ccoptionsdlg.cpp/settings.xrc, added checkBox items for logging client and server logs.
//          Set unused check boxes in ccoptionsdlg.cpp to hiddent. What to do with them?
//              Does Clangd have an equivalent to these (local,global,preprocessor,macros) choices?
//          2021/11/7
//          To ccoptionsdlg.cpp/settings.xrc, added LLVM txtMasterPath, btnMasterPath, btnAutoDetect config fields.
//          2021/11/6
//          For OnLSP_ReferencesResponse() remember the log filebase and use for
//              subsequent "textDocument/definition" and "textDocument/definition"
//              responses.
//          2021/11/3
//          Remove dead code, remove double STX's in LSP headers
//          2021/11/2
//          change LSPEventSinkHandler to use a sequence integer rather than just the
//              LSP textDocument/xxx header request hdr. Send integer to clangd and use it
//              on return to execute the appropriate callback.
//              This solves some miss executions of queued requests and stolen responses
//              because multiple textDocucmet/xxx hdrs of the same type are sent at the same time.
//          2021/11/1
//          Moved all functions responding to requested clangd data to Parser class.
//          Removed some debugging msgs from ParseFunctionsAndFillToolbar()
//          2021/10/31
//          Moved OnLSP_CompletionResponse() to Parser.
//          Call LSP_ParseDocumentSymbols() directly rather than using a command event
//          2021/10/30
//          Fixed LSP_ParseDocumentSymbols() DoAddToken() miss-queing tokens esp. after RemoveFile()
//              Was not updating m_LastParent properly.
//          2021/10/15
//          Cone and delete IdleCallBack queue entry before executing the call
//          Fixed erroneous "editor is being parsed" caused by LSP_AddToServerFilesParsing()
//              in LSP_DidChange(). There's no response to clear the entry.
//          2021/10/14
//          Deprecated IsFileParsed() to stop huge amount of mutex locking.
//              Replaced it with Parser query of std::set of filenames having been parsed and
//              having received their clangd diagnostics response.
//              Except for ~ParserBase() dtor there are no mutex blocks in the UI thread.
//              If the lockTimeout fails, the function is rescheduled on the idle queue
//               or simply returns (if code allows).
//          2021/10/11-2
//          If main ccmanager's Code completion box is unchecked, go silent.
//          2021/10/11
//          Reinstated single line updates to clangd v13
//          Fixed GetTokenInFile() to search headers and implementations
//          2021/10/10
//          GenerateHTML() Don't block UI locking TokenTree
//          2021/10/9
//          RemoveLastFunction. Testing deprecation. Just return.
//          FindTokensInFile(); Require call to own TokenTree lock.
//          Enable/Test GenerateHTML.
//          Enable/Test Insert/Refactor All class methods w/o implementation.
//          Enable/test Add doxygen when implementing fuction method
//          Enable/test Documentation popup / GetDocumentation() / GetTokenInFile()
//              called in OnLSP_CompletionResponse() to get tknIndex.
//          2021/10/4
//          Fix CB crash/hang when CB shutdown with shown/floating windows (main::OnApplicationClose)
//          Hang issueing 'reparse project'. Fix: use find/remove event hander, not popEventHandler
//          Re-instated code for dialog ccdebug info (key: Alt-Shift double click tree item)
//          2021/09/28
//          Added parser.h/cpp m_nternalParsingPaused to allow code to pause file parsing //(ph 2021/07/31)
//              Especially where the code invokes a dialog that holds the TokenTree mutex lock. This
//              can cause the clangd jason responses to backup up and eat memory. Cf: OnLSP_ParseDocumentSymbols()
//          2021/09/27
//          Implement IdleCallbackHandler to avoid mutex blocking main UI thread
//              using wxMutex.lockTimeOut(). If fails, queue an idle time callback
//              to the function instead of blocking.
//          Fix crash because IdleCallbacks weren't cleared during project close;
//          2021/09/21
//          Eliminated the mutex s_ParserMutex. Was only being used by ParserThreadedTask to guard
//              m_BatchParseFiles. ParserThreadedTask is now deprecated and m_BatchParseFiles is only
//              used by Parser, each of which has its own m_BatchParseFiles.
//          2021/09/18
//          Don't update ClassBrowserView (UI symbols tree) if mouse is in the Symbls tab window.
//              User could be using it and updating it causes users to lose their selections.
//          Update ClassBrowserView only every 4 "DocumentSymbols" files processed.
//          2021/09/15
//          if Batch files queued not empty, set Batch timer to sleep to 300ms instead of 1000sec in order
//              to check finished parsering and possibly start another clangd parser.
//          2021/09/11
//          Updating symbols tree done in OnIdle() time only via Parser::OnLSP_ParseDocumentSymbols()
//          CodeCompletion::OnToolbarTimer(wxCommandEvent& event) allows CC Toolbar (class and function) to be initiated from OnLSP_ParseDocumentSymbols()
//          For GotoPrevNextFunction() revert to original CC code. Much faster than a LSP request/response cycle
//          2021/09/10
//          Fixed crash in Parser::Done(). wasn't checking ptrs before usage.
//          client.cpp: Reverted to allowing clangd to using half of processes, else
//              all hardware threads used for parsing and none left for on-the-fly requests such as completions, goto's etc.
//              Use the pid in CBCCLogger-<pid> to avoid debugged process conflicts over name.
//          2021/09/9
//          Fixed loop when class missing ending '}' in DoHandleClass()
//          Added Entry guard to ClassBrowserBuilderThread::Init() to stop dead lock with ExpandItems()
//          2021/09/15
//          if Batch files queued not empty, set Batch timer to sleep to 300ms instead of 1000sec in order
//              to check finished parsering and possibly start another clangd parser.
//          2021/09/11
//          Updating symbols tree done in OnIdle() time only via Parser::OnLSP_ParseDocumentSymbols()
//          CodeCompletion::OnToolbarTimer(wxCommandEvent& event) allows CC Toolbar (class and function) to be initiated from OnLSP_ParseDocumentSymbols()
//          For GotoPrevNextFunction() revert to original CC code. Much faster than a LSP request/response cycle
//          2021/09/10
//          Fixed crash in Parser::Done(). wasn't checking ptrs before usage.
//          client.cpp: Reverted to allowing clangd to using half of processes, else
//              all hardware threads used for parsing and none left for on-the-fly requests such as completions, goto's etc.
//              Use the pid in CBCCLogger-<pid> to avoid debugged process conflicts over name.
//          2021/09/9
//          Fixed loop when class missing ending '}' in DoHandleClass()
//          Added Entry guard to ClassBrowserBuilderThread::Init() to stop dead lock with ExpandItems()
//          2021/08/30
//          Changed cclogger to write log to \temp\CCLogger-<#>.log
//          changed cclogger to trace locks/unlocks using trylock(250). Trying to catch hang.
//          changed cclogger to assert locks/unlocks using trylock(250). Trying to catch hang.
//          Changed cclogger to send DebugLogError events
//          Fixed ParseManager::DeleteParser()  bad parser address by re-instating DeleteParser() in OnProjectClosed() 2021/09/2
//          Added m_CCDelay (config slider) support code to use as auto-code-completion delay. 2021/09/2
//          2021/08/25
//          Fixed Done() to check when active editor does not belong to active project.
//          2021/08/14
//          Added LSP_SignatureHelp() for menu item "Edit/Show calltip"
//          2021/08/11
//          Fixed LSP_hover, modified CC::GetTokenAt() and CC::GetCallTips() 2021/08/11
//          2021/08/10
//          LSPDiagnosticsResultsLog::SyncEditor() verify file actually exists 2021/08/10
//          OnLSP_DiagnosticsResponse() append filename to "LSP diagnostics:" header line. 2021/08/10
//          Deprecated parserthreadedtask.h/cpp (no longer used) 2021/08/10
//          2021/08/7
//          Move gathering files to parse from CodeCompletion::OnLSP_Event() initialization to ParseManager::DoFullParsing()  2021/08/7
//          2021/07/17
//          Get parallel parse threads from configuration. Remove unused config items.  2021/07/17
//          client.cpp Set hasChangedLineCount = true to avoid crashes when modifying single line 2021/07/26
//              It causes client to alway send the whole text of a file. Needs to be fixed
//          Removed showerr parameter from GetLSPclient(pProject|peditor). Just too confusing.   2021/07/27
//          Updated Goto[Decl|Impl] to verify file belongs to active project    2021/07/27
//          Deprecated/Removed parserthread.h/cpp 2021/07/27
//          Added parser.h/cpp m_UserParsingPaused to allow user to manually pause file parsing //(ph 2021/07/31)
//0.0.20(h) 2021/07/16
//          Support CanDetach() missing call from sdk\pluginsconfigurationdlg.cpp
//          Use CanDetach() to ask user to close workspace before uninstalling clangd_client
//          Reduce number of clangd parsing cores to 2. 3 causes 50%-90% cpu load on 4core 8thread cpu.
//          Fixed Annoying dialog to record users reponse correctly
//0.0.20(h) 2021/06/29
//          Added "LSP clangd", "LSP Clangd is not active for this project." to codecompletion::GetLSPclient(cbProject*)
// 0.0.19(h) 2021/06/14    moved to wx3.1.5
//          Fixed Re-parse now Symbols context event by invoking idParseCurrentProject event        //(ph 2021/06/14)
//          Added "Editor not yet parsed" and "...parsing delay" msg to find refs, goto decl/impls funcs. //(ph 2021/06/14)
//          changed wxCheck_msg2 to cbAssertNonFatal. wxCheck did not work in release mode. //(ph 2021/06/15)
//          For FindReferences and GotoDeclImpl, Output InfoWindow::Display() for files not contained in a project. //(ph 2021/06/15)
//          Turned on #define CC_ENABLE_LOCKER_ASSERT   2021/06/16
//          Added IsServerFilesParsing() to client.h 2021/06/16
// 0.0.18
//          Dont request symbols when only a document completion request 2021/04/6
//          Fixed crash re: double gotoFunction dialogs
//          Implemented background parse of unopened project files
//          Fixed: Stopped EncodingDector conversion msgs when file is loaded into hidden editors 2021/04/13
//          Modified CC GotoFunction dialog to use TokenTree when using LSP client 2021/04/13
//          Focus LSP messages log on didSave   2021/04/14
//          Fixed: Dont call for symbols on dumb diagnostics sent on a didclose //(ph 2021/04/15
//          Dont parse system headers, clangd already does that.    2021/04/16
//          On reparse project, call CC parse first in so it's ptr is available for LSP reparseing 2021/04/15
//          Fixed: invalid utf8 text handed to clangd in didOpen for background 2021/04/16
//          Sort background files to be parsed in last modified order   2021/04/16
//          Switch to LSP messages log only when user uses Menu or shortcut keys to save file 2021/04/16
//          For background parsing sort by Modification time to process recently used files first.  2021/04/19
//          Fixed: crash after switching lsp-to-CC-to-LSP then reparsing project. Event handers are removed.
//              Invalidated event.Skip() as both LSP and CC had been restarted before return and Skip(); 2021/04/17
//          Added pProject as token.m_UserData so that cmd titles are highlighted when parsing is finished. 2021/04/22
//          Track background parcing in m_LSP_CurrBackgroundFilesParsing array 2021/04/23
//          Changed m_LSP_CurrBackgroundFilesParsing to std::map containing filename and parse starting time. 2021/04/24
//          Write parse duration time for background parsed files to  DebugLog  2021/04/24
//          added #ifdef for SHOW_SERVER_CONSOLE in ProcessLanguageClient::ProcessLanguageClient()  2021/04/24
//              Showing the server console steals focus from CodeBlocks.
//          Dont ask for .h symbols in diagnostics response. It clobbers token tree .cpp tokens with wrong items. 2021/04/24
//          Update ClassBrowser every three completed background parsing finishes (OnLSP_ParseDocumentSymbols) 2021/04/27
//          Paused background parsing while debugger is running 2021/04/28
//          Parallel parse max of half of cpus or 3 2021/04/28
//          Always put out a message to the LSP messages log even if zero diagnostics   2021/04/28
//          Replace all log calls containing ...log(F(... with ...log(wxString::Format("",) because 2021/05/4
//                the F() call is not thread safe and causes crashes.
//          Move ClassBrowser updates to OnLSP_ParseDocumentSymbols() to avoid OnParserEnd() lock hangs. 2021/05/4
//          Write "ended parsing" msg for each file to regular log 2021/05/7
//          Verify pParser for pProject exists before creating client
//          Switch from DoLockClangd_CacheAccess() to OnDebuggerStarting() to shutdown clangd when debugging clangd. 2021/05/11
//          Stopped didOpen() of .h files during LSP initialization. But it causes goto defs/decls from .h files to fail. 2021/05/11
//          ^^^ Reverted: symbols are not requested in OnLSP_DiagnosticsResponse() for .h files. But .h files needs didOpen() for clangd. //(ph 2021/05/12)
//          Implemented Reparse single file for LSP clangd //(ph 2021/05/13)
//          Fixed GoToFunction() for both headers and implementations. 2021/05/22
//          ParseManager::DoFullParsing() Parse headers only if 'ShowInheritance" is set //(ph 2021/05/24)
//              Note: cfg setting and symbols context setting is out of sync.
//          Removed all WriteOptions() except for OnApply(). They were causing previous parsers options
//              to be applied to the next created parser after a parser delete. //(ph 2021/05/25)
//              and Modifying the config while a project was loaded was impossible. Config items wouldn't stick.
//          Removed Parser->RemoveFile() before parsing json data. Was causing crashes. 2021/06/4
//              Still don't understand how stale data is removed from Token tree.
//          Parsing headers in both foreground and background when "Show inheritance members" 2021/06/11
//          Fixed: parsing children in LSP_symbolsparser.cpp //(ph 2021/06/11)
//          Fixed some crashes because not handling typedef and class forward declaration correctly. 2021/06/12
// 0.0.17   2021/04/2
//          Support for TokenTree (symbols browser & toolbar)
//  0.0.16  2021/03/13
//          Output msg when .json or .cache is out of sync with move/deleted files. 2021/03/6
//          Delete the clangd_cache.lock file when project closed.
//          Experimental textDocument/symbol code in Parser();
//          Fixed missing GoToNextFunction() LSP routine 2021/03/17
//          Added LSP calls for textDocument/semanticTokens/full
//          Added wxMutex guards for the pipe input buffer  2021/03/18
//  0.0.15  2021/03/6
//          Add callback to OnProjectFileAdded() to delay LSP_DidOpen() until after the
//              ProjectFile* is added to the editor and the project. 2021/03/5
//          Fix OnLSP_ReferencesResponse() bad calculation of relative file names. 2021/03/6
//              Not correct fix. The .json CDB was out of sync bec. of moved files/
//  0.0.15  2021/03/4
//          LSP_didChange() - if last line of text, send full editor text. We dont know
//              how that last line used to look. So we cant calculate line and col accurately.
//          Optimize OnLSP_DiagnosticsResponse() to show 'ignored' diagnostics count.
//          Persistent user ignored diagnostics to config
//          Detect clang and clangd installation.   2021/03/3
//          Set query-driver agrument from master toolchain 2021/03/4
//          Verify clang installation and clangd version >= 12.0.0 2021/03/4
//  0.0.14  2021/02/27
//          Implemented "Reparse active project" for clangd. Also used to kill/recreate
//              the client/server to force re-reading of compile_commands.json.
//          CleanUpLSPLogs() to remove closed logs locked by windows.
//          Use clangd12, remove envPath mangling, add log "Ignore Messages" context item
//          For clangd-12 add " --resource-dir=<clang-12>" and " --query-driver=f:\\usr\\MinGW810_64seh\\**\\g*"
//          Disable clangd plugin if CodeCompletion is loaded and enabled.
//          Allow user to restart clangd client/server on unusual termination
//          Modded didChange() call to send full editor text only when line count has changed.
//              otherwise just send the single line changed.
//  0.0.13  2021/02/9
//          check for stc->active AutoComp/CallTip windows before switching log focus
//          Added wxWakeUpIdle when setting m_PendindingCompletionRequest
//          Fixed missed completion buffer causing permanent CompletionIsBusy condition
//          Implement lockFile for .Clangd-cache directory
//          Rework Get/SetCompletionTimeBusy() to count down from a set busy time
//              so it can't block forever.
//          Changed to Clangd, it's faster and less buggier
//          Start calling didClose/didOpen instead of didSave because of bug #320 which
//              causes completions to stop after a didSave.
//              https://github.com/clangd/clangd/issues/320
//  0.0.12  2021/01/31
//          Fix OnEditorActivated file-reopen asking for file instead of project initialized
//          Switch to LSP log only when last LSP request was textDocument/didSave
//          Dwell time for keyboard activity before invoking LSP_complete()
//  0.0.11  2021/01/28
//          Add wxCHECK2_MSG() for editor param in calls to LSP_ functions
//          Add wxCHECK2_MSG() to catch non response to LSP_FindReferences
//          Fix LSP log focus to not screw up code completion popup window
//          Make sure ALL json data is in the input buffer before using
//          Add "Editors file is not yet parsed." message boxes
//  0.0.10  2021/01/27
//          Removed modifying .Clangd files. None of them worked.
//          Added () and (arguments) after completion function
//  0.0.9   2021/01/23
//          Remove Manager::Yield()'s for project close
//          try/catch the hell out of OnLSP_DiagnosticsResponse()
//  0.0.8   2021/01/22
//          Optimize LSP_Shutdown() to avoid long project close time
//          Optimize reading Clangd for input reads and missed buffers
//          Create a separate client/server for each file/project to circumvent
//              Clangd's inablility to handle files outside the root_dir.
//  0.0.7   2021/01/18
//          Implemented Clangd extension request $Clangd/info
//          Decided not to used $Clangd/info as status for IsParsed(editor)
//          Add tuple status mechanism to track status of LSP IsParsed(editor)
//              last LSP request and editor position of the request.
//  0.0.6   2021/01/16
//          OnLSP_references: specific requests for declaration and definition
//          On LSP requests, require editor and editor caret position
//          Clear LSP messages log on LSP_didSave()
//          Preserve last editor*, LSPtype, and caret position for last LSP request
//          Focus LSP log to the last diagnostics header entry
//  0.0.5   2021/01/16
//          Fixed compiler_commands filename and dir relationship
//          One-Clangd-per-project implementation
//          LSP messages log implementation
//  0.0.04  2021/01/9
//          Optimized UpdateCompileCommands.json file to only update when file is changed.
//          Fixed LSP diagnostics log to not interfere with Search results log
