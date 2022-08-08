// System include files
#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/filefn.h>

// CB include files
#include <sdk.h> // Code::Blocks SDK
#include "cbworkspace.h"
#include "filemanager.h"
#include "logmanager.h"
#include "manager.h"
#include "macrosmanager.h"
#include "uservarmanager.h"
#include "tinyxml.h"

// ProjectExporter include files
#include "CMakeListsExporter.h"

CMakeListsExporter::CMakeListsExporter()
{
    // Does not support C::B ${variable}. Supports $(variable)
    m_RE_Unix.Compile("([^$]|^)(\\$[(]?(#?[A-Za-z_0-9.]+)[\\) /\\\\]?)", wxRE_EXTENDED | wxRE_NEWLINE);
    // MSDOS %VARIABLE%
    m_RE_DOS.Compile("([^%]|^)(%(#?[A-Za-z_0-9.]+)%)", wxRE_EXTENDED | wxRE_NEWLINE);
}

CMakeListsExporter::~CMakeListsExporter()
{
}

void CMakeListsExporter::ExpandMacros(wxString & buffer)
{
    if (buffer.IsEmpty())
    {
        return;
    }

    if (buffer.IsSameAs("\"\"") || buffer.IsSameAs("''"))
    {
        buffer.Clear();
        return;
    }

    static const wxString delim("$%[");

    if (buffer.find_first_of(delim) != wxString::npos)
    {
        // See macromanager.cpp MacrosManager::ReplaceMacros function for where this code was taken from
        static const wxString const_COIN("COIN");
        static const wxString const_RANDOM("RANDOM");
        static const wxString toNativePath("$TO_NATIVE_PATH{");
        static const wxString toUnixPath("$TO_UNIX_PATH{");
        static const wxString toWindowsPath("$TO_WINDOWS_PATH{");
        wxString search;
        wxString replace;
        UserVariableManager * pUsrVarMan = Manager::Get()->GetUserVariableManager();
        MacrosManager * macroMan = Manager::Get()->GetMacrosManager();
        const MacrosMap & Macros = macroMan->GetMacros();

        while (m_RE_Unix.Matches(buffer))
        {
            replace.Empty();
            search = m_RE_Unix.GetMatch(buffer, 2);
            wxString var = m_RE_Unix.GetMatch(buffer, 3).Upper();

            if (var.GetChar(0) == '#')
            {
                replace = UnixFilename(pUsrVarMan->Replace(var));
            }
            else
            {
                if (var.compare(const_COIN) == 0)
                {
                    replace.assign(1u, (rand() & 1) ? '1' : '0');
                }
                else
                    if (var.compare(const_RANDOM) == 0)
                    {
                        replace = wxString::Format("%d", rand() & 0xffff);
                    }
                    else
                    {
                        if (!Macros.empty())
                        {
                            MacrosMap::const_iterator it;

                            if ((it = Macros.find(var)) != Macros.end())
                            {
                                replace = it->second;
                            }
                        }
                    }
            }

            const wxChar l = search.Last(); // make non-braced variables work

            if (l == '/' || l == '\\' || l == '$' || l == ' ')
            {
                replace.append(l);
            }

            if (replace.IsEmpty())
            {
                wxGetEnv(var, &replace);
            }

            if (replace.IsSameAs("\"\"") || replace.IsSameAs("''") || replace.empty())
            {
                buffer.Replace(search, "", false);
            }
            else
            {
                buffer.Replace(search, replace, false);
            }
        }

        while (m_RE_DOS.Matches(buffer))
        {
            replace.Empty();
            search = m_RE_DOS.GetMatch(buffer, 2);
            wxString var = m_RE_DOS.GetMatch(buffer, 3).Upper();

            if (var.GetChar(0) == '#')
            {
                replace = UnixFilename(pUsrVarMan->Replace(var));
            }
            else
            {
                if (var.compare(const_COIN) == 0)
                {
                    replace.assign(1u, (rand() & 1) ? '1' : '0');
                }
                else
                    if (var.compare(const_RANDOM) == 0)
                    {
                        replace = wxString::Format("%d", rand() & 0xffff);
                    }
                    else
                    {
                        if (!Macros.empty())
                        {
                            MacrosMap::const_iterator it;

                            if ((it = Macros.find(var)) != Macros.end())
                            {
                                replace = it->second;
                            }
                        }
                    }
            }

            if (replace.IsEmpty())
            {
                wxGetEnv(var, &replace);
            }

            if (replace.IsSameAs("\"\"") || replace.IsSameAs("''") || replace.empty())
            {
                buffer.Replace(search, "", false);
            }
            else
            {
                buffer.Replace(search, replace, false);
            }
        }
    }

    buffer.Replace("%%", "%");
    buffer.Replace("$$", "$");
}

