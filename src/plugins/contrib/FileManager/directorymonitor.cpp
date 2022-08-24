// =================================================================
// |     see https://sourceforge.net/p/codeblocks/tickets/663/     |
// =================================================================

// C++ includes (including wxWidgets)
#include <algorithm>
#include <set>
#include <vector>
#include <wx/string.h>

// Code::Blocks includes
#include "sdk.h"
#include "manager.h"
#include "logmanager.h"

// FileManager includes
#include "directorymonitor.h"


// In milliseconds
#define FILE_CHECK_INTERVAL 500

DEFINE_EVENT_TYPE(wxEVT_MONITOR_NOTIFY)

wxDirectoryMonitorEvent::wxDirectoryMonitorEvent(const wxString & mon_dir, int event_type, const wxString & uri): wxNotifyEvent(wxEVT_MONITOR_NOTIFY)
{
    m_mon_dir = mon_dir;
    m_event_type = event_type;
    m_info_uri = wxString(uri);
}

wxDirectoryMonitorEvent::wxDirectoryMonitorEvent(const wxDirectoryMonitorEvent & c) : wxNotifyEvent(c)
{
    m_mon_dir = wxString(c.m_mon_dir);
    m_event_type = c.m_event_type;
    m_info_uri = wxString(c.m_info_uri);
}

BEGIN_EVENT_TABLE(wxDirectoryMonitor, wxEvtHandler)
    EVT_MONITOR_NOTIFY(0, wxDirectoryMonitor::OnMonitorEvent)
END_EVENT_TABLE()

wxDirectoryMonitor::wxDirectoryMonitor(wxEvtHandler * parent, const wxArrayString & uri, int eventfilter): m_timer(NULL)
{
    m_parent = parent;
    m_uri = uri;
    m_eventfilter = eventfilter;
    Bind(wxEVT_TIMER, &wxDirectoryMonitor::OnTimer, this);
}

wxDirectoryMonitor::~wxDirectoryMonitor()
{
    Stop();
    Unbind(wxEVT_TIMER, &wxDirectoryMonitor::OnTimer, this);
}

void wxDirectoryMonitor::OnMonitorEvent(wxDirectoryMonitorEvent & e)
{
    if (m_parent)
    {
        m_parent->AddPendingEvent(e);
    }
}

void wxDirectoryMonitor::Start()
{
    Stop();
    m_timer = new wxTimer(this);
    m_timer->Start(FILE_CHECK_INTERVAL, true);
}

void wxDirectoryMonitor::Stop()
{
    if (m_timer)
    {
        m_timer->Stop();
    }

    wxDELETE(m_timer);
}

void wxDirectoryMonitor::Clear()
{
    Stop();
    m_files.clear();
}

void wxDirectoryMonitor::ChangePaths(const wxArrayString & uri)
{
    m_uri = uri;
    //    m_monitorthread->UpdatePaths(uri);
}

size_t wxDirectoryMonitor::GetFileSize(const wxFileName & filename)
{
    struct stat b;
    wxString file_name = filename.GetFullPath();
    const char * cfile = file_name.mb_str(wxConvUTF8).data();

    if (::stat(cfile, &b) == 0)
    {
        return b.st_size;
    }
    else
    {
        Manager::Get()->GetLogManager()->LogError(_("Failed to open file:")  +  file_name + "." + strerror(errno));
        return 0;
    }
}
time_t wxDirectoryMonitor::GetFileModificationTime(const wxFileName & filename)
{
    wxString file = filename.GetFullPath();
    struct stat buff;
    const wxCharBuffer cname = file.mb_str(wxConvUTF8);

    if (stat(cname.data(), &buff) < 0)
    {
        return 0;
    }

    return buff.st_mtime;
}

void wxDirectoryMonitor::OnTimer(wxTimerEvent & event)
{
    std::set<wxString> nonExistingFiles;
    std::for_each(m_files.begin(), m_files.end(), [&](const std::pair<wxString, wxDirectoryMonitor::File> & p)
    {
        const File & f = p.second;
        const wxFileName & fn = f.filename;

        if (!fn.Exists())
        {
            // fire file not found event
            //clFileSystemEvent evt(wxEVT_FILE_NOT_FOUND);
            //evt.SetPath(fn.GetFullPath());
            //GetOwner()->AddPendingEvent(evt);
            // add the missing file to a set
            nonExistingFiles.insert(fn.GetFullPath());
        }
        else
        {
#ifdef __WXMSW__
            size_t prev_value = f.file_size;
            size_t curr_value = GetFileSize(fn);
#else
            time_t prev_value = f.lastModified;
            time_t curr_value = GetFileModificationTime(fn);
#endif

            if (prev_value != curr_value)
            {
                wxDirectoryMonitorEvent e(fn.GetFullPath(), MONITOR_FILE_CHANGED, fn.GetName());
                m_parent->AddPendingEvent(e);
            }

#ifdef __WXMSW__
            File updatdFile = f;
            updatdFile.file_size = curr_value;
#else
            // Always update the last modified timestamp
            File updatdFile = f;
            updatdFile.lastModified = curr_value;
#endif
            m_files[fn.GetFullPath()] = updatdFile;
        }
    });
    // Remove the non existing files
    std::for_each(nonExistingFiles.begin(), nonExistingFiles.end(), [&](const wxString & fn)
    {
        m_files.erase(fn);
    });

    if (m_timer)
    {
        m_timer->Start(FILE_CHECK_INTERVAL, true);
    }
}

