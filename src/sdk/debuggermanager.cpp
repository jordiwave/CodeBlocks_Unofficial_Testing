/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 */

#include "sdk_precomp.h"
#ifndef CB_PRECOMP
    #include <wx/artprov.h>
    #include <wx/bmpbuttn.h>
    #include <wx/combobox.h>
    #include <wx/dir.h>
    #include <wx/filedlg.h>
    #include <wx/frame.h>
    #include <wx/menu.h>
    #include <wx/regex.h>
    #include <wx/settings.h>
    #include <wx/sizer.h>
    #include <wx/stattext.h>
    #include <wx/toolbar.h>

    #include "cbeditor.h"
    #include "cbexception.h"
    #include "cbplugin.h"
    #include "cbproject.h"
    #include "compilerfactory.h"
    #include "configmanager.h"
    #include "editormanager.h"
    #include "logmanager.h"
    #include "macrosmanager.h"
    #include "projectmanager.h"
#endif

#include <algorithm>
#include <sstream>

#include <wx/textfile.h>
#include <wx/xml/xml.h>
#ifdef __WXMSW__ // for wxRegKey
    #include <wx/msw/registry.h>
#endif // __WXMSW__

#include "debuggermanager.h"
#include "annoyingdialog.h"
#include "compiler.h"
#include "cbdebugger_interfaces.h"
#include "loggers.h"
#include "manager.h"

cbWatch::cbWatch() :
    m_changed(true),
    m_removed(false),
    m_expanded(false),
    m_autoUpdate(true)
{
}

cbWatch::~cbWatch()
{
    m_children.clear();
}

void cbWatch::AddChild(cb::shared_ptr<cbWatch> parent, cb::shared_ptr<cbWatch> watch)
{
    watch->m_parent = parent;
    parent->m_children.push_back(watch);
}

void cbWatch::RemoveChild(int index)
{
    std::vector<cb::shared_ptr<cbWatch> >::iterator it = m_children.begin();
    std::advance(it, index);
    m_children.erase(it);
}

inline bool TestIfMarkedForRemoval(cb::shared_ptr<cbWatch> watch)
{
    if (watch->IsRemoved())
    {
        return true;
    }
    else
    {
        watch->RemoveMarkedChildren();
        return false;
    }
}

bool cbWatch::RemoveMarkedChildren()
{
    size_t start_size = m_children.size();
    std::vector<cb::shared_ptr<cbWatch> >::iterator new_last;
    new_last = std::remove_if(m_children.begin(), m_children.end(), &TestIfMarkedForRemoval);
    m_children.erase(new_last, m_children.end());
    return start_size != m_children.size();
}
void cbWatch::RemoveChildren()
{
    m_children.clear();
}

int cbWatch::GetChildCount() const
{
    return m_children.size();
}

cb::shared_ptr<cbWatch> cbWatch::GetChild(int index)
{
    std::vector<cb::shared_ptr<cbWatch> >::iterator it = m_children.begin();
    std::advance(it, index);
    return *it;
}

cb::shared_ptr<const cbWatch> cbWatch::GetChild(int index) const
{
    std::vector<cb::shared_ptr<cbWatch> >::const_iterator it = m_children.begin();
    std::advance(it, index);
    return *it;
}

cb::shared_ptr<cbWatch> cbWatch::FindChild(const wxString & symbol)
{
    for (std::vector<cb::shared_ptr<cbWatch> >::iterator it = m_children.begin(); it != m_children.end(); ++it)
    {
        wxString s;
        (*it)->GetSymbol(s);

        if (s == symbol)
        {
            return *it;
        }
    }

    return cb::shared_ptr<cbWatch>();
}

int cbWatch::FindChildIndex(const wxString & symbol) const
{
    int index = 0;

    for (std::vector<cb::shared_ptr<cbWatch> >::const_iterator it = m_children.begin();
            it != m_children.end();
            ++it, ++index)
    {
        wxString s;
        (*it)->GetSymbol(s);

        if (s == symbol)
        {
            return index;
        }
    }

    return -1;
}

cb::shared_ptr<const cbWatch> cbWatch::GetParent() const
{
    return m_parent.lock();
}

cb::shared_ptr<cbWatch> cbWatch::GetParent()
{
    return m_parent.lock();
}

bool cbWatch::IsRemoved() const
{
    return m_removed;
}

bool cbWatch::IsChanged() const
{
    return m_changed;
}

void cbWatch::MarkAsRemoved(bool flag)
{
    m_removed = flag;
}

void cbWatch::MarkChildsAsRemoved()
{
    for (std::vector<cb::shared_ptr<cbWatch> >::iterator it = m_children.begin(); it != m_children.end(); ++it)
    {
        (*it)->MarkAsRemoved(true);
    }
}
void cbWatch::MarkAsChanged(bool flag)
{
    m_changed = flag;
}

void cbWatch::MarkAsChangedRecursive(bool flag)
{
    m_changed = flag;

    for (std::vector<cb::shared_ptr<cbWatch> >::iterator it = m_children.begin(); it != m_children.end(); ++it)
    {
        (*it)->MarkAsChangedRecursive(flag);
    }
}

bool cbWatch::IsExpanded() const
{
    return m_expanded;
}

void cbWatch::Expand(bool expand)
{
    m_expanded = expand;
}

bool cbWatch::IsAutoUpdateEnabled() const
{
    return m_autoUpdate;
}

void cbWatch::AutoUpdate(bool enabled)
{
    m_autoUpdate = enabled;
}

wxString cbWatch::MakeSymbolToAddress() const
{
    wxString symbol;
    GetSymbol(symbol);
    return symbol;
}

bool cbWatch::IsPointerType() const
{
    return false;
}

cb::shared_ptr<cbWatch> DLLIMPORT cbGetRootWatch(cb::shared_ptr<cbWatch> watch)
{
    cb::shared_ptr<cbWatch> root = watch;

    while (root)
    {
        cb::shared_ptr<cbWatch> parent = root->GetParent();

        if (!parent)
        {
            break;
        }

        root = parent;
    }

    return root;
}

cbStackFrame::cbStackFrame() :
    m_valid(false)
{
}

void cbStackFrame::SetNumber(int number)
{
    m_number = number;
}

void cbStackFrame::SetAddress(uint64_t address)
{
    m_address = address;
}

void cbStackFrame::SetSymbol(const wxString & symbol)
{
    m_symbol = symbol;
}

void cbStackFrame::SetFile(const wxString & filename, const wxString & line)
{
    m_file = filename;
    m_line = line;
}

void cbStackFrame::MakeValid(bool flag)
{
    m_valid = flag;
}

int cbStackFrame::GetNumber() const
{
    return m_number;
}

uint64_t cbStackFrame::GetAddress() const
{
    return m_address;
}

wxString cbStackFrame::GetAddressAsString() const
{
    if (m_address != 0)
    {
        return cbDebuggerAddressToString(m_address);
    }
    else
    {
        return wxEmptyString;
    }
}

const wxString & cbStackFrame::GetSymbol() const
{
    return m_symbol;
}

const wxString & cbStackFrame::GetFilename() const
{
    return m_file;
}

const wxString & cbStackFrame::GetLine() const
{
    return m_line;
}

bool cbStackFrame::IsValid() const
{
    return m_valid;
}

cbThread::cbThread()
{
}

cbThread::cbThread(bool active, int number, const wxString & info)
{
    m_active = active;
    m_number = number;
    m_info = info;
}

bool cbThread::IsActive() const
{
    return m_active;
}

int cbThread::GetNumber() const
{
    return m_number;
}

const wxString & cbThread::GetInfo() const
{
    return m_info;
}

cbDebuggerConfiguration::cbDebuggerConfiguration(const ConfigManagerWrapper & config) :
    m_config(config),
    m_menuId(wxID_ANY)
{
}

cbDebuggerConfiguration::cbDebuggerConfiguration(const cbDebuggerConfiguration & o) :
    m_config(o.m_config),
    m_name(o.m_name)
{
}

void cbDebuggerConfiguration::SetName(const wxString & name)
{
    m_name = name;
}
const wxString & cbDebuggerConfiguration::GetName() const
{
    return m_name;
}

const ConfigManagerWrapper & cbDebuggerConfiguration::GetConfig() const
{
    return m_config;
}

void cbDebuggerConfiguration::SetConfig(const ConfigManagerWrapper & config)
{
    m_config = config;
}

void cbDebuggerConfiguration::SetMenuId(long id)
{
    m_menuId = id;
}

long cbDebuggerConfiguration::GetMenuId() const
{
    return m_menuId;
}