void CMakeListsExporter::ConvertMacros(wxString & buffer)
{
    if (buffer.IsEmpty())
    {
        return;
    }

    if (buffer.IsSameAs("\"\"") || buffer.IsSameAs("''"))
    {
        buffer.Clear();
        return;
    }

    static const wxString delim("$%[");

    if (buffer.find_first_of(delim) != wxString::npos)
    {
        // See macromanager.cpp MacrosManager::ReplaceMacros function for where this code was taken from
        static const wxString const_COIN("COIN");
        static const wxString const_RANDOM("RANDOM");
        static const wxString toNativePath("$TO_NATIVE_PATH{");
        static const wxString toUnixPath("$TO_UNIX_PATH{");
        static const wxString toWindowsPath("$TO_WINDOWS_PATH{");
        wxString search;
        wxString replace;
        // UserVariableManager* pUsrVarMan = Manager::Get()->GetUserVariableManager();
        MacrosManager * macroMan = Manager::Get()->GetMacrosManager();
        const MacrosMap & Macros = macroMan->GetMacros();

        while (m_RE_Unix.Matches(buffer))
        {
            replace.Empty();
            search = m_RE_Unix.GetMatch(buffer, 2);
            wxString var = m_RE_Unix.GetMatch(buffer, 3).Upper();

            if (var.GetChar(0) == '#')
            {
                replace = var.Upper();

                if (replace.Find('.') == wxNOT_FOUND)
                {
                    replace.Append(".BASE");
                }

                replace.Prepend("${GCV_");
                replace.Replace("#", "");
                replace.Replace(".", "_");
                replace.Append("}");
            }
            else
            {
                if (var.compare(const_COIN) == 0)
                {
                    replace.assign(1u, (rand() & 1) ? '1' : '0');
                }
                else
                    if (var.compare(const_RANDOM) == 0)
                    {
                        replace = wxString::Format("%d", rand() & 0xffff);
                    }
                    else
                    {
                        if (!Macros.empty())
                        {
                            MacrosMap::const_iterator it;

                            if ((it = Macros.find(var)) != Macros.end())
                            {
                                replace = it->second;
                            }
                        }
                    }
            }

            const wxChar l = search.Last(); // make non-braced variables work

            if (l == '/' || l == '\\' || l == '$' || l == ' ')
            {
                replace.append(l);
            }

            if (replace.IsEmpty() && search.StartsWith("$("))
            {
                replace = var.Upper();

                if (replace.Find('.') == wxNOT_FOUND)
                {
                    replace.Append(".BASE");
                }

                replace.Prepend("${GCV_");
                replace.Replace("#", "");
                replace.Replace("(", "");
                replace.Replace(")", "");
                replace.Replace(".", "_");
                replace.Append("}");
            }

            if (!replace.IsEmpty())
            {
                if (replace.IsSameAs("\"\"") || replace.IsSameAs("''") || replace.empty())
                {
                    buffer.Replace(search, "", false);
                }
                else
                {
                    buffer.Replace(search, replace, false);
                }
            }
        }

        while (m_RE_DOS.Matches(buffer))
        {
            replace.Empty();
            search = m_RE_DOS.GetMatch(buffer, 2);
            wxString var = m_RE_DOS.GetMatch(buffer, 3).Upper();

            if (var.GetChar(0) == '#')
            {
                replace = var.Upper();

                if (replace.Find('.') == wxNOT_FOUND)
                {
                    replace.Append(".BASE");
                }

                replace.Prepend("${GCV_");
                replace.Replace("#", "");
                replace.Replace(".", "_");
                replace.Append("}");
            }
            else
            {
                if (var.compare(const_COIN) == 0)
                {
                    replace.assign(1u, (rand() & 1) ? '1' : '0');
                }
                else
                    if (var.compare(const_RANDOM) == 0)
                    {
                        replace = wxString::Format("%d", rand() & 0xffff);
                    }
                    else
                    {
                        if (!Macros.empty())
                        {
                            MacrosMap::const_iterator it;

                            if ((it = Macros.find(var)) != Macros.end())
                            {
                                replace = it->second;
                            }
                        }
                    }
            }

            if (!replace.IsEmpty())
            {
                if (replace.IsSameAs("\"\"") || replace.IsSameAs("''") || replace.empty())
                {
                    buffer.Replace(search, "", false);
                }
                else
                {
                    buffer.Replace(search, replace, false);
                }
            }
        }
    }

    buffer.Replace("%%", "%");
    buffer.Replace("$$", "$");
    buffer.Replace("\\", "/");
}

void CMakeListsExporter::ExportGlobalVariableSets(ExportMode eMode)
{
    ConfigManager * pCfgMan = Manager::Get()->GetConfigManager("gcv");

    if (pCfgMan)
    {
        const wxString defSet(pCfgMan->Read("/active"));

        if (eMode == ExportMode::GVS_EXPORT_DEFAULT_ONLY)
        {
            m_ContentCMakeListGlobalVariables.append(wxString::Format("# Global Variables for \"%s\" set:%s", defSet, EOL));
        }
        else
        {
            m_ContentCMakeListGlobalVariables.append(wxString::Format("# Global Variables. The default set is: \"%s\"%s", defSet, EOL));
        }

        m_ContentCMakeListGlobalVariables.append(EOL);
        wxString value;
        const wxString cSets("/sets/");
        wxArrayString sets = pCfgMan->EnumerateSubPaths(cSets);
        sets.Sort();

        for (const wxString & sCurrentSet : sets)
        {
            wxArrayString vars = pCfgMan->EnumerateSubPaths(cSets + sCurrentSet + "/");
            vars.Sort();

            for (const wxString & sCurrentVar : vars)
            {
                wxString path(cSets + sCurrentSet + '/' + sCurrentVar + '/');
                wxArrayString knownMembers = pCfgMan->EnumerateKeys(path);

                if (defSet.IsSameAs(sCurrentSet, false))
                {
                    if (eMode != ExportMode::GVS_EXPORT_NON_DEFAULT)
                    {
                        for (unsigned int i = 0; i < knownMembers.GetCount(); ++i)
                        {
                            value = pCfgMan->Read(path + knownMembers[i]);
                            ConvertMacros(value);
                            m_ContentCMakeListGlobalVariables.append(wxString::Format("set( GCV_%s_%s \"%s\")%s", sCurrentVar.Upper(), knownMembers[i].Upper(), value, EOL));
                        }

                        if (eMode == ExportMode::GVS_EXPORT_ALL)
                        {
                            m_ContentCMakeListGlobalVariables.append(EOL);
                        }
                    }
                }
                else
                {
                    if (eMode != ExportMode::GVS_EXPORT_DEFAULT_ONLY)
                    {
                        for (unsigned int i = 0; i < knownMembers.GetCount(); ++i)
                        {
                            value = pCfgMan->Read(path + knownMembers[i]);
                            ConvertMacros(value);
                            m_ContentCMakeListGlobalVariables.append(wxString::Format("# %s.%s.%s: %s%s", sCurrentSet, sCurrentVar, knownMembers[i], value, EOL));
                        }

                        m_ContentCMakeListGlobalVariables.append(EOL);
                    }
                }
            }
        }
    }
    else
    {
        m_ContentCMakeListGlobalVariables.append(wxString::Format("# No Global Variables found!%s", EOL));
    }

    m_ContentCMakeListGlobalVariables.append(EOL);
}

