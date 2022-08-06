// System include files
#include <wx/filename.h>
#include <wx/textfile.h>

// CB include files
#include <sdk.h> // Code::Blocks SDK
#include "filemanager.h"
#include "logmanager.h"
#include "manager.h"
#include "macrosmanager.h"
#include <tinyxml.h>

// ProjectExporter include files
#include "CMakeListsExporter.h"

CMakeListsExporter::CMakeListsExporter()
{
    //ctor
}

CMakeListsExporter::~CMakeListsExporter()
{
    //dtor
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

wxString CMakeListsExporter::GetOptions(const wxString & source)
{
    wxString output = source.AfterFirst('$');

    //Manager::Get()->GetLogManager()->Log( source.AfterFirst('$') );
    while (!output.IsEmpty())
    {
        if (output.Left(1) == "(")
        {
            if (!output.BeforeFirst(')').IsEmpty() && !StringExists(m_options, "$" + output.BeforeFirst(')') + ")"))
            {
                m_options.Add("$" + output.BeforeFirst(')') + ")");
            }
        }
        else
            if (!output.BeforeFirst(' ').IsEmpty())
            {
                if (!StringExists(m_options, "$(" + output.BeforeFirst(' ') + ")"))
                {
                    m_options.Add("$(" + output.BeforeFirst(' ') + ")");
                }
            }

        output = output.AfterFirst('$');
    }

    output = source;
    output.Replace("$(#", "$(");
    return output;
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

void CMakeListsExporter::RunExport()
{
    FileManager * fileMgr = Manager::Get()->GetFileManager();
    LogManager * logMgr = Manager::Get()->GetLogManager();
    wxString tmpStringA, tmpStringB, tmpStringC;
    wxArrayString tmpArrayA, tmpArrayB, tmpArrayC;
    wxString m_content;
    const wxChar * EOL = wxTextFile::GetEOL();
    ProjectsArray * arr = Manager::Get()->GetProjectManager()->GetProjects();

    for (unsigned int i = 0; i < arr->GetCount(); ++i)
    {
        cbProject * project = arr->Item(i);

        if (!project)
        {
            continue;
        }

        wxString projectTitle = project->GetTitle();

        for (int i = 0; i < project->GetBuildTargetsCount(); i++)
        {
            ProjectBuildTarget * buildTarget = project->GetBuildTarget(i);

            if (!buildTarget)
            {
                continue;
            }

            wxString targetTitle = buildTarget->GetTitle();
            m_content.Clear();
            m_content.append(wxString::Format("cmake_minimum_required(VERSION 3.15) %s", EOL));
            m_content.append(EOL);
            m_content.append(wxString::Format("project(%s)%s", targetTitle, EOL));
            m_content.append(wxString::Format("# Target Title: %s%s", targetTitle, EOL));
            m_content.append(wxString::Format("# Output FileName: %s%s",  buildTarget->GetOutputFilename(), EOL));
            m_content.append(wxString::Format("# Project Compiler: %s%s", project->GetCompilerID(), EOL));
            m_content.append(wxString::Format("# Target Compiler:  %s%s", buildTarget->GetCompilerID(), EOL));

            // ====================================================================================
            // Target Output Type
            switch (buildTarget->GetTargetType())
            {
                case ttExecutable:
                    m_content.append(wxString::Format("# Target type: ttExecutable%s", EOL));
                    break;

                case ttConsoleOnly:
                    m_content.append(wxString::Format("# Target type: ttConsoleOnly%s", EOL));
                    break;

                case ttStaticLib:
                    m_content.append(wxString::Format("# Target type: ttStaticLib%s", EOL));
                    break;

                case ttDynamicLib:
                    if (buildTarget->GetCreateStaticLib() || buildTarget->GetCreateDefFile())
                    {
                        m_content.append(wxString::Format("# Target type: ttDynamicLib - DLL%s", EOL));
                    }
                    else
                    {
                        m_content.append(wxString::Format("# Target type: ttDynamicLib - module%s",  EOL));
                    }

                    break;

                default:
                    m_content.append(wxString::Format("# Target type: unrecognized target type%s", EOL));
                    Manager::Get()->GetLogManager()->LogError("Warning: \"" + targetTitle + "\" is of an unrecognized target type; skipping...");
                    break;
            }

            m_content.append(wxString::Format("# Target Options:%s", EOL));
            m_content.append(wxString::Format("#                 projectCompilerOptionsRelation: %s%s",     GetHumanReadableOptionRelation(buildTarget, ortCompilerOptions), EOL));
            m_content.append(wxString::Format("#                 projectLinkerOptionsRelation: %s%s",       GetHumanReadableOptionRelation(buildTarget, ortLinkerOptions), EOL));
            m_content.append(wxString::Format("#                 projectIncludeDirsRelation: %s%s",         GetHumanReadableOptionRelation(buildTarget, ortIncludeDirs), EOL));
            m_content.append(wxString::Format("#                 projectLibDirsRelation: %s%s",             GetHumanReadableOptionRelation(buildTarget, ortLibDirs), EOL));
            m_content.append(wxString::Format("#                 projectResourceIncludeDirsRelation: %s%s", GetHumanReadableOptionRelation(buildTarget, ortResDirs), EOL));
            m_content.append(EOL);
            // ====================================================================================
            m_content.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
            m_content.append(EOL);
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
                tmpStringA.Replace("/", "\\");
                m_content.append(wxString::Format("# Compiler Include paths:%s", EOL));
                m_content.append(wxString::Format("include_directories(%s)%s", tmpStringA, EOL));
                m_content.append(EOL);
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
                    tmpStringB += wxString::Format("add_definitions(%s)%s", tmpArrayA[j], EOL);
                }
                else
                {
                    tmpStringA += wxString::Format("%s%s                      ", tmpArrayA[j], EOL);
                }
            }

            if (!tmpStringA.IsEmpty())
            {
                m_content.append(wxString::Format("# Compiler flags:%s", EOL));
                m_content.append(wxString::Format("set( CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} %s\")%s", tmpStringA, EOL));
                m_content.append(EOL);
            }

            if (!tmpStringB.IsEmpty())
            {
                tmpStringB.Replace("/", "\\");
                m_content.append(wxString::Format("# Compiler Definitions:%s", EOL));
                m_content.append(wxString::Format("%s%s", tmpStringB, EOL));
            }

            // ====================================================================================
            m_content.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
            m_content.append(EOL);
            // ====================================================================================
            // Linker search directories
            tmpArrayA = AppendOptionsArray(project->GetLibDirs(), buildTarget->GetLibDirs(), buildTarget->GetOptionRelation(ortLibDirs));
            tmpStringA.Clear();

            for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
            {
                tmpStringA += wxString::Format("\"%s\"%s                        ", GetOptions(ConvertSlash(tmpArrayA[j])), EOL);
            }

            if (!tmpStringA.IsEmpty())
            {
                m_content.append(wxString::Format("# Linker search paths:%s", EOL));
                m_content.append(wxString::Format("target_link_directories(%s)%s", tmpStringA, EOL));
                m_content.append(EOL);
            }

            // ==============================================================================================================================================================
            // Linker options
            tmpArrayA = AppendOptionsArray(project->GetLinkerOptions(), buildTarget->GetLinkerOptions(), buildTarget->GetOptionRelation(ortLinkerOptions));
            tmpStringA.Clear();

            for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
            {
                tmpStringA += wxString::Format("%s%s                    ", GetOptions(ConvertSlash(tmpArrayA[j])), EOL);
            }

            if (!tmpStringA.IsEmpty())
            {
                m_content.append(wxString::Format("# Linker options:%s", EOL));
                m_content.append(wxString::Format("target_link_options(%s)%s", tmpStringA, EOL));
                m_content.append(EOL);
            }

            tmpArrayA = AppendOptionsArray(project->GetLinkLibs(), buildTarget->GetLinkLibs(), buildTarget->GetOptionRelation(ortLibDirs));
            tmpStringA.Clear();

            for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
            {
                tmpStringA += wxString::Format("\"%s\"%s                      ", GetOptions(ConvertSlash(tmpArrayA[j])), EOL);
            }

            if (!tmpStringA.IsEmpty())
            {
                m_content.append(wxString::Format("# Linker libraries to include:%s", EOL));
                m_content.append(wxString::Format("target_link_libraries(%s)%s", tmpStringA, EOL));
                m_content.append(EOL);
            }

            // ====================================================================================
            m_content.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
            m_content.append(EOL);
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
                tmpArraySrc.Sort();
                m_content.append(wxString::Format("# Source files to compile:%s", EOL));

                for (unsigned int j = 0; j < tmpArraySrc.GetCount(); j++)
                {
                    tmpStringA = tmpArraySrc[j];
                    tmpStringA.Replace("/", "\\");
                    m_content.append(wxString::Format("add_library(\"%s\")%s", tmpStringA, EOL));
                }

                m_content.append(EOL);
            }

            if (!tmpArrayRes.IsEmpty())
            {
                tmpArrayRes.Sort();
                tmpStringC.Replace("/", "\\");
                m_content.append(wxString::Format("if (MINGW)%s", EOL));
                m_content.append(wxString::Format("    # Windows Resource file:%s", EOL));

                for (unsigned int j = 0; j < tmpArrayRes.GetCount(); j++)
                {
                    tmpStringA = tmpArrayRes[j];
                    tmpStringA.Replace("/", "\\");
                    m_content.append(wxString::Format("    add_library(\"%s\")%s", tmpStringA, EOL));
                }

                m_content.append(EOL);
                wxArrayString tmpArrayRI = AppendOptionsArray(project->GetResourceIncludeDirs(), buildTarget->GetResourceIncludeDirs(), buildTarget->GetOptionRelation(ortResDirs));
                wxString tmpStringRI;

                for (unsigned int j = 0; j < tmpArrayRI.GetCount(); j++)
                {
                    tmpStringRI += GetOptions(ConvertSlash(tmpArrayRI[j]));
                }

                if (!tmpStringRI.IsEmpty())
                {
                    tmpStringRI.Replace("/", "\\");
                    m_content.append(EOL);
                    m_content.append(wxString::Format("    # Resource include directories:%s", EOL));
                    m_content.append(wxString::Format("    set (CMAKE_RC_FLAGS \"${CMAKE_RC_FLAGS} ${WX_RC_FLAGS} %s\")%s", tmpStringRI, EOL));
                }

                m_content.append(wxString::Format("endif() %s", EOL));
                m_content.append(EOL);
            }

            m_content.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
            m_content.append(EOL);

            if (!tmpArrayhdr.IsEmpty())
            {
                tmpArrayhdr.Sort();
                m_content.append(wxString::Format("# Header file :%s", EOL));

                for (unsigned int j = 0; j < tmpArrayhdr.GetCount(); j++)
                {
                    tmpStringA = tmpArrayhdr[j];
                    tmpStringA.Replace("/", "\\");
                    m_content.append(wxString::Format("# \"%s\"%s", tmpStringA, EOL));
                }

                m_content.append(EOL);
            }

            m_content.append(wxString::Format("# -----------------------------------------------------------------------------%s", EOL));
            m_content.append(EOL);
            // ====================================================================================
            // Before build commands
            tmpArrayA = buildTarget->GetCommandsBeforeBuild();

            if (!tmpArrayA.IsEmpty())
            {
                m_content.append(wxString::Format("# Target before commands:%s", EOL));

                for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
                {
                    m_content.append(wxString::Format("# %s%s", tmpArrayA[j], EOL));
                }
            }

            tmpArrayA = project->GetCommandsBeforeBuild();

            if (!tmpArrayA.IsEmpty())
            {
                m_content.append(wxString::Format("# Project before commands:%s", EOL));

                for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
                {
                    m_content.append(wxString::Format("# %s%s", tmpArrayA[j], EOL));
                }
            }

            m_content.append(EOL);
            // ====================================================================================
            // After build commands
            tmpArrayA = buildTarget->GetCommandsAfterBuild();

            if (!tmpArrayA.IsEmpty())
            {
                m_content.append(wxString::Format("# Target after commands:%s", EOL));

                for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
                {
                    m_content.append(wxString::Format("# %s%s", tmpArrayA[j], EOL));
                }
            }

            tmpArrayA = project->GetCommandsAfterBuild();

            if (!tmpArrayA.IsEmpty())
            {
                m_content.append(wxString::Format("# Project after commands:%s", EOL));

                for (unsigned int j = 0; j < tmpArrayA.GetCount(); j++)
                {
                    m_content.append(wxString::Format("# %s%s", tmpArrayA[j], EOL));
                }
            }

            //output file
            wxFileName wxfProjectFileName(project->GetFilename());
            wxString wxsFileName = ValidateFilename(wxString::Format("%sCMakeLists_%s_%s_%s.txt", wxfProjectFileName.GetPathWithSep(), wxfProjectFileName.GetName(), projectTitle, targetTitle));
            fileMgr->Save(wxsFileName, m_content, wxFONTENCODING_SYSTEM, true, true);
            logMgr->DebugLog(wxString::Format("Exported file: %s", wxsFileName));
        }
    }
}