bool cbDebuggerCommonConfig::GetFlag(Flags flag)
{
    ConfigManager * c = Manager::Get()->GetConfigManager(wxT("debugger_common"));

    switch (flag)
    {
        case AutoBuild:
            return c->ReadBool(wxT("/common/auto_build"), true);

        case AutoSwitchFrame:
            return c->ReadBool(wxT("/common/auto_switch_frame"), true);

        case ShowDebuggersLog:
            return c->ReadBool(wxT("/common/debug_log"), false);

        case JumpOnDoubleClick:
            return c->ReadBool(wxT("/common/jump_on_double_click"), false);

        case RequireCtrlForTooltips:
            return c->ReadBool(wxT("/common/require_ctrl_for_tooltips"), false);

        case ShowTemporaryBreakpoints:
            return c->ReadBool(wxT("/common/show_temporary_breakpoints"), false);

        default:
            return false;
    }
}

void cbDebuggerCommonConfig::SetFlag(Flags flag, bool value)
{
    ConfigManager * c = Manager::Get()->GetConfigManager(wxT("debugger_common"));

    switch (flag)
    {
        case AutoBuild:
            c->Write(wxT("/common/auto_build"), value);
            break;

        case AutoSwitchFrame:
            c->Write(wxT("/common/auto_switch_frame"), value);
            break;

        case ShowDebuggersLog:
            c->Write(wxT("/common/debug_log"), value);
            break;

        case JumpOnDoubleClick:
            c->Write(wxT("/common/jump_on_double_click"), value);
            break;

        case RequireCtrlForTooltips:
            c->Write(wxT("/common/require_ctrl_for_tooltips"), value);
            break;

        case ShowTemporaryBreakpoints:
            c->Write(wxT("/common/show_temporary_breakpoints"), value);

        default:
            ;
    }
}

wxString cbDebuggerCommonConfig::GetValueTooltipFont()
{
    wxFont system = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    system.SetPointSize(std::max(system.GetPointSize() - 3, 7));
    wxString defaultFont = system.GetNativeFontInfo()->ToString();
    ConfigManager * c = Manager::Get()->GetConfigManager(wxT("debugger_common"));
    wxString configFont = c->Read(wxT("/common/tooltip_font"));
    return configFont.empty() ? defaultFont : configFont;
}

void cbDebuggerCommonConfig::SetValueTooltipFont(const wxString & font)
{
    const wxString & oldFont = GetValueTooltipFont();

    if (font != oldFont && !font.empty())
    {
        ConfigManager * c = Manager::Get()->GetConfigManager(wxT("debugger_common"));
        c->Write(wxT("/common/tooltip_font"), font);
    }
}

cbDebuggerCommonConfig::Perspective cbDebuggerCommonConfig::GetPerspective()
{
    ConfigManager * c = Manager::Get()->GetConfigManager(wxT("debugger_common"));
    int v = c->ReadInt(wxT("/common/perspective"),
                       static_cast<int>(Perspective::OnePerDebuggerConfig));

    if (v < Perspective::OnlyOne || v > Perspective::UseCurrent)
    {
        return Perspective::OnePerDebuggerConfig;
    }

    return static_cast<Perspective>(v);
}

void cbDebuggerCommonConfig::SetPerspective(int perspective)
{
    ConfigManager * c = Manager::Get()->GetConfigManager(wxT("debugger_common"));

    if (perspective < Perspective::OnlyOne || perspective > Perspective::UseCurrent)
    {
        perspective = Perspective::OnePerDebuggerConfig;
    }

    c->Write(wxT("/common/perspective"), perspective);
}

static bool EvalXMLPlatform(const wxXmlNode * node)
{
    bool val = false;
    wxString test;

    if (node->GetAttribute("platform", &test))
    {
        if (test == "windows")
        {
            val = platform::windows;
        }
        else
            if (test == "macosx")
            {
                val = platform::macosx;
            }
            else
                if (test == "linux")
                {
                    val = platform::Linux;
                }
                else
                    if (test == "freebsd")
                    {
                        val = platform::freebsd;
                    }
                    else
                        if (test == "netbsd")
                        {
                            val = platform::netbsd;
                        }
                        else
                            if (test == "openbsd")
                            {
                                val = platform::openbsd;
                            }
                            else
                                if (test == "darwin")
                                {
                                    val = platform::darwin;
                                }
                                else
                                    if (test == "solaris")
                                    {
                                        val = platform::solaris;
                                    }
                                    else
                                        if (test == "unix")
                                        {
                                            val = platform::Unix;
                                        }
    }

    return val;
}

static wxString SearchForDebuggerExecutable(wxString pathParam, const wxString & exeNameParam)
{
    LogManager * pLogMgr = Manager::Get()->GetLogManager();
    wxFileName fnDetectDebuggerExecutable;
    fnDetectDebuggerExecutable.SetFullName(exeNameParam);
    pathParam = pathParam.Trim().Trim(false);
    fnDetectDebuggerExecutable.SetPath(pathParam);

    if (fnDetectDebuggerExecutable.FileExists())
    {
        pLogMgr->DebugLog(wxString::Format(_("SearchForDebuggerExecutable detected %s"), fnDetectDebuggerExecutable.GetFullPath()));
        return fnDetectDebuggerExecutable.GetFullPath();
    }
    else
    {
        fnDetectDebuggerExecutable.SetPath(pathParam + wxFILE_SEP_PATH + "bin");

        if (fnDetectDebuggerExecutable.FileExists())
        {
            pLogMgr->DebugLog(wxString::Format(_("SearchForDebuggerExecutable detected %s"), fnDetectDebuggerExecutable.GetFullPath()));
            return fnDetectDebuggerExecutable.GetFullPath();
        }
    }

    return wxEmptyString;
}

static wxString SearchCompilerXMLFiles(const wxString & compilerIDLookup)
{
    // register pure XML compilers
    // user paths first
    wxDir dir;
    wxString filename;
    wxArrayString compilers;
    wxString path = ConfigManager::GetFolder(sdDataUser) + "/compilers/";

    if (wxDirExists(path) && dir.Open(path))
    {
        bool ok = dir.GetFirst(&filename, "compiler_*.xml", wxDIR_FILES);

        while (ok)
        {
            compilers.Add(path + filename);
            ok = dir.GetNext(&filename);
        }
    }

    // global paths next
    path = ConfigManager::GetFolder(sdDataGlobal) + "/compilers/";

    if (wxDirExists(path) && dir.Open(path))
    {
        bool ok = dir.GetFirst(&filename, "compiler_*.xml", wxDIR_FILES);

        while (ok)
        {
            for (size_t i = 0; i < compilers.GetCount(); ++i)
            {
                if (compilers[i].EndsWith(filename))
                {
                    ok = false;
                    break;
                }
            }

            if (ok) // user compilers of the same name take precedence
            {
                compilers.Add(path + filename);
            }

            ok = dir.GetNext(&filename);
        }
    }

    bool nonPlatComp = Manager::Get()->GetConfigManager("compiler")->ReadBool("/non_plat_comp", false);

    for (size_t i = 0; i < compilers.GetCount(); ++i)
    {
        wxXmlDocument compiler;

        if (!compiler.Load(compilers[i]) || compiler.GetRoot()->GetName() != "CodeBlocks_compiler")
        {
            Manager::Get()->GetLogManager()->LogError(wxString::Format(_("Error: Invalid Code::Blocks compiler definition '%s'."), compilers[i]));
        }
        else
        {
            bool val = true;
            wxString test;

            if (!nonPlatComp && compiler.GetRoot()->GetAttribute("platform", &test))
            {
                if (test == "windows")
                {
                    val = platform::windows;
                }
                else
                    if (test == "macosx")
                    {
                        val = platform::macosx;
                    }
                    else
                        if (test == "linux")
                        {
                            val = platform::Linux;
                        }
                        else
                            if (test == "freebsd")
                            {
                                val = platform::freebsd;
                            }
                            else
                                if (test == "netbsd")
                                {
                                    val = platform::netbsd;
                                }
                                else
                                    if (test == "openbsd")
                                    {
                                        val = platform::openbsd;
                                    }
                                    else
                                        if (test == "darwin")
                                        {
                                            val = platform::darwin;
                                        }
                                        else
                                            if (test == "solaris")
                                            {
                                                val = platform::solaris;
                                            }
                                            else
                                                if (test == "unix")
                                                {
                                                    val = platform::Unix;
                                                }
            }

            if (val)
            {
                wxString xmlCompilerName = compiler.GetRoot()->GetAttribute("name", wxEmptyString);
                wxString xmlCompilerID = compiler.GetRoot()->GetAttribute("id", wxEmptyString);

                if (xmlCompilerID.IsSameAs(compilerIDLookup))
                {
                    return compilers[i];
                }
            }
        }
    }

    return wxEmptyString;
}