void CMakeListsExporter::ExportMacros()
{
    MacrosManager * macroMan = Manager::Get()->GetMacrosManager();

    if (macroMan)
    {
        const MacrosMap & Macros = macroMan->GetMacros();

        if (Macros.empty())
        {
            m_ContentCMakeListGlobalVariables.append(wxString::Format("# No Macros found!%s", EOL));
        }
        else
        {
            m_ContentCMakeListGlobalVariables.append(wxString::Format("# Macros:%s", EOL));
            // MacrosMap uses a hash as key, to get sorted macros we need to copy them to a non-hashed map
            std::map <wxString, wxString> NewMap;

            for (MacrosMap::const_iterator it = Macros.begin(); it != Macros.end(); ++it)
            {
                NewMap[it->first] = it->second;
            }

            for (decltype(NewMap)::value_type & Item : NewMap)
            {
                m_ContentCMakeListGlobalVariables.append(wxString::Format("#        %s: %s%s", Item.first,  Item.second, EOL));
            }
        }
    }

    m_ContentCMakeListGlobalVariables.append(EOL);
}

void CMakeListsExporter::ExportGlobalVariables()
{
    m_ContentCMakeListGlobalVariables  = wxEmptyString;
    m_sGlobalVariableFileName = wxEmptyString;
    cbWorkspace * pWorkspace = Manager::Get()->GetProjectManager()->GetWorkspace();

    if (pWorkspace)
    {
        FileManager * fileMgr = Manager::Get()->GetFileManager();
        LogManager * logMgr = Manager::Get()->GetLogManager();
        // ====================================================================================
        // Global variables - GVS_EXPORT_DEFAULT_ONLY
        ExportGlobalVariableSets(ExportMode::GVS_EXPORT_DEFAULT_ONLY);
        // ====================================================================================
        m_ContentCMakeListGlobalVariables.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
        m_ContentCMakeListGlobalVariables.append(EOL);
        // ====================================================================================
        // Global variables - GVS_EXPORT_NON_DEFAULT
        ExportGlobalVariableSets(ExportMode::GVS_EXPORT_NON_DEFAULT);
        // ====================================================================================
        m_ContentCMakeListGlobalVariables.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
        m_ContentCMakeListGlobalVariables.append(EOL);
        // ====================================================================================
        // Macros
        ExportMacros();
        // ====================================================================================
        // Save the global variables to a file
        wxString wxsFileName = wxEmptyString;
        const wxString sGlobalVariableFileName("CMakeLists_GlobalVariablesCodeBlocks_Windows.txt");
        cbWorkspace * pWorkspace = Manager::Get()->GetProjectManager()->GetWorkspace();

        if (pWorkspace && pWorkspace->IsOK())
        {
            wxFileName wxfWorkspaceFileName(pWorkspace->GetFilename());
            wxsFileName = ValidateFilename(wxString::Format("%s%s", wxfWorkspaceFileName.GetPathWithSep(), sGlobalVariableFileName));
        }
        else
        {
            cbProject * project = Manager::Get()->GetProjectManager()->GetProjects()->Item(0);

            if (project)
            {
                wxFileName wxfProjectFileName(project->GetFilename());
                wxsFileName = ValidateFilename(wxString::Format("%s%s", wxfProjectFileName.GetPathWithSep(), sGlobalVariableFileName));
            }
        }

        if (!wxsFileName.IsEmpty())
        {
            fileMgr->Save(wxsFileName, m_ContentCMakeListGlobalVariables, wxFONTENCODING_SYSTEM, true, true);
            logMgr->DebugLog(wxString::Format("Exported file: %s", wxsFileName));
            m_sGlobalVariableFileName = wxsFileName.Clone();
        }
        else
        {
            logMgr->DebugLogError(wxString::Format("Could not export the global variables!!!"));
        }
    }

    m_ContentCMakeListGlobalVariables = wxEmptyString;
}

wxString CMakeListsExporter::ValidateFilename(const wxString & iFileName)
{
    wxFileName fnFileName(iFileName);
    wxString sFN = fnFileName.GetName();

    for (size_t i = 0; i < sFN.Length(); i++)
    {
        if (!wxIsalnum(sFN[i]))
        {
            sFN[i] = '_';
        }
    }

    fnFileName.SetName(sFN);
    return fnFileName.GetFullPath();
}

