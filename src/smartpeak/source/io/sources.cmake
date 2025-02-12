### the directory name
set(directory source/io)

### list all filenames of the directory here
set(sources_list
	CSVWriter.cpp
	ParametersParser.cpp
	SelectDilutionsParser.cpp
	SequenceParser.cpp
	SessionDB.cpp
	InputDataValidation.cpp
)

### add path to the filenames
set(sources)
foreach(i ${sources_list})
	list(APPEND sources ${directory}/${i})
endforeach(i)

### pass source file list to the upper instance
set(SmartPeak_sources ${SmartPeak_sources} ${sources})

### source group definition
source_group("Source Files\\io" FILES ${sources})