wxString cbDetectDebuggerExecutable(const wxString & compilerID, const wxString & exeNameParam)
{
    LogManager * pLogMgr = Manager::Get()->GetLogManager();
    wxString masterPath = wxEmptyString;
    wxFileName exeName(exeNameParam);

    if (platform::windows)
    {
        if (exeName.GetExt().empty())
        {
            exeName.SetExt("exe");
        }
    }

    // Check Project default compiler path to see if file in it
    cbProject * pProject = Manager::Get()->GetProjectManager()->GetActiveProject();

    if (pProject)
    {
        // pLogMgr->DebugLog(wxString::Format("cbDetectDebuggerExecutable pProject found.(Line %d)", __LINE__));
        int compilerIdx = CompilerFactory::GetCompilerIndex(pProject->GetCompilerID());

        if (compilerIdx != -1)
        {
            Compiler * prjCompiler = CompilerFactory::GetCompiler(compilerIdx);

            if (prjCompiler)
            {
                masterPath = prjCompiler->GetMasterPath();

                if (!masterPath.IsEmpty())
                {
                    wxString debuggerSearchResult = SearchForDebuggerExecutable(masterPath, exeName.GetFullName());

                    if (!debuggerSearchResult.IsEmpty())
                    {
                        // pLogMgr->DebugLog(wxString::Format("cbDetectDebuggerExecutable : debugger %s (Line %d)", debuggerSearchResult, __LINE__));
                        return debuggerSearchResult;
                    }
                }
            }
        }
    }

    // else
    // {
    //    pLogMgr->DebugLog(wxString::Format("cbDetectDebuggerExecutable no project found (Line %d)",  __LINE__));
    // }
    // Check default compiler path to see if file in it
    Compiler * defaultCompiler = CompilerFactory::GetDefaultCompiler();

    if (defaultCompiler)
    {
        masterPath = defaultCompiler->GetMasterPath();

        // pLogMgr->DebugLog(wxString::Format("cbDetectDebuggerExecutable defaultCompiler found. masterPath %s  (Line %d)", masterPath, __LINE__));
        if (!masterPath.IsEmpty() && wxDirExists(masterPath + wxFILE_SEP_PATH + "bin"))
        {
            wxString debuggerSearchResult = SearchForDebuggerExecutable(masterPath, exeName.GetFullName());

            if (!debuggerSearchResult.IsEmpty())
            {
                // pLogMgr->DebugLog(wxString::Format("cbDetectDebuggerExecutable : debugger %s (Line %d)", debuggerSearchResult, __LINE__));
                return debuggerSearchResult;
            }
        }
    }

    // else
    // {
    //     pLogMgr->DebugLog(wxString::Format("cbDetectDebuggerExecutable no defaultCompiler found (Line %d)",  __LINE__));
    // }
    wxString exePath = cbFindFileInPATH(exeName.GetFullName());

    if (exePath.IsEmpty())
    {
        // Search now based on the compiler.xml file. Code taken from CompilerXML::AutoDetectInstallationDir() in compilerXML.cpp
        if (!compilerID.IsEmpty())
        {
            wxString XMLFileName = SearchCompilerXMLFiles(compilerID);

            if (!XMLFileName.IsEmpty())
            {
                // pLogMgr->DebugLog(wxString::Format("cbDetectDebuggerExecutable : checking XML file %s (Line %d)", XMLFileName, __LINE__));
                enum SearchMode
                {
                    SM_Master,
                    SM_Other
                };
                wxXmlDocument compiler;

                if (compiler.Load(XMLFileName))
                {
                    wxXmlNode * node = compiler.GetRoot()->GetChildren();
                    int depth = 0;
                    SearchMode sm = SM_Other;

                    while (node)
                    {
                        if (
                            node->GetName() == "if" &&
                            node->GetChildren()
                        )
                        {
                            if (EvalXMLPlatform(node))
                            {
                                node = node->GetChildren();
                                ++depth;
                                continue;
                            }
                            else
                                if (node->GetNext() &&
                                        node->GetNext()->GetName() == "else" &&
                                        node->GetNext()->GetChildren()
                                   )
                                {
                                    node = node->GetNext()->GetChildren();
                                    ++depth;
                                    continue;
                                }
                        }
                        else
                            if (node->GetName() == "Path" &&
                                    node->GetChildren()
                               )
                            {
                                wxString value = node->GetAttribute("type", wxEmptyString);

                                if (value == "master")
                                {
                                    sm = SM_Master;
                                }
                                else
                                {
                                    sm = SM_Other;
                                }
                            }
                            else
                                if (node->GetName() == "Search" &&
                                        sm == SM_Master
                                   )
                                {
                                    wxString value;

                                    if (node->GetAttribute("envVar", &value))
                                    {
                                        wxString pathValues;
                                        wxGetEnv(value, &pathValues);

                                        if (!pathValues.IsEmpty())
                                        {
                                            wxArrayString pathArray = GetArrayFromString(pathValues, wxPATH_SEP);

                                            for (size_t i = 0; i < pathArray.GetCount(); ++i)
                                            {
                                                wxString debuggerSearchResult = SearchForDebuggerExecutable(pathArray[i], exeNameParam);

                                                if (!debuggerSearchResult.IsEmpty())
                                                {
                                                    // pLogMgr->DebugLog(wxString::Format("cbDetectDebuggerExecutable : debugger %s (Line %d)", debuggerSearchResult, __LINE__));
                                                    return debuggerSearchResult;
                                                }
                                            }
                                        }
                                    }
                                    else
                                        if (node->GetAttribute("path", &value))
                                        {
                                            if (wxIsWild(value))
                                            {
                                                wxString path = wxFindFirstFile(value, wxDIR);

                                                if (!path.IsEmpty())
                                                {
                                                    wxString debuggerSearchResult = SearchForDebuggerExecutable(path, exeNameParam);

                                                    if (!debuggerSearchResult.IsEmpty())
                                                    {
                                                        // pLogMgr->DebugLog(wxString::Format("cbDetectDebuggerExecutable : debugger %s (Line %d)", debuggerSearchResult, __LINE__));
                                                        return debuggerSearchResult;
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                wxString debuggerSearchResult = SearchForDebuggerExecutable(value, exeNameParam);

                                                if (!debuggerSearchResult.IsEmpty())
                                                {
                                                    // pLogMgr->DebugLog(wxString::Format("cbDetectDebuggerExecutable : debugger %s (Line %d)", debuggerSearchResult, __LINE__));
                                                    return debuggerSearchResult;
                                                }
                                            }
                                        }
                                        else
                                            if (node->GetAttribute("file", &value))
                                            {
                                                wxString regexp = node->GetAttribute("regex", wxEmptyString);
                                                int idx = wxAtoi(node->GetAttribute("index", "0"));
                                                wxRegEx re;

                                                if (wxFileExists(value) && re.Compile(regexp))
                                                {
                                                    wxTextFile file(value);

                                                    for (size_t i = 0; i < file.GetLineCount(); ++i)
                                                    {
                                                        if (re.Matches(file.GetLine(i)))
                                                        {
                                                            wxString debuggerSearchResult = SearchForDebuggerExecutable(re.GetMatch(file.GetLine(i), idx), exeNameParam);

                                                            if (!debuggerSearchResult.IsEmpty())
                                                            {
                                                                // pLogMgr->DebugLog(wxString::Format("cbDetectDebuggerExecutable : debugger %s (Line %d)", debuggerSearchResult, __LINE__));
                                                                return debuggerSearchResult;
                                                            }
                                                        }
                                                    }
                                                }
                                            }

#ifdef __WXMSW__ // for wxRegKey
                                            else
                                                if (node->GetAttribute("registry", &value))
                                                {
                                                    wxRegKey key;
                                                    wxString dir;
                                                    key.SetName(value);

                                                    if (key.Exists() && key.Open(wxRegKey::Read))
                                                    {
                                                        key.QueryValue(node->GetAttribute("value", wxEmptyString), dir);

                                                        if (!dir.IsEmpty() && wxDirExists(dir))
                                                        {
                                                            wxString debuggerSearchResult = SearchForDebuggerExecutable(dir, exeNameParam);

                                                            if (!debuggerSearchResult.IsEmpty())
                                                            {
                                                                // pLogMgr->DebugLog(wxString::Format("cbDetectDebuggerExecutable : debugger %s (Line %d)", debuggerSearchResult, __LINE__));
                                                                return debuggerSearchResult;
                                                            }
                                                        }

                                                        key.Close();
                                                    }
                                                }

#endif // __WXMSW__
                                }
                                else
                                    if (node->GetName() == "Add")
                                    {
                                        wxString value;

                                        if (sm == SM_Master)
                                        {
                                            wxString path;
                                            wxXmlNode * child = node->GetChildren();

                                            while (child)
                                            {
                                                if (child->GetType() == wxXML_TEXT_NODE || child->GetType() == wxXML_CDATA_SECTION_NODE)
                                                {
                                                    path << child->GetContent();
                                                }
                                                else
                                                    if (child->GetName() == "master")
                                                    {
                                                        if (!masterPath.IsEmpty())
                                                        {
                                                            path << masterPath;
                                                        }
                                                    }
                                                    else
                                                        if (child->GetName() == "separator")
                                                        {
                                                            path << wxFILE_SEP_PATH;
                                                        }
                                                        else
                                                            if (child->GetName() == "envVar")
                                                            {
                                                                value = child->GetAttribute("default", wxEmptyString);
                                                                wxGetEnv(child->GetAttribute("value", wxEmptyString), &value);
                                                                path << value;
                                                            }

                                                child = child->GetNext();
                                            }

                                            wxString debuggerSearchResult = SearchForDebuggerExecutable(path, exeNameParam);

                                            if (!debuggerSearchResult.IsEmpty())
                                            {
                                                // pLogMgr->DebugLog(wxString::Format("cbDetectDebuggerExecutable : debugger %s (Line %d)", debuggerSearchResult, __LINE__));
                                                return debuggerSearchResult;
                                            }
                                        }
                                    }
                                    else
                                        if ((node->GetName() == "Fallback") && (sm == SM_Master))
                                        {
                                            wxString value = node->GetAttribute("path", wxEmptyString);
                                            // pLogMgr->DebugLog(wxString::Format("cbDetectDebuggerExecutable : debugger %s (Line %d)", value, __LINE__));
                                            return value;
                                        }

                        while ((!node->GetNext() || sm == SM_Master) && depth > 0)
                        {
                            node = node->GetParent();

                            if (node->GetName() == "Path")
                            {
                                sm = SM_Other;
                            }

                            --depth;
                        }

                        node = node->GetNext();
                    }
                }
                else
                {
                    pLogMgr->DebugLogError(wxString::Format("cbDetectDebuggerExecutable : Error loading XML file %s (%s line %d)", XMLFileName, __FILE__, __LINE__));
                }
            }
        }

        exePath = wxEmptyString;
    }

    if (!wxDirExists(exePath))
    {
        return wxEmptyString;
    }

    return exePath + wxFILE_SEP_PATH + exeName.GetFullName();
}

uint64_t cbDebuggerStringToAddress(const wxString & address)
{
    const wxString::size_type length = address.length();

    if (length == 0)
    {
        return 0;
    }

    wxString::size_type ii = 0;

    // Skip 0x at the beginning
    if (length >= 2 && address[0] == '0' && address[1] == 'x')
    {
        ii += 2;
    }

    uint64_t result = 0;

    for (; ii < address.length(); ++ii)
    {
        wxUniChar ch = address[ii];

        if (unsigned(ch - '0') < 10)
        {
            result *= 16;
            result += ch - '0';
        }
        else
            if (ch >= 'a' && ch <= 'f')
            {
                result *= 16;
                result += 10 + (ch - 'a');
            }
            else
                if (ch >= 'A' && ch <= 'F')
                {
                    result *= 16;
                    result += 10 + (ch - 'A');
                }
                else
                    if (ch == '`')
                        ; // This is used in 64 bit addresses in CDB, so skip it.
                    else
                    {
                        return 0;
                    }
    }

    return result;
}

wxString cbDebuggerAddressToString(uint64_t address)
{
    std::stringstream s;
    s << "0x" << std::hex << address;
    return wxString(s.str().c_str(), wxConvUTF8);
}

class DebugTextCtrlLogger : public TextCtrlLogger
{
    public:
        DebugTextCtrlLogger(bool fixedPitchFont, bool debugLog) :
            TextCtrlLogger(fixedPitchFont),
            m_panel(nullptr),
            m_debugLog(debugLog)
        {
        }

        wxWindow * CreateTextCtrl(wxWindow * parent)
        {
            return TextCtrlLogger::CreateControl(parent);
        }

        wxWindow * CreateControl(wxWindow * parent) override;

    private:
        wxPanel * m_panel;
        bool    m_debugLog;
};

class DebugLogPanel : public wxPanel
{
    public:
        DebugLogPanel(wxWindow * parent, DebugTextCtrlLogger * text_control_logger, bool debug_log) :
            wxPanel(parent),
            m_text_control_logger(text_control_logger),
            m_debug_log(debug_log)
        {
            int idDebug_LogEntryControl = wxNewId();
            int idDebug_ExecuteButton = wxNewId();
            int idDebug_ClearButton = wxNewId();
            int idDebug_LoadButton = wxNewId();
            wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
            wxBoxSizer * control_sizer = new wxBoxSizer(wxHORIZONTAL);
            wxWindow * text_control = text_control_logger->CreateTextCtrl(this);
            sizer->Add(text_control, wxEXPAND, wxEXPAND | wxALL, 0);
            sizer->Add(control_sizer, 0, wxEXPAND | wxALL, 0);
            wxStaticText * label = new wxStaticText(this, wxID_ANY, _T("Command:"),
                                                    wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
            m_command_entry = new wxComboBox(this, idDebug_LogEntryControl, wxEmptyString,
                                             wxDefaultPosition, wxDefaultSize, 0, nullptr,
                                             wxCB_DROPDOWN | wxTE_PROCESS_ENTER);
            wxBitmap execute_bitmap = wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_EXECUTABLE_FILE")),
                                                               wxART_BUTTON);
            wxBitmap clear_bitmap = wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_DELETE")), wxART_BUTTON);
            wxBitmap file_open_bitmap = wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),
                                                                 wxART_BUTTON);
            wxBitmapButton * button_execute;
            button_execute = new wxBitmapButton(this, idDebug_ExecuteButton, execute_bitmap, wxDefaultPosition,
                                                wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator,
                                                _T("idDebug_ExecuteButton"));
            button_execute->SetToolTip(_("Execute current command"));
            wxBitmapButton * button_load = new wxBitmapButton(this, idDebug_LoadButton, file_open_bitmap, wxDefaultPosition,
                                                              wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator,
                                                              _T("idDebug_LoadButton"));
            button_load->SetDefault();
            button_load->SetToolTip(_("Load from file"));
            wxBitmapButton * button_clear = new wxBitmapButton(this, idDebug_ClearButton, clear_bitmap, wxDefaultPosition,
                                                               wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator,
                                                               _T("idDebug_ClearButton"));
            button_clear->SetDefault();
            button_clear->SetToolTip(_("Clear output window"));
            control_sizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
            control_sizer->Add(m_command_entry, wxEXPAND, wxEXPAND | wxALL, 2);
            control_sizer->Add(button_execute, 0, wxEXPAND | wxALL, 0);
            control_sizer->Add(button_load, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
            control_sizer->Add(button_clear, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
            SetSizer(sizer);
            Connect(idDebug_LogEntryControl,
                    wxEVT_COMMAND_TEXT_ENTER,
                    wxObjectEventFunction(&DebugLogPanel::OnEntryCommand));
            Connect(idDebug_ExecuteButton,
                    wxEVT_COMMAND_BUTTON_CLICKED,
                    wxObjectEventFunction(&DebugLogPanel::OnEntryCommand));
            Connect(idDebug_ClearButton,
                    wxEVT_COMMAND_BUTTON_CLICKED,
                    wxObjectEventFunction(&DebugLogPanel::OnClearLog));
            Connect(idDebug_LoadButton,
                    wxEVT_COMMAND_BUTTON_CLICKED,
                    wxObjectEventFunction(&DebugLogPanel::OnLoadFile));
            // UpdateUI events
            Connect(idDebug_ExecuteButton,
                    wxEVT_UPDATE_UI,
                    wxObjectEventFunction(&DebugLogPanel::OnUpdateUI));
            Connect(idDebug_LoadButton,
                    wxEVT_UPDATE_UI,
                    wxObjectEventFunction(&DebugLogPanel::OnUpdateUI));
            Connect(idDebug_LogEntryControl,
                    wxEVT_UPDATE_UI,
                    wxObjectEventFunction(&DebugLogPanel::OnUpdateUI));
        }

        void OnEntryCommand(cb_unused wxCommandEvent & event)
        {
            assert(m_command_entry);
            wxString cmd = m_command_entry->GetValue();
            cmd.Trim(false);
            cmd.Trim(true);

            if (cmd.IsEmpty())
            {
                return;
            }

            cbDebuggerPlugin * plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();

            if (plugin)
            {
                plugin->SendCommand(cmd, m_debug_log);
                // If it already exists in the list, remove it and add it as the first element of the wxComboBox list
                int index = m_command_entry->FindString(cmd);

                if (index != wxNOT_FOUND)
                {
                    m_command_entry->Delete(index);
                }

                m_command_entry->Insert(cmd, 0);
                m_command_entry->SetValue(wxEmptyString);
            }
        }

        void OnClearLog(cb_unused wxCommandEvent & event)
        {
            assert(m_command_entry);
            assert(m_text_control_logger);
            m_text_control_logger->Clear();
            m_command_entry->SetFocus();
        }

        void OnLoadFile(cb_unused wxCommandEvent & event)
        {
            cbDebuggerPlugin * plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();

            if (!plugin)
            {
                return;
            }

            ConfigManager * manager = Manager::Get()->GetConfigManager(_T("app"));
            wxString path = manager->Read(_T("/file_dialogs/file_run_dbg_script/directory"), wxEmptyString);
            wxFileDialog dialog(this, _("Load script"), path, wxEmptyString,
                                _T("Debugger script files (*.gdb)|*.gdb"), wxFD_OPEN | compatibility::wxHideReadonly);
            PlaceWindow(&dialog);

            if (dialog.ShowModal() == wxID_OK)
            {
                manager->Write(_T("/file_dialogs/file_run_dbg_script/directory"), dialog.GetDirectory());
                plugin->SendCommand(_T("source ") + dialog.GetPath(), m_debug_log);
            }
        }

        void OnUpdateUI(wxUpdateUIEvent & event)
        {
            cbDebuggerPlugin * plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
            event.Enable(plugin && plugin->IsRunning() && plugin->IsStopped());
        }
    private:
        DebugTextCtrlLogger * m_text_control_logger;
        wxComboBox * m_command_entry;
        bool m_debug_log;
};

wxWindow * DebugTextCtrlLogger::CreateControl(wxWindow * parent)
{
    if (!m_panel)
    {
        m_panel = new DebugLogPanel(parent, this, m_debugLog);
    }

    return m_panel;
}

template<> DebuggerManager * Mgr<DebuggerManager>::instance = nullptr;
template<> bool  Mgr<DebuggerManager>::isShutdown = false;

inline void ReadActiveDebuggerConfig(wxString & name, int & configIndex)
{
    ConfigManager & config = *Manager::Get()->GetConfigManager(_T("debugger_common"));
    name = config.Read("active_debugger", wxEmptyString);

    if (name.empty())
    {
        configIndex = -1;
    }
    else
    {
        configIndex = std::max(0, config.ReadInt("active_debugger_config", 0));
    }
}

inline void WriteActiveDebuggerConfig(const wxString & name, int configIndex)
{
    ConfigManager & configMgr = *Manager::Get()->GetConfigManager(_T("debugger_common"));
    configMgr.Write("active_debugger", name);
    configMgr.Write("active_debugger_config", configIndex);
}

cbDebuggerConfiguration * DebuggerManager::PluginData::GetConfiguration(int index)
{
    if (m_configurations.empty())
    {
        cbAssert(false);
    }

    if (index >= static_cast<int>(m_configurations.size()))
    {
        return nullptr;
    }
    else
    {
        return m_configurations[index];
    }
}

DebuggerManager::DebuggerManager() :
    m_interfaceFactory(nullptr),
    m_activeDebugger(nullptr),
    m_menuHandler(nullptr),
    m_backtraceDialog(nullptr),
    m_breakPointsDialog(nullptr),
    m_cpuRegistersDialog(nullptr),
    m_disassemblyDialog(nullptr),
    m_examineMemoryDialog(nullptr),
    m_threadsDialog(nullptr),
    m_watchesDialog(nullptr),
    m_logger(nullptr),
    m_loggerIndex(-1),
    m_isDisassemblyMixedMode(false),
    m_useTargetsDefault(false)
{
    typedef cbEventFunctor<DebuggerManager, CodeBlocksEvent> Event;
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE,        new Event(this, &DebuggerManager::OnProjectActivated));
    // connect with cbEVT_PROJECT_OPEN, too (see here: http://forums.codeblocks.org/index.php/topic,17260.msg118431.html#msg118431)
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_OPEN,            new Event(this, &DebuggerManager::OnProjectActivated));
    Manager::Get()->RegisterEventSink(cbEVT_BUILDTARGET_SELECTED,    new Event(this, &DebuggerManager::OnTargetSelected));
    Manager::Get()->RegisterEventSink(cbEVT_SETTINGS_CHANGED,        new Event(this, &DebuggerManager::OnSettingsChanged));
    Manager::Get()->RegisterEventSink(cbEVT_PLUGIN_LOADING_COMPLETE, new Event(this, &DebuggerManager::OnPluginLoadingComplete));
    wxString activeDebuggerName;
    int activeConfig;
    ReadActiveDebuggerConfig(activeDebuggerName, activeConfig);

    if (activeDebuggerName.empty() && activeConfig == -1)
    {
        m_useTargetsDefault = true;
    }

    ConfigManager * c = Manager::Get()->GetConfigManager("debugger_common");
    m_isDisassemblyMixedMode = c->ReadBool("/common/disassembly/mixed_mode", false);
}

