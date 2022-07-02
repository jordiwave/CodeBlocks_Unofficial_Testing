##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=dbgcli
ConfigurationName      :=Debug
WorkspaceConfiguration :=Debug
WorkspacePath          :=D:/Andrew_Development/Work_InProgress/DAP_Debugger/eranif_dgbd
ProjectPath            :=D:/Andrew_Development/Work_InProgress/DAP_Debugger/eranif_dgbd/dbgcli
IntermediateDirectory  :=../build-$(WorkspaceConfiguration)/dbgcli
OutDir                 :=$(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=andrew
Date                   :=21/06/2022
CodeLitePath           :="C:/Program Files/CodeLite"
MakeDirCommand         :=mkdir
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputDirectory        :=D:/Andrew_Development/Work_InProgress/DAP_Debugger/eranif_dgbd/build-$(WorkspaceConfiguration)/bin
OutputFile             :=..\build-$(WorkspaceConfiguration)\bin\$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)WXUSINGDLL_DAP 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :=$(IntermediateDirectory)/ObjectsList.txt
PCHCompileFlags        :=
RcCmpOptions           := $(shell wx-config --rcflags)
RcCompilerName         :=windres
LinkOptions            :=  $(shell wx-config --libs) -mwindows
IncludePath            := $(IncludeSwitch)C:\msys64\mingw64\include\wx-3.1  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch).. 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)dapcxx $(LibrarySwitch)ws2_32 
ArLibs                 :=  "dapcxx" "ws2_32" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)$(WorkspacePath)/build-$(WorkspaceConfiguration)/lib 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := ar rcus
CXX      := g++
CC       := gcc
CXXFLAGS :=  -g -O0 -Wall $(shell wx-config --cflags) $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
PATH:=C:\msys64\mingw64\bin;$PATH
WXWIN:=D:\Andrew_Development\Libraries\wxWidgets-3.1.7_win64
WXCFG:=gcc_dll\mswu
Objects0=$(IntermediateDirectory)/UI.cpp$(ObjectSuffix) $(IntermediateDirectory)/MainFrame.cpp$(ObjectSuffix) $(IntermediateDirectory)/resources.rc$(ObjectSuffix) $(IntermediateDirectory)/UI_dbgcli_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/ConsoleApp.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: MakeIntermediateDirs $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"
	@if not exist "$(OutputDirectory)" $(MakeDirCommand) "$(OutputDirectory)"

$(IntermediateDirectory)/.d:
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/UI.cpp$(ObjectSuffix): UI.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Andrew_Development/Work_InProgress/DAP_Debugger/eranif_dgbd/dbgcli/UI.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/UI.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/UI.cpp$(PreprocessSuffix): UI.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/UI.cpp$(PreprocessSuffix) UI.cpp

$(IntermediateDirectory)/MainFrame.cpp$(ObjectSuffix): MainFrame.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Andrew_Development/Work_InProgress/DAP_Debugger/eranif_dgbd/dbgcli/MainFrame.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MainFrame.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/MainFrame.cpp$(PreprocessSuffix): MainFrame.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/MainFrame.cpp$(PreprocessSuffix) MainFrame.cpp

$(IntermediateDirectory)/resources.rc$(ObjectSuffix): resources.rc
	$(RcCompilerName) -i "D:/Andrew_Development/Work_InProgress/DAP_Debugger/eranif_dgbd/dbgcli/resources.rc" $(RcCmpOptions)   $(ObjectSwitch)$(IntermediateDirectory)/resources.rc$(ObjectSuffix) $(RcIncludePath)
$(IntermediateDirectory)/UI_dbgcli_bitmaps.cpp$(ObjectSuffix): UI_dbgcli_bitmaps.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Andrew_Development/Work_InProgress/DAP_Debugger/eranif_dgbd/dbgcli/UI_dbgcli_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/UI_dbgcli_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/UI_dbgcli_bitmaps.cpp$(PreprocessSuffix): UI_dbgcli_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/UI_dbgcli_bitmaps.cpp$(PreprocessSuffix) UI_dbgcli_bitmaps.cpp

$(IntermediateDirectory)/ConsoleApp.cpp$(ObjectSuffix): ConsoleApp.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Andrew_Development/Work_InProgress/DAP_Debugger/eranif_dgbd/dbgcli/ConsoleApp.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ConsoleApp.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ConsoleApp.cpp$(PreprocessSuffix): ConsoleApp.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ConsoleApp.cpp$(PreprocessSuffix) ConsoleApp.cpp

##
## Clean
##
clean:
	$(RM) -r $(IntermediateDirectory)