wxString CMakeListsExporter::GetHumanReadableOptionRelation(ProjectBuildTarget * buildTarget, OptionsRelationType type)
{
    wxString result = wxEmptyString;
    OptionsRelation relation = buildTarget->GetOptionRelation(type);

    switch (relation)
    {
        case orUseParentOptionsOnly:
            result = wxString::Format("Use parent options only. Type %d", (int)relation);
            break;

        case orUseTargetOptionsOnly:
            result = wxString::Format("Use target options only. Type %d", (int)relation);
            break;

        case orPrependToParentOptions:
            result = wxString::Format("Use target options first then parent options. Type %d", (int)relation);
            break;

        case orAppendToParentOptions:
            result = wxString::Format("Uses parent options first then target options. Type %d", (int)relation);
            break;

        default:
            result = wxString::Format("Unknown option for type: %d", (int)relation);
            break;
    }

    return result;
}

wxString CMakeListsExporter::GetTargetRootDirectory(ProjectBuildTarget * buildTarget, wxString & wsRelTargetRootDirectory)
{
    wxFileName wxfTargetFileName;
    FilesList & filesList = buildTarget->GetFilesList();

    for (FilesList::iterator j = filesList.begin(); j != filesList.end(); j++)
    {
        ProjectFile * pf = *j;
        wxString cfn(pf->relativeFilename);

        if (cfn.Right(4) == ".cpp" || cfn.Right(2) == ".c" || cfn.Right(3) == ".cc" || cfn.Right(4) == ".cxx")
        {
            if (pf->compile)
            {
                if (!wxfTargetFileName.Exists())
                {
                    wxfTargetFileName.Assign(cfn);
                }
                else
                {
                    wxFileName wxfSrcFile(cfn);

                    if (wxfSrcFile.GetDirCount() < wxfTargetFileName.GetDirCount())
                    {
                        wxfTargetFileName.Assign(cfn);
                    }
                }
            }
        }
    }

    wsRelTargetRootDirectory = wxfTargetFileName.GetPath();
    return wxfTargetFileName.GetFullPath();
}