DebuggerManager::~DebuggerManager()
{
    for (RegisteredPlugins::iterator it = m_registered.begin(); it != m_registered.end(); ++it)
    {
        it->second.ClearConfigurations();
    }

    Manager::Get()->RemoveAllEventSinksFor(this);
    delete m_interfaceFactory;
}

bool DebuggerManager::RegisterDebugger(cbDebuggerPlugin * plugin)
{
    RegisteredPlugins::iterator it = m_registered.find(plugin);

    if (it != m_registered.end())
    {
        return false;
    }

    const wxString & guiName = plugin->GetGUIName();
    const wxString & settingsName = plugin->GetSettingsName();
    wxRegEx regExSettingsName("^[a-z_][a-z0-9_]+$");

    if (!regExSettingsName.Matches(settingsName))
    {
        wxString s;
        s = wxString::Format(_("The settings name for the debugger plugin \"%s\" - \"%s\" contains invalid characters"),
                             guiName.c_str(), settingsName.c_str());
        Manager::Get()->GetLogManager()->LogError(s);
        return false;
    }

    int normalIndex = -1;
    GetLogger(normalIndex);
    plugin->SetupLog(normalIndex);
    PluginData data;
    m_registered[plugin] = data;
    it = m_registered.find(plugin);
    ProcessSettings(it);
    // There should be at least one configuration for every plugin.
    // If this is not the case, something is wrong and should be fixed.
    cbAssert(!it->second.GetConfigurations().empty());
    wxString activeDebuggerName;
    int activeConfig;
    ReadActiveDebuggerConfig(activeDebuggerName, activeConfig);

    if (activeDebuggerName == settingsName)
    {
        if (activeConfig > static_cast<int>(it->second.GetConfigurations().size()))
        {
            activeConfig = 0;
        }

        m_activeDebugger = plugin;
        m_activeDebugger->SetActiveConfig(activeConfig);
        m_menuHandler->SetActiveDebugger(m_activeDebugger);
    }

    CreateWindows();
    m_menuHandler->RebuildMenus();
    return true;
}

