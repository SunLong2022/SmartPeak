### the directory name
set(directory source/ui)

### list all filenames of the directory here
set(sources_list
	AboutWidget.cpp
	CalibratorsPlotWidget.cpp
	ChromatogramPlotWidget.cpp
	FilePicker.cpp
	GuiAppender.cpp
	Heatmap2DWidget.cpp
	InfoWidget.cpp
	LogWidget.cpp
	ParameterEditorWidget.cpp
	ParametersTableWidget.cpp
	Report.cpp
	RunWorkflowWidget.cpp
	SpectraPlotWidget.cpp
	StatisticsWidget.cpp
	Widget.cpp
	ImEntry.cpp
	WindowSizesAndPositions.cpp
	Workflow.cpp
	WorkflowStepWidget.cpp
)

### add path to the filenames
set(sources)
foreach(i ${sources_list})
	list(APPEND sources ${directory}/${i})
endforeach(i)

### pass source file list to the upper instance
set(SmartPeak_sources ${SmartPeak_sources} ${sources})

### source group definition
source_group("Source Files\\ui" FILES ${sources})