void CMakeListsExporter::RunExport()
{
    ExportGlobalVariables();

    if (m_sGlobalVariableFileName.IsEmpty())
    {
        return;
    }

    m_ContentCMakeListTarget = wxEmptyString;
    FileManager * fileMgr = Manager::Get()->GetFileManager();
    LogManager * logMgr = Manager::Get()->GetLogManager();
    wxString tmpStringA, tmpStringB, tmpStringC;
    wxArrayString tmpArrayA, tmpArrayB, tmpArrayC;
    ProjectsArray * arr = Manager::Get()->GetProjectManager()->GetProjects();

    for (unsigned int i = 0; i < arr->GetCount(); ++i)
    {
        cbProject * project = arr->Item(i);

        if (!project)
        {
            continue;
        }

        wxString projectTitle = project->GetTitle();
        wxString projectTopLevelPath = UnixFilename(project->GetCommonTopLevelPath(), wxPATH_UNIX);

        for (int i = 0; i < project->GetBuildTargetsCount(); i++)
        {
            ProjectBuildTarget * buildTarget = project->GetBuildTarget(i);

            if (!buildTarget)
            {
                continue;
            }

            wxString targetTitle = buildTarget->GetTitle();
            m_ContentCMakeListTarget.Clear();
            m_ContentCMakeListTarget.append(wxString::Format("cmake_minimum_required(VERSION 3.15) %s", EOL));
            m_ContentCMakeListTarget.append(EOL);
            m_ContentCMakeListTarget.append(wxString::Format("project(%s)%s", targetTitle, EOL));
            m_ContentCMakeListTarget.append(wxString::Format("# Target Title: %s%s", targetTitle, EOL));
            m_ContentCMakeListTarget.append(wxString::Format("# Output FileName: %s%s",  buildTarget->GetOutputFilename(), EOL));
            m_ContentCMakeListTarget.append(wxString::Format("# Project Compiler: %s%s", project->GetCompilerID(), EOL));
            m_ContentCMakeListTarget.append(wxString::Format("# Target Compiler:  %s%s", buildTarget->GetCompilerID(), EOL));
            m_ContentCMakeListTarget.append(wxString::Format("# Project CommonTopLevelPath:  %s%s", project->GetCommonTopLevelPath(), EOL));

            // ====================================================================================
            // Target Output Type
            switch (buildTarget->GetTargetType())
            {
                case ttExecutable:
                    m_ContentCMakeListTarget.append(wxString::Format("# Target type: ttExecutable%s", EOL));
                    break;

                case ttConsoleOnly:
                    m_ContentCMakeListTarget.append(wxString::Format("# Target type: ttConsoleOnly%s", EOL));
                    break;

                case ttStaticLib:
                    m_ContentCMakeListTarget.append(wxString::Format("# Target type: ttStaticLib%s", EOL));
                    break;

                case ttDynamicLib:
                    if (buildTarget->GetCreateStaticLib() || buildTarget->GetCreateDefFile())
                    {
                        m_ContentCMakeListTarget.append(wxString::Format("# Target type: ttDynamicLib - DLL%s", EOL));
                    }
                    else
                    {
                        m_ContentCMakeListTarget.append(wxString::Format("# Target type: ttDynamicLib - module%s",  EOL));
                    }

                    break;

                default:
                    m_ContentCMakeListTarget.append(wxString::Format("# Target type: unrecognized target type%s", EOL));
                    Manager::Get()->GetLogManager()->LogError("Warning: \"" + targetTitle + "\" is of an unrecognized target type; skipping...");
                    break;
            }

            wxString wsRelTargetRootDirectory;
            wxString sTargetRootDir = GetTargetRootDirectory(buildTarget, wsRelTargetRootDirectory);
            m_ContentCMakeListTarget.append(wxString::Format("# Target detected root directory:  %s (%s)%s", sTargetRootDir, wsRelTargetRootDirectory, EOL));
            m_ContentCMakeListTarget.append(wxString::Format("# Target Options:%s", EOL));
            m_ContentCMakeListTarget.append(wxString::Format("#                 projectCompilerOptionsRelation: %s%s",     GetHumanReadableOptionRelation(buildTarget, ortCompilerOptions), EOL));
            m_ContentCMakeListTarget.append(wxString::Format("#                 projectLinkerOptionsRelation: %s%s",       GetHumanReadableOptionRelation(buildTarget, ortLinkerOptions), EOL));
            m_ContentCMakeListTarget.append(wxString::Format("#                 projectIncludeDirsRelation: %s%s",         GetHumanReadableOptionRelation(buildTarget, ortIncludeDirs), EOL));
            m_ContentCMakeListTarget.append(wxString::Format("#                 projectLibDirsRelation: %s%s",             GetHumanReadableOptionRelation(buildTarget, ortLibDirs), EOL));
            m_ContentCMakeListTarget.append(wxString::Format("#                 projectResourceIncludeDirsRelation: %s%s", GetHumanReadableOptionRelation(buildTarget, ortResDirs), EOL));
            m_ContentCMakeListTarget.append(EOL);
            // ====================================================================================
            m_ContentCMakeListTarget.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
            m_ContentCMakeListTarget.append(EOL);
            // ====================================================================================
            m_ContentCMakeListTarget.append(wxString::Format("# Include global variable definition file:%s", EOL));
            m_ContentCMakeListTarget.append(wxString::Format("include(%s)%s", UnixFilename(m_sGlobalVariableFileName, wxPATH_UNIX), EOL));
            m_ContentCMakeListTarget.append(EOL);
            // ====================================================================================
            m_ContentCMakeListTarget.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
            m_ContentCMakeListTarget.append(EOL);
            // ====================================================================================
            m_ContentCMakeListTarget.append(wxString::Format("# Include CMakePrintHelpers module:%s", EOL));
            m_ContentCMakeListTarget.append(wxString::Format("include(CMakePrintHelpers)%s", EOL));
            m_ContentCMakeListTarget.append(EOL);
            // ====================================================================================
            m_ContentCMakeListTarget.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
            m_ContentCMakeListTarget.append(EOL);
            // ====================================================================================
            // Compiler search directories
            tmpArrayA = AppendOptionsArray(project->GetIncludeDirs(), buildTarget->GetIncludeDirs(), buildTarget->GetOptionRelation(ortIncludeDirs));
            tmpStringA.Clear();

            for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
            {
                tmpStringA += wxString::Format("\"%s\"%s                    ", tmpArrayA[j], EOL);
            }

            if (!tmpStringA.IsEmpty())
            {
                m_ContentCMakeListTarget.append(wxString::Format("# Compiler Include paths:%s", EOL));
                tmpStringA.Replace("/", "\\");
                ConvertMacros(tmpStringA);
                m_ContentCMakeListTarget.append(wxString::Format("include_directories(%s)%s", tmpStringA, EOL));
                m_ContentCMakeListTarget.append(EOL);
            }

            // ====================================================================================
            // Compiler options
            tmpArrayA = AppendOptionsArray(project->GetCompilerOptions(), buildTarget->GetCompilerOptions(), buildTarget->GetOptionRelation(ortCompilerOptions));
            tmpStringA.Clear();
            tmpStringB.Clear();

            for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
            {
                if ((tmpArrayA[j].Left(2) == "-D") || (tmpArrayA[j].Left(2) == "/D"))
                {
                    wxString tmpStringCVT = tmpArrayA[j].Clone();
                    ConvertMacros(tmpStringCVT);
                    tmpStringB += wxString::Format("add_definitions(%s)%s", tmpStringCVT, EOL);
                }
                else
                {
                    wxString tmpStringCVT = tmpArrayA[j].Clone();
                    ConvertMacros(tmpStringCVT);
                    tmpStringA += wxString::Format("set( CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} %s\")%s", tmpStringCVT, EOL);
                }
            }

            if (!tmpStringA.IsEmpty())
            {
                m_ContentCMakeListTarget.append(wxString::Format("# Compiler flags:%s", EOL));
                m_ContentCMakeListTarget.append(wxString::Format("%s%s", tmpStringA, EOL));
            }

            if (!tmpStringB.IsEmpty())
            {
                m_ContentCMakeListTarget.append(wxString::Format("# Compiler Definitions:%s", EOL));
                m_ContentCMakeListTarget.append(wxString::Format("%s%s", tmpStringB, EOL));
            }

            // ====================================================================================
            m_ContentCMakeListTarget.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
            m_ContentCMakeListTarget.append(EOL);
            // ====================================================================================
            // Source code file list
            FilesList & filesList = buildTarget->GetFilesList();
            wxArrayString tmpArraySrc, tmpArrayhdr, tmpArrayRes;

            for (FilesList::iterator j = filesList.begin(); j != filesList.end(); j++)
            {
                ProjectFile * pf = *j;
                wxString cfn(pf->relativeFilename);

                // is header
                if (cfn.Right(2) == ".h" || cfn.Right(4) == ".hpp" || cfn.Right(4) == ".hxx" || cfn.Right(3) == ".hh")
                {
                    tmpArrayhdr.Add(ConvertSlash(cfn));
                }
                else
                    if (cfn.Right(4) == ".cpp" || cfn.Right(2) == ".c" || cfn.Right(3) == ".cc" || cfn.Right(4) == ".cxx")
                    {
                        if (pf->compile)
                        {
                            tmpArraySrc.Add(ConvertSlash(cfn));
                        }
                    }
                    else
                        if (cfn.Right(3) == ".rc")
                        {
                            if (pf->compile)
                            {
                                tmpArrayRes.Add(ConvertSlash(cfn));
                            }
                        }
            }

            if (!tmpArraySrc.IsEmpty())
            {
                bool bIsFirstEntry = true;
                tmpArraySrc.Sort();
                m_ContentCMakeListTarget.append(wxString::Format("# Source files to compile:%s", EOL));
                m_ContentCMakeListTarget.append(wxString::Format("unset(SOURCE_FILES)%s", EOL));

                for (unsigned int j = 0; j < tmpArraySrc.GetCount(); j++)
                {
                    tmpStringA = tmpArraySrc[j];
                    tmpStringA.Replace("/", "\\");
                    ConvertMacros(tmpStringA);

                    if (bIsFirstEntry)
                    {
                        m_ContentCMakeListTarget.append(wxString::Format("set( SOURCE_FILES \"%s%s\")%s", projectTopLevelPath, tmpStringA, EOL));
                        bIsFirstEntry = false;
                    }
                    else
                    {
                        m_ContentCMakeListTarget.append(wxString::Format("set( SOURCE_FILES ${SOURCE_FILES} \"%s%s\")%s", projectTopLevelPath, tmpStringA, EOL));
                    }
                }

                m_ContentCMakeListTarget.append(EOL);
            }

            if (!tmpArrayhdr.IsEmpty())
            {
                tmpArrayhdr.Sort();
                m_ContentCMakeListTarget.append(wxString::Format("# Header files :%s", EOL));

                for (unsigned int j = 0; j < tmpArrayhdr.GetCount(); j++)
                {
                    tmpStringA = tmpArrayhdr[j];
                    tmpStringA.Replace("/", "\\");
                    ConvertMacros(tmpStringA);

                    if ((buildTarget->GetTargetType() == ttStaticLib) || (buildTarget->GetTargetType() == ttDynamicLib))
                    {
                        m_ContentCMakeListTarget.append(wxString::Format("set( SOURCE_FILES ${SOURCE_FILES} \"%s%s\")%s", projectTopLevelPath, tmpStringA, EOL));
                    }
                    else
                    {
                        m_ContentCMakeListTarget.append(wxString::Format("# \"%s%s\"%s", projectTopLevelPath, tmpStringA, EOL));
                    }
                }

                m_ContentCMakeListTarget.append(EOL);
            }

            // ====================================================================================
            // Target Output Type with source fikes
            switch (buildTarget->GetTargetType())
            {
                case ttExecutable:
                    m_ContentCMakeListTarget.append(wxString::Format("# Target type: ttExecutable%s", EOL));
                    m_ContentCMakeListTarget.append(wxString::Format("add_executable(${PROJECT_NAME} ${SOURCE_FILES})%s", EOL));
                    break;

                case ttConsoleOnly:
                    m_ContentCMakeListTarget.append(wxString::Format("# Target type: ttConsoleOnly%s", EOL));
                    m_ContentCMakeListTarget.append(wxString::Format("add_executable(${PROJECT_NAME} ${SOURCE_FILES})%s", EOL));
                    break;

                case ttStaticLib:
                    m_ContentCMakeListTarget.append(wxString::Format("# Target type: ttStaticLib%s", EOL));
                    m_ContentCMakeListTarget.append(wxString::Format("add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})%s", EOL));
                    break;

                case ttDynamicLib:
                    if (buildTarget->GetCreateStaticLib() || buildTarget->GetCreateDefFile())
                    {
                        m_ContentCMakeListTarget.append(wxString::Format("# Target type: ttDynamicLib - DLL%s", EOL));
                        m_ContentCMakeListTarget.append(wxString::Format("add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})%s", EOL));
                    }
                    else
                    {
                        m_ContentCMakeListTarget.append(wxString::Format("# Target type: ttDynamicLib - module%s",  EOL));
                        m_ContentCMakeListTarget.append(wxString::Format("add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})%s", EOL));
                    }

                    break;

                default:
                    m_ContentCMakeListTarget.append(wxString::Format("# Target type: unrecognized target type%s", EOL));
                    Manager::Get()->GetLogManager()->LogError("Warning: \"" + targetTitle + "\" is of an unrecognized target type; skipping...");
                    break;
            }

            m_ContentCMakeListTarget.append(wxString::Format("unset(SOURCE_FILES)%s", EOL));
            m_ContentCMakeListTarget.append(EOL);
            // ====================================================================================
            m_ContentCMakeListTarget.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
            m_ContentCMakeListTarget.append(EOL);
            // ====================================================================================
            // Linker search directories
            tmpArrayA = AppendOptionsArray(project->GetLibDirs(), buildTarget->GetLibDirs(), buildTarget->GetOptionRelation(ortLibDirs));

            if (!tmpArrayA.IsEmpty())
            {
                bool bIsFirstEntry = true;
                m_ContentCMakeListTarget.append(wxString::Format("# Linker search paths:%s", EOL));
                m_ContentCMakeListTarget.append(wxString::Format("unset(LINKER_DIR_LIST)%s", EOL));

                for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
                {
                    tmpStringA = tmpArrayA[j].Clone();
                    ConvertMacros(tmpStringA);

                    if (bIsFirstEntry)
                    {
                        m_ContentCMakeListTarget.append(wxString::Format("set( LINKER_DIR_LIST \"%s\")%s", tmpStringA, EOL));
                        bIsFirstEntry = false;
                    }
                    else
                    {
                        m_ContentCMakeListTarget.append(wxString::Format("set( LINKER_DIR_LIST ${LINKER_DIR_LIST} \"%s\")%s", tmpStringA, EOL));
                    }
                }

                m_ContentCMakeListTarget.append(wxString::Format("target_link_directories(${PROJECT_NAME} PRIVATE ${LINKER_DIR_LIST})%s", EOL));
                m_ContentCMakeListTarget.append(wxString::Format("unset(LINKER_DIR_LIST)%s", EOL));
                m_ContentCMakeListTarget.append(EOL);
            }

            // ====================================================================================
            // Linker options
            tmpArrayA = AppendOptionsArray(project->GetLinkerOptions(), buildTarget->GetLinkerOptions(), buildTarget->GetOptionRelation(ortLinkerOptions));

            if (!tmpArrayA.IsEmpty())
            {
                bool bIsFirstEntry = true;
                m_ContentCMakeListTarget.append(wxString::Format("# Linker options:%s", EOL));
                m_ContentCMakeListTarget.append(wxString::Format("unset(LINKER_OPTIONS_LIST)%s", EOL));

                for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
                {
                    tmpStringA = tmpArrayA[j].Clone();
                    ConvertMacros(tmpStringA);

                    if (bIsFirstEntry)
                    {
                        m_ContentCMakeListTarget.append(wxString::Format("set( LINKER_OPTIONS_LIST \"%s\")%s", tmpStringA, EOL));
                        bIsFirstEntry = false;
                    }
                    else
                    {
                        m_ContentCMakeListTarget.append(wxString::Format("set( LINKER_OPTIONS_LIST ${LINKER_OPTIONS_LIST} \"%s\")%s", tmpStringA, EOL));
                    }
                }

                m_ContentCMakeListTarget.append(wxString::Format("target_link_options(${PROJECT_NAME} PRIVATE ${LINKER_OPTIONS_LIST})%s", EOL));
                m_ContentCMakeListTarget.append(wxString::Format("unset(LINKER_OPTIONS_LIST)%s", EOL));
                m_ContentCMakeListTarget.append(EOL);
            }

            tmpArrayA = AppendOptionsArray(project->GetLinkLibs(), buildTarget->GetLinkLibs(), buildTarget->GetOptionRelation(ortLibDirs));

            if (!tmpArrayA.IsEmpty())
            {
                bool bIsFirstEntry = true;
                m_ContentCMakeListTarget.append(wxString::Format("# Linker libraries to include:%s", EOL));
                m_ContentCMakeListTarget.append(wxString::Format("unset(LINKER_LIBRARIES_LIST)%s", EOL));

                for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
                {
                    tmpStringA = tmpArrayA[j].Clone();
                    ConvertMacros(tmpStringA);

                    if (bIsFirstEntry)
                    {
                        m_ContentCMakeListTarget.append(wxString::Format("set( LINKER_LIBRARIES_LIST \"%s\")%s", tmpStringA, EOL));
                        bIsFirstEntry = false;
                    }
                    else
                    {
                        m_ContentCMakeListTarget.append(wxString::Format("set( LINKER_LIBRARIES_LIST ${LINKER_LIBRARIES_LIST} \"%s\")%s", tmpStringA, EOL));
                    }
                }

                ConvertMacros(tmpStringA);
                m_ContentCMakeListTarget.append(wxString::Format("target_link_libraries(${PROJECT_NAME} ${LINKER_LIBRARIES_LIST})%s", EOL));
                m_ContentCMakeListTarget.append(wxString::Format("unset(LINKER_LIBRARIES_LIST)%s", EOL));
                m_ContentCMakeListTarget.append(EOL);
            }

            // ====================================================================================
            m_ContentCMakeListTarget.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
            m_ContentCMakeListTarget.append(EOL);

            // ====================================================================================
            // Resource file(s)

            if (!tmpArrayRes.IsEmpty())
            {
                tmpArrayRes.Sort();
                tmpStringC.Replace("/", "\\");
                m_ContentCMakeListTarget.append(wxString::Format("if (MINGW)%s", EOL));
                m_ContentCMakeListTarget.append(wxString::Format("    # Windows Resource file:%s", EOL));

                for (unsigned int j = 0; j < tmpArrayRes.GetCount(); j++)
                {
                    tmpStringA = tmpArrayRes[j];
                    tmpStringA.Replace("/", "\\");
                    ConvertMacros(tmpStringA);
                    m_ContentCMakeListTarget.append(wxString::Format("    add_library(${PROJECT_NAME} \"%s\")%s", tmpStringA, EOL));
                }

                m_ContentCMakeListTarget.append(EOL);
                wxArrayString tmpArrayRI = AppendOptionsArray(project->GetResourceIncludeDirs(), buildTarget->GetResourceIncludeDirs(), buildTarget->GetOptionRelation(ortResDirs));
                wxString tmpStringRI;

                for (unsigned int j = 0; j < tmpArrayRI.GetCount(); j++)
                {
                    tmpStringRI += tmpArrayRI[j];
                }

                if (!tmpStringRI.IsEmpty())
                {
                    m_ContentCMakeListTarget.append(EOL);
                    m_ContentCMakeListTarget.append(wxString::Format("    # Resource include directories:%s", EOL));
                    tmpStringRI.Replace("/", "\\");
                    ConvertMacros(tmpStringRI);
                    m_ContentCMakeListTarget.append(wxString::Format("    set (CMAKE_RC_FLAGS \"${CMAKE_RC_FLAGS} ${WX_RC_FLAGS} %s\")%s", tmpStringRI, EOL));
                }

                m_ContentCMakeListTarget.append(wxString::Format("endif() %s", EOL));
                m_ContentCMakeListTarget.append(EOL);
            }

            // ====================================================================================
            m_ContentCMakeListTarget.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
            m_ContentCMakeListTarget.append(EOL);
            // ====================================================================================
            // Before build commands
            tmpArrayA = buildTarget->GetCommandsBeforeBuild();

            if (!tmpArrayA.IsEmpty())
            {
                m_ContentCMakeListTarget.append(wxString::Format("# Target before commands:%s", EOL));

                for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
                {
                    tmpStringA = tmpArrayA[j];
                    //ConvertMacros(tmpStringA);
                    m_ContentCMakeListTarget.append(wxString::Format("# %s%s", tmpStringA, EOL));
                }
            }

            tmpArrayA = project->GetCommandsBeforeBuild();

            if (!tmpArrayA.IsEmpty())
            {
                m_ContentCMakeListTarget.append(wxString::Format("# Project before commands:%s", EOL));

                for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
                {
                    tmpStringA = tmpArrayA[j];
                    //ConvertMacros(tmpStringA);
                    m_ContentCMakeListTarget.append(wxString::Format("# %s%s", tmpStringA, EOL));
                }

                m_ContentCMakeListTarget.append(EOL);
            }

            // ====================================================================================
            // After build commands
            tmpArrayA = buildTarget->GetCommandsAfterBuild();

            if (!tmpArrayA.IsEmpty())
            {
                m_ContentCMakeListTarget.append(wxString::Format("# Target after commands:%s", EOL));

                for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
                {
                    tmpStringA = tmpArrayA[j];
                    //ConvertMacros(tmpStringA);
                    m_ContentCMakeListTarget.append(wxString::Format("# %s%s", tmpStringA, EOL));
                }

                m_ContentCMakeListTarget.append(EOL);
            }

            tmpArrayA = project->GetCommandsAfterBuild();

            if (!tmpArrayA.IsEmpty())
            {
                m_ContentCMakeListTarget.append(wxString::Format("# Project after commands:%s", EOL));

                for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
                {
                    tmpStringA = tmpArrayA[j];
                    //ConvertMacros(tmpStringA);
                    m_ContentCMakeListTarget.append(wxString::Format("# %s%s", tmpStringA, EOL));
                }

                m_ContentCMakeListTarget.append(EOL);
            }

            // ====================================================================================
            m_ContentCMakeListTarget.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
            m_ContentCMakeListTarget.append(EOL);
            // ====================================================================================
            //output file
            wxFileName wxfTargetFileName(sTargetRootDir);

            if (wxfTargetFileName.DirExists())
            {
                wxfTargetFileName.SetFullName("CMakeLists.txt");
                wxString wsFullFileName(wxfTargetFileName.GetFullPath());

                if (wxfTargetFileName.FileExists())
                {
                    logMgr->DebugLogError(wxString::Format("Deleting file: %s!!!", wsFullFileName));
                    ::wxRemoveFile(wxfTargetFileName.GetFullPath());

                    if (wxfTargetFileName.FileExists())
                    {
                        logMgr->DebugLogError(wxString::Format("File still exists: %s!!!", wsFullFileName));
                    }
                }

                if (!wxfTargetFileName.FileExists())
                {
                    fileMgr->Save(wsFullFileName, m_ContentCMakeListTarget, wxFONTENCODING_SYSTEM, true, true);
                    logMgr->DebugLog(wxString::Format("Exported file: %s", wsFullFileName));
                }
            }
            else
            {
                logMgr->DebugLogError(wxString::Format("Could not save CMakeLists.txt in the missing directory: %s!!!", wxfTargetFileName.GetFullPath()));
            }

            //            wxFileName wxfProjectFileName(project->GetFilename());
            //            wxString wxsFileName = ValidateFilename(wxString::Format("%sCMakeLists_%s_%s_%s.txt", wxfProjectFileName.GetPathWithSep(), wxfProjectFileName.GetName(), projectTitle, targetTitle));
            //            fileMgr->Save(wxsFileName, m_ContentCMakeListTarget, wxFONTENCODING_SYSTEM, true, true);
            //            logMgr->DebugLog(wxString::Format("Exported file: %s", wxsFileName));
        }
    }

    m_ContentCMakeListTarget = wxEmptyString;
}