bool DebuggerManager::UnregisterDebugger(cbDebuggerPlugin * plugin)
{
    RegisteredPlugins::iterator it = m_registered.find(plugin);

    if (it == m_registered.end())
    {
        return false;
    }

    it->second.ClearConfigurations();
    m_registered.erase(it);

    if (plugin == m_activeDebugger)
    {
        if (m_registered.empty())
        {
            m_activeDebugger = nullptr;
        }
        else
        {
            m_activeDebugger = m_registered.begin()->first;
        }

        m_menuHandler->SetActiveDebugger(m_activeDebugger);
    }

    if (!Manager::IsAppShuttingDown())
    {
        m_menuHandler->RebuildMenus();
        RefreshUI();
    }

    if (m_registered.empty())
    {
        DestroyWindows();

        if (Manager::Get()->GetLogManager())
        {
            Manager::Get()->GetDebuggerManager()->HideLogger();
        }
    }

    return true;
}

void DebuggerManager::ProcessSettings(RegisteredPlugins::iterator it)
{
    cbDebuggerPlugin * plugin = it->first;
    PluginData & data = it->second;
    ConfigManager * config = Manager::Get()->GetConfigManager("debugger_common");
    wxString path = "/sets/" + plugin->GetSettingsName();
    wxArrayString configs = config->EnumerateSubPaths(path);
    configs.Sort();

    if (configs.empty())
    {
        config->Write(path + "/conf1/name", wxString("Default"));
        configs = config->EnumerateSubPaths(path);
        configs.Sort();
    }

    data.ClearConfigurations();
    data.m_lastConfigID = -1;

    for (size_t jj = 0; jj < configs.Count(); ++jj)
    {
        wxString configPath = path + "/" + configs[jj];
        wxString name = config->Read(configPath + "/name");
        cbDebuggerConfiguration * pluginConfig;
        pluginConfig = plugin->LoadConfig(ConfigManagerWrapper("debugger_common", configPath + "/values"));

        if (pluginConfig)
        {
            pluginConfig->SetName(name);
            data.GetConfigurations().push_back(pluginConfig);
        }
    }
}

ConfigManagerWrapper DebuggerManager::NewConfig(cbDebuggerPlugin * plugin, cb_unused const wxString & name)
{
    RegisteredPlugins::iterator it = m_registered.find(plugin);

    if (it == m_registered.end())
    {
        return ConfigManagerWrapper();
    }

    wxString path = "/sets/" + it->first->GetSettingsName();

    if (it->second.m_lastConfigID == -1)
    {
        ConfigManager * config = Manager::Get()->GetConfigManager("debugger_common");
        wxArrayString configs = config->EnumerateSubPaths(path);

        for (size_t ii = 0; ii < configs.GetCount(); ++ii)
        {
            long id;

            if (configs[ii].Remove(0, 4).ToLong(&id))
            {
                it->second.m_lastConfigID = std::max<long>(it->second.m_lastConfigID, id);
            }
        }
    }

    path << "/conf" << ++it->second.m_lastConfigID;
    return ConfigManagerWrapper("debugger_common", path +  "/values");
}

void DebuggerManager::RebuildAllConfigs()
{
    for (RegisteredPlugins::iterator it = m_registered.begin(); it != m_registered.end(); ++it)
    {
        ProcessSettings(it);
    }

    m_menuHandler->RebuildMenus();
}

wxMenu * DebuggerManager::GetMenu()
{
    wxMenuBar * menuBar = Manager::Get()->GetAppFrame()->GetMenuBar();
    cbAssert(menuBar);
    wxMenu * menu = nullptr;
    int menu_pos = menuBar->FindMenu(_("&Debug"));

    if (menu_pos != wxNOT_FOUND)
    {
        menu = menuBar->GetMenu(menu_pos);
    }

    if (!menu)
    {
        menu = Manager::Get()->LoadMenu(_T("debugger_menu"), true);
        // ok, now, where do we insert?
        // three possibilities here:
        // a) locate "Compile" menu and insert after it
        // b) locate "Project" menu and insert after it
        // c) if not found (?), insert at pos 5
        int finalPos = 5;
        int projcompMenuPos = menuBar->FindMenu(_("&Build"));

        if (projcompMenuPos == wxNOT_FOUND)
        {
            projcompMenuPos = menuBar->FindMenu(_("&Compile"));
        }

        if (projcompMenuPos != wxNOT_FOUND)
        {
            finalPos = projcompMenuPos + 1;
        }
        else
        {
            projcompMenuPos = menuBar->FindMenu(_("&Project"));

            if (projcompMenuPos != wxNOT_FOUND)
            {
                finalPos = projcompMenuPos + 1;
            }
        }

        menuBar->Insert(finalPos, menu, _("&Debug"));
        m_menuHandler->RebuildMenus();
    }

    return menu;
}

bool DebuggerManager::HasMenu() const
{
    wxMenuBar * menuBar = Manager::Get()->GetAppFrame()->GetMenuBar();
    cbAssert(menuBar);
    int menu_pos = menuBar->FindMenu(_("&Debug"));
    return menu_pos != wxNOT_FOUND;
}

void DebuggerManager::BuildContextMenu(wxMenu & menu, const wxString & word_at_caret, bool is_running)
{
    m_menuHandler->BuildContextMenu(menu, word_at_caret, is_running);
}

TextCtrlLogger * DebuggerManager::GetLogger(int & index)
{
    LogManager * msgMan = Manager::Get()->GetLogManager();

    if (!m_logger)
    {
        m_logger = new DebugTextCtrlLogger(true, false);
        m_loggerIndex = msgMan->SetLog(m_logger);
        LogSlot & slot = msgMan->Slot(m_loggerIndex);
        slot.title = _("Debugger");
        // set log image
        const int uiSize = Manager::Get()->GetImageSize(Manager::UIComponent::InfoPaneNotebooks);
        wxString prefix(ConfigManager::GetDataFolder() + "/resources.zip#zip:/images/infopane/");
#if wxCHECK_VERSION(3, 1, 6)
        slot.icon = new wxBitmapBundle(cbLoadBitmapBundle(prefix, "misc.png", uiSize, wxBITMAP_TYPE_PNG));
#else
        const int uiScaleFactor = Manager::Get()->GetUIScaleFactor(Manager::UIComponent::InfoPaneNotebooks);
        prefix << wxString::Format("%dx%d/", uiSize, uiSize);
        slot.icon = new wxBitmap(cbLoadBitmapScaled(prefix + "misc.png", wxBITMAP_TYPE_PNG, uiScaleFactor));
#endif
        CodeBlocksLogEvent evtAdd(cbEVT_ADD_LOG_WINDOW, m_logger, slot.title, slot.icon);
        Manager::Get()->ProcessEvent(evtAdd);
    }

    index = m_loggerIndex;
    return m_logger;
}

TextCtrlLogger * DebuggerManager::GetLogger()
{
    int index;
    return GetLogger(index);
}

void DebuggerManager::HideLogger()
{
    LogManager * logManager = Manager::Get()->GetLogManager();

    if (logManager)
    {
        // TODO: This is wrong. We need some automatic way for this to happen!!!
        LogSlot & slot = logManager->Slot(m_loggerIndex);
        delete slot.icon;
        slot.icon = nullptr;
    }

    CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_logger);
    Manager::Get()->ProcessEvent(evt);
    m_logger = nullptr;
    m_loggerIndex = -1;
}

void DebuggerManager::SetInterfaceFactory(cbDebugInterfaceFactory * factory)
{
    cbAssert(!m_interfaceFactory);
    m_interfaceFactory = factory;
    CreateWindows();
    m_backtraceDialog->EnableWindow(false);
    m_cpuRegistersDialog->EnableWindow(false);
    m_disassemblyDialog->EnableWindow(false);
    m_examineMemoryDialog->EnableWindow(false);
    m_threadsDialog->EnableWindow(false);
}

void DebuggerManager::CreateWindows()
{
    if (!m_backtraceDialog)
    {
        m_backtraceDialog = m_interfaceFactory->CreateBacktrace();
    }

    if (!m_breakPointsDialog)
    {
        m_breakPointsDialog = m_interfaceFactory->CreateBreapoints();
    }

    if (!m_cpuRegistersDialog)
    {
        m_cpuRegistersDialog = m_interfaceFactory->CreateCPURegisters();
    }

    if (!m_disassemblyDialog)
    {
        m_disassemblyDialog = m_interfaceFactory->CreateDisassembly();
    }

    if (!m_examineMemoryDialog)
    {
        m_examineMemoryDialog = m_interfaceFactory->CreateMemory();
    }

    if (!m_threadsDialog)
    {
        m_threadsDialog = m_interfaceFactory->CreateThreads();
    }

    if (!m_watchesDialog)
    {
        m_watchesDialog = m_interfaceFactory->CreateWatches();
    }
}

void DebuggerManager::DestroyWindows()
{
    m_interfaceFactory->DeleteBacktrace(m_backtraceDialog);
    m_backtraceDialog = nullptr;
    m_interfaceFactory->DeleteBreakpoints(m_breakPointsDialog);
    m_breakPointsDialog = nullptr;
    m_interfaceFactory->DeleteCPURegisters(m_cpuRegistersDialog);
    m_cpuRegistersDialog = nullptr;
    m_interfaceFactory->DeleteDisassembly(m_disassemblyDialog);
    m_disassemblyDialog = nullptr;
    m_interfaceFactory->DeleteMemory(m_examineMemoryDialog);
    m_examineMemoryDialog = nullptr;
    m_interfaceFactory->DeleteThreads(m_threadsDialog);
    m_threadsDialog = nullptr;
    m_interfaceFactory->DeleteWatches(m_watchesDialog);
    m_watchesDialog = nullptr;
}

cbDebugInterfaceFactory * DebuggerManager::GetInterfaceFactory()
{
    return m_interfaceFactory;
}

void DebuggerManager::SetMenuHandler(cbDebuggerMenuHandler * handler)
{
    m_menuHandler = handler;
}

cbDebuggerMenuHandler * DebuggerManager::GetMenuHandler()
{
    return m_menuHandler;
}

cbBacktraceDlg * DebuggerManager::GetBacktraceDialog()
{
    return m_backtraceDialog;
}

cbBreakpointsDlg * DebuggerManager::GetBreakpointDialog()
{
    return m_breakPointsDialog;
}

cbCPURegistersDlg * DebuggerManager::GetCPURegistersDialog()
{
    return m_cpuRegistersDialog;
}

cbDisassemblyDlg * DebuggerManager::GetDisassemblyDialog()
{
    return m_disassemblyDialog;
}

cbExamineMemoryDlg * DebuggerManager::GetExamineMemoryDialog()
{
    return m_examineMemoryDialog;
}

cbThreadsDlg * DebuggerManager::GetThreadsDialog()
{
    return m_threadsDialog;
}

cbWatchesDlg * DebuggerManager::GetWatchesDialog()
{
    return m_watchesDialog;
}

bool DebuggerManager::ShowBacktraceDialog()
{
    cbBacktraceDlg * dialog = GetBacktraceDialog();

    if (!IsWindowReallyShown(dialog->GetWindow()))
    {
        // show the backtrace window
        CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
        evt.pWindow = dialog->GetWindow();
        Manager::Get()->ProcessEvent(evt);
        return true;
    }
    else
    {
        return false;
    }
}

bool DebuggerManager::UpdateBacktrace()
{
    return m_backtraceDialog && IsWindowReallyShown(m_backtraceDialog->GetWindow());
}

bool DebuggerManager::UpdateCPURegisters()
{
    return m_cpuRegistersDialog && IsWindowReallyShown(m_cpuRegistersDialog->GetWindow());
}

bool DebuggerManager::UpdateDisassembly()
{
    return m_disassemblyDialog && IsWindowReallyShown(m_disassemblyDialog->GetWindow());
}

bool DebuggerManager::UpdateExamineMemory()
{
    return m_examineMemoryDialog && IsWindowReallyShown(m_examineMemoryDialog->GetWindow());
}

bool DebuggerManager::UpdateThreads()
{
    return m_threadsDialog && IsWindowReallyShown(m_threadsDialog->GetWindow());
}

cbDebuggerPlugin * DebuggerManager::GetDebuggerHavingWatch(cb::shared_ptr<cbWatch> watch)
{
    watch = cbGetRootWatch(watch);

    for (RegisteredPlugins::iterator it = m_registered.begin(); it != m_registered.end(); ++it)
    {
        if (it->first->HasWatch(watch))
        {
            return it->first;
        }
    }

    return nullptr;
}

bool DebuggerManager::ShowValueTooltip(const cb::shared_ptr<cbWatch> & watch, const wxRect & rect)
{
    return m_interfaceFactory->ShowValueTooltip(watch, rect);
}

DebuggerManager::RegisteredPlugins const & DebuggerManager::GetAllDebuggers() const
{
    return m_registered;
}
DebuggerManager::RegisteredPlugins & DebuggerManager::GetAllDebuggers()
{
    return m_registered;
}
cbDebuggerPlugin * DebuggerManager::GetActiveDebugger()
{
    return m_activeDebugger;
}

inline void RefreshBreakpoints(cb_unused const cbDebuggerPlugin * plugin)
{
    EditorManager * editorManager = Manager::Get()->GetEditorManager();
    int count = editorManager->GetEditorsCount();

    for (int ii = 0; ii < count; ++ii)
    {
        EditorBase * editor = editorManager->GetEditor(ii);

        if (editor->IsBuiltinEditor())
        {
            static_cast<cbEditor *>(editor)->RefreshBreakpointMarkers();
        }
    }
}

void DebuggerManager::SetActiveDebugger(cbDebuggerPlugin * activeDebugger, ConfigurationVector::const_iterator config)
{
    RegisteredPlugins::const_iterator it = m_registered.find(activeDebugger);
    cbAssert(it != m_registered.end());
    m_useTargetsDefault = false;
    m_activeDebugger = activeDebugger;
    int index = std::distance(it->second.GetConfigurations().begin(), config);
    m_activeDebugger->SetActiveConfig(index);
    WriteActiveDebuggerConfig(it->first->GetSettingsName(), index);
    RefreshUI();
}

void DebuggerManager::RefreshUI()
{
    m_menuHandler->SetActiveDebugger(m_activeDebugger);
    m_menuHandler->RebuildMenus();
    RefreshBreakpoints(m_activeDebugger);

    if (m_activeDebugger)
    {
        if (m_backtraceDialog)
        {
            m_backtraceDialog->EnableWindow(m_activeDebugger->SupportsFeature(cbDebuggerFeature::Callstack));
        }

        if (m_cpuRegistersDialog)
        {
            m_cpuRegistersDialog->EnableWindow(m_activeDebugger->SupportsFeature(cbDebuggerFeature::CPURegisters));
        }

        if (m_disassemblyDialog)
        {
            m_disassemblyDialog->EnableWindow(m_activeDebugger->SupportsFeature(cbDebuggerFeature::Disassembly));
        }

        if (m_examineMemoryDialog)
        {
            m_examineMemoryDialog->EnableWindow(m_activeDebugger->SupportsFeature(cbDebuggerFeature::ExamineMemory));
        }

        if (m_threadsDialog)
        {
            m_threadsDialog->EnableWindow(m_activeDebugger->SupportsFeature(cbDebuggerFeature::Threads));
        }
    }

    if (m_watchesDialog)
    {
        m_watchesDialog->RefreshUI();
    }

    if (m_breakPointsDialog)
    {
        m_breakPointsDialog->Reload();
    }
}

bool DebuggerManager::IsActiveDebuggerTargetsDefault() const
{
    return m_activeDebugger && m_useTargetsDefault;
}

void DebuggerManager::SetTargetsDefaultAsActiveDebugger()
{
    m_activeDebugger = nullptr;
    m_menuHandler->SetActiveDebugger(nullptr);
    FindTargetsDebugger();
}

void DebuggerManager::FindTargetsDebugger()
{
    if (Manager::Get()->GetProjectManager()->IsLoadingOrClosing())
    {
        return;
    }

    m_activeDebugger = nullptr;
    m_menuHandler->SetActiveDebugger(nullptr);

    if (m_registered.empty())
    {
        m_menuHandler->MarkActiveTargetAsValid(false);
        return;
    }

    ProjectManager * projectMgr = Manager::Get()->GetProjectManager();
    LogManager * log = Manager::Get()->GetLogManager();
    cbProject * project = projectMgr->GetActiveProject();
    ProjectBuildTarget * target = nullptr;

    if (project)
    {
        const wxString & targetName = project->GetActiveBuildTarget();

        if (project->BuildTargetValid(targetName))
        {
            target = project->GetBuildTarget(targetName);
        }
    }

    Compiler * compiler = nullptr;

    if (!target)
    {
        if (project)
        {
            compiler = CompilerFactory::GetCompiler(project->GetCompilerID());
        }

        if (!compiler)
        {
            compiler = CompilerFactory::GetDefaultCompiler();
        }

        if (!compiler)
        {
            log->LogError(_("Can't get the compiler for the active target, nor the project, nor the default one!"));
            m_menuHandler->MarkActiveTargetAsValid(false);
            return;
        }
    }
    else
    {
        compiler = CompilerFactory::GetCompiler(target->GetCompilerID());

        if (!compiler)
        {
            log->LogError(wxString::Format(_("Current target '%s' doesn't have valid compiler!"),
                                           target->GetTitle().c_str()));
            m_menuHandler->MarkActiveTargetAsValid(false);
            return;
        }
    }

    wxString dbgString = compiler->GetPrograms().DBGconfig;
    wxString::size_type pos = dbgString.find(':');
    wxString name, config;

    if (pos != wxString::npos)
    {
        name = dbgString.substr(0, pos);
        config = dbgString.substr(pos + 1, dbgString.length() - pos - 1);
    }

    if (name.empty() || config.empty())
    {
        if (compiler->GetID() != "null")
        {
            log->LogError(wxString::Format(_("Current compiler '%s' doesn't have correctly defined debugger!"),
                                           compiler->GetName().c_str()));
        }

        m_menuHandler->MarkActiveTargetAsValid(false);
        return;
    }

    for (RegisteredPlugins::iterator it = m_registered.begin(); it != m_registered.end(); ++it)
    {
        PluginData & data = it->second;

        if (it->first->GetSettingsName() == name)
        {
            ConfigurationVector & configs = data.GetConfigurations();
            int index = 0;

            for (ConfigurationVector::iterator itConf = configs.begin(); itConf != configs.end(); ++itConf, ++index)
            {
                if ((*itConf)->GetName() == config)
                {
                    m_activeDebugger = it->first;
                    m_activeDebugger->SetActiveConfig(index);
                    m_useTargetsDefault = true;
                    WriteActiveDebuggerConfig(wxEmptyString, -1);
                    RefreshUI();
                    m_menuHandler->MarkActiveTargetAsValid(true);
                    return;
                }
            }
        }
    }

    if (target)
    {
        log->LogError(wxString::Format(_("Can't find the debugger config: '%s' for the current target '%s'!"),
                                       dbgString, target->GetTitle()));
    }
    else
    {
        log->LogError(wxString::Format(_("Can't find the debugger config: '%s' for the compiler '%s'!"),
                                       dbgString, compiler->GetName()));
    }

    m_menuHandler->MarkActiveTargetAsValid(false);
}

bool DebuggerManager::IsDisassemblyMixedMode()
{
    return m_isDisassemblyMixedMode;
}

void DebuggerManager::SetDisassemblyMixedMode(bool mixed)
{
    m_isDisassemblyMixedMode = mixed;
    ConfigManager * c = Manager::Get()->GetConfigManager("debugger_common");
    c->Write("/common/disassembly/mixed_mode", m_isDisassemblyMixedMode);
}

void DebuggerManager::OnProjectActivated(cb_unused CodeBlocksEvent & event)
{
    if (m_useTargetsDefault)
    {
        FindTargetsDebugger();
    }
}

void DebuggerManager::OnTargetSelected(cb_unused CodeBlocksEvent & event)
{
    if (m_useTargetsDefault)
    {
        FindTargetsDebugger();
    }
}

void DebuggerManager::OnSettingsChanged(CodeBlocksEvent & event)
{
    const int value = event.GetInt();

    if (value < int(cbSettingsType::First) || value >= int(cbSettingsType::Last))
    {
        return;
    }

    const cbSettingsType settingType = cbSettingsType(value);

    if (settingType == cbSettingsType::Compiler || settingType == cbSettingsType::Debugger
            || settingType == cbSettingsType::BuildOptions)
    {
        if (m_useTargetsDefault)
        {
            FindTargetsDebugger();
        }
    }
}

void DebuggerManager::OnPluginLoadingComplete(cb_unused CodeBlocksEvent & event)
{
    RefreshUI();

    if (!m_activeDebugger)
    {
        m_useTargetsDefault = true;
        FindTargetsDebugger();
    }
}

void DebuggerManager::SaveDebuggerConfigOptions(CompilerDebuggerOptions & cdoConfiguation)
{
    ConfigManager * config = Manager::Get()->GetConfigManager("debugger_common");
    int offset = cdoConfiguation.debuggerConfigurationName.Find(':');
    wxString pluginName = "NoPlugin";
    wxString debugConfigName = "";

    if (offset == wxNOT_FOUND)
    {
        if (!cdoConfiguation.debuggerConfigurationName.IsEmpty())
        {
            pluginName = cdoConfiguation.debuggerConfigurationName;
        }

        debugConfigName = cdoConfiguation.compilerIDName;
    }
    else
    {
        pluginName = cdoConfiguation.debuggerConfigurationName.Left(offset);
        debugConfigName = cdoConfiguation.debuggerConfigurationName.Mid(offset + 1);
    }

    wxString setPath = wxString::Format("/sets/%s", pluginName);
    wxArrayString configs = config->EnumerateSubPaths(setPath);
    configs.Sort();
    bool foundConfigName = false;

    for (size_t jj = 0; jj < configs.Count(); ++jj)
    {
        wxString name = config->Read(wxString::Format("%s/%s/name", setPath, configs[jj]));;

        if (
            name.IsSameAs(cdoConfiguation.compilerIDName) ||
            name.IsSameAs(debugConfigName)
        )
        {
            foundConfigName = true;
        }
    }

    if (foundConfigName == false)
    {
        // LogManager *pLogMgr = Manager::Get()->GetLogManager();
        wxString pathDebuggerEntry = wxString::Format("/sets/%s/conf%lu", pluginName, (long)configs.Count() + 1);
        config->Write(wxString::Format("%s/name", pathDebuggerEntry), debugConfigName);
        // pLogMgr->DebugLog(wxString::Format("DebuggerManager::SaveDebuggerConfigOptions compilerIDName: %s (Line %d)", cdoConfiguation.compilerIDName, __LINE__));
        wxString tryTmpPath = UnixFilename(cdoConfiguation.executablePath, wxPATH_NATIVE);

        if (cdoConfiguation.compilerMasterPath.IsEmpty() || wxFileExists(tryTmpPath))
        {
            cdoConfiguation.executablePath = tryTmpPath;
            // pLogMgr->DebugLog(wxString::Format("DebuggerManager::SaveDebuggerConfigOptions found debugger: %s (Line %d)", cdoConfiguation.executablePath, __LINE__));
        }
        else
        {
            // pLogMgr->DebugLog(wxString::Format("Line %d", __LINE__));
            tryTmpPath = SearchForDebuggerExecutable(cdoConfiguation.compilerMasterPath, cdoConfiguation.executablePath);

            if (!tryTmpPath.IsEmpty())
            {
                cdoConfiguation.executablePath = tryTmpPath;
                // pLogMgr->DebugLog(wxString::Format("DebuggerManager::SaveDebuggerConfigOptions found debugger: %s (Line %d)", cdoConfiguation.executablePath, __LINE__));
            }
            else
            {
                tryTmpPath = cbDetectDebuggerExecutable(cdoConfiguation.compilerIDName, cdoConfiguation.executablePath);

                if (!tryTmpPath.IsEmpty())
                {
                    cdoConfiguation.executablePath = tryTmpPath;
                    // pLogMgr->DebugLog(wxString::Format("DebuggerManager::SaveDebuggerConfigOptions found debugger: %s (Line %d)", cdoConfiguation.executablePath, __LINE__));
                }

                // else
                // {
                //     pLogMgr->DebugLog(wxString::Format("DebuggerManager::SaveDebuggerConfigOptions using debugger: %s (Line %d)", cdoConfiguation.executablePath, __LINE__));
                // }
            }
        }

        config->Write(pathDebuggerEntry + "/values/executable_path",        cdoConfiguation.executablePath);
        config->Write(pathDebuggerEntry + "/values/user_arguments",         cdoConfiguation.userArguments);
        config->Write(pathDebuggerEntry + "/values/type",                   cdoConfiguation.type);
        config->Write(pathDebuggerEntry + "/values/init_commands",          cdoConfiguation.initCommands);
        config->Write(pathDebuggerEntry + "/values/disable_init",           cdoConfiguation.disableInit ? "true" : "false");
        config->Write(pathDebuggerEntry + "/values/watch_args",             cdoConfiguation.watchArgs ? "true" : "false");
        config->Write(pathDebuggerEntry + "/values/watch_locals",           cdoConfiguation.watchLocals ? "true" : "false");
        config->Write(pathDebuggerEntry + "/values/catch_exceptions",       cdoConfiguation.catchExceptions ? "true" : "false");
        config->Write(pathDebuggerEntry + "/values/eval_tooltip",           cdoConfiguation.evalExpressionAsTooltip ? "true" : "false");
        config->Write(pathDebuggerEntry + "/values/add_other_search_dirs",  cdoConfiguation.addOtherSearchDirs ? "true" : "false");
        config->Write(pathDebuggerEntry + "/values/do_not_run",             cdoConfiguation.doNoRunDebuggee ? "true" : "false");
        config->Write(pathDebuggerEntry + "/values/persist_debug_elements", cdoConfiguation.persistDebugElements ? "true" : "false");
        config->Write(pathDebuggerEntry + "/values/disassembly_flavor",     cdoConfiguation.disassemblyFlavor);
        config->Write(pathDebuggerEntry + "/values/instruction_set",        cdoConfiguation.instructionSet);
        config->Write(pathDebuggerEntry + "/values/portNumber",             cdoConfiguation.portNumber);
        config->Flush();
    }

    Manager::Get()->GetLogManager()->Log("CompilerDebuggerOptions:");
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 debuggerConfigurationName %s"), cdoConfiguation.debuggerConfigurationName));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 compilerIDName %s"), cdoConfiguation.compilerIDName));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 executablePath %s"), cdoConfiguation.executablePath));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 userArguments %s"), cdoConfiguation.userArguments));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 type %s"), cdoConfiguation.type));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 initCommands %s"), cdoConfiguation.initCommands));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 disableInit %s"), cdoConfiguation.disableInit ? "true" : "false"));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 watchArgs %s"), cdoConfiguation.watchArgs ? "true" : "false"));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 watchLocals %s"), cdoConfiguation.watchLocals ? "true" : "false"));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 catchExceptions %s"), cdoConfiguation.catchExceptions ? "true" : "false"));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 evalExpressionAsTooltip %s"), cdoConfiguation.evalExpressionAsTooltip ? "true" : "false"));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 addOtherSearchDirs %s"), cdoConfiguation.addOtherSearchDirs ? "true" : "false"));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 doNoRunDebuggee %s"), cdoConfiguation.doNoRunDebuggee ? "true" : "false"));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 persistDebugElements %s"), cdoConfiguation.persistDebugElements ? "true" : "false"));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 disassemblyFlavor %s"), cdoConfiguation.disassemblyFlavor));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 instructionSet %s"), cdoConfiguation.instructionSet));
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("                 portNumber %s"), cdoConfiguation.portNumber));
}
