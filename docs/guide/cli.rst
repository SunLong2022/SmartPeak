.. begin_smartpeak_cli_usage

Using SmartPeak CLI
----------------------------------------------------------------------------------------------------------

This page describes command line tool for working with SmartPeak. SmartPeak CLI provides an equivalent of SmartPeak GUI application, 
however with a possibility to run in headless mode.

.. image:: ../images/SmartPeakCLI.gif

.. note::
    Currently SmartPeak CLI does not generate any graphical files.

Load session
~~~~~~~~~~~~

SmartPeak CLI requires only one parameter ``load-session`` to start running a workflow. 
All other parameters are optional and can be specified to customize settings.
The parameter accepts a path to a session file (ex: ``session.db``) file, created from SmartPeakGUI.
It's also possible to give a session directory that would have a specific directory and file naming organisation as a parameter. This solution however should not be preferred. 
An An example below:

In Powershell

.. code-block:: powershell

    ./SmartPeakCLI --load-session C:/data/GCMS_SIM_Unknowns/session.db

.. code-block:: powershell

    ./SmartPeakCLI --load-session C:/data/GCMS_SIM_Unknowns

In Bash

.. code-block:: bash

    ./SmartPeakCLI --load-session $HOME/data/GCMS_SIM_Unknowns/session.db

.. code-block:: bash

    ./SmartPeakCLI --load-session $HOME/data/GCMS_SIM_Unknowns

Export reports
~~~~~~~~~~~~~~

After running a workflow, SmartPeak offers saving the result of the computation in concise csv files.
By default all possible reports will be exported, i.e. ``PivotTable`` and ``FeatureDB``. 
However user can decide which report type to save using ``--report`` option. 
It is still possible to specify both with this option, e.g. ``--report pivottable featuredb``.
If ``--output-features`` path is not given, the files are written to current working directory.

In Powershell

.. code-block:: powershell

    ./SmartPeakCLI
        --load-session C:/data/GCMS_SIM_Unknowns
        --report featuredb

In Bash

.. code-block:: bash

    ./SmartPeakCLI \
        --load-session $HOME/data/GCMS_SIM_Unknowns \
        --report featuredb

.. note::
    Notice that values for ``--report`` are case insensitive, therefore user can use ``featuredb`` as well as ``FeatureDB``.

Output directory
~~~~~~~~~~~~~~~~

In order to save files in specific location, set an ``--output-features`` path (relative or absolute), or --reports-out-dir for the reports directories.

In Powershell

.. code-block:: powershell

    ./SmartPeakCLI
        --load-session C:/data/GCMS_SIM_Unknowns
        --report featuredb
        --reports-out-dir $HOME/data/reports
        --output-features $HOME/data/output_features

In Bash

.. code-block:: bash

    ./SmartPeakCLI \
        --load-session $HOME/data/GCMS_SIM_Unknowns \
        --report featuredb \
        --reports-out-dir $HOME/data/reports
        --output-features $HOME/data/output_features

.. warning::
    Notice that if given a path to non-existing directory, SmartPeak will attempt to create one.
    In case of insufficient access permission or other potential errors, CLI tool will exit with status ``1`` before running the workflow.

Specify report sample types
~~~~~~~~~~~~~~~~~~~~~~~~~~~

By default SmartPeak includes all the sample types available in the report. The option ``--report-sample-types`` allows to reduce this set. 
Values are space separated list, case insensitive.

In Powershell

.. code-block:: powershell

    ./SmartPeakCLI
        --load-session C:/data/GCMS_SIM_Unknowns
        --report featuredb
        --report-sample-types standard blank qc
        --reports-out-dir $HOME/data/reports
        --output-features $HOME/data/output_features

In Bash

.. code-block:: bash

    ./SmartPeakCLI \
        --load-session $HOME/data/GCMS_SIM_Unknowns \
        --report featuredb \
        --report-sample-types standard blank qc \
        --reports-out-dir $HOME/data/reports \
        --output-features $HOME/data/output_features

.. note::
    List of all possible sample types can be found here: :ref:`sample-types`.

.. warning::
    SmartPeak validates values for the ``--report-sample-types`` option before running the workflow. 
    In case of incorrect value, the process exits with status ``1``.

Specify report meta-data
~~~~~~~~~~~~~~~~~~~~~~~~

Similarly to sample-types, the meta-data can also be listed as a command line option ``--report-metadata``. 
The same rules applies, an example below.

In Powershell

.. code-block:: powershell

    ./SmartPeakCLI
        --load-session C:/data/GCMS_SIM_Unknowns
        --report featuredb
        --report-metadata peak_area logSN RT
        --reports-out-dir $HOME/data/reports
        --output-features $HOME/data/output_features

In Bash

.. code-block:: bash

    ./SmartPeakCLI \
        --load-session $HOME/data/GCMS_SIM_Unknowns \
        --report featuredb \
        --report-metadata peak_area logSN RT \
        --reports-out-dir $HOME/data/reports \
        --output-features $HOME/data/output_features

.. note::
    List of all possible values can be found here: :ref:`metadata`.

.. warning::
    SmartPeak validates values for the ``--report-metadata`` option before running the workflow. 
    In case of incorrect value, the process exits with status ``1``.

Override workflow
~~~~~~~~~~~~~~~~~

The default workflow setting is loaded from the ``workflow.csv``, the file location is the same as ``sequence.csv``.
The CLI tool allows to modify the list of commands inline with an option ``--workflow``.

In Powershell

.. code-block:: powershell

    ./SmartPeakCLI
        --load-session C:/data/GCMS_SIM_Unknowns
        --report featuredb
        --report-metadata peak_area logSN RT
        --workflow  LOAD_RAW_DATA
                    MAP_CHROMATOGRAMS
                    EXTRACT_CHROMATOGRAM_WINDOWS
                    ZERO_CHROMATOGRAM_BASELINE
                    PICK_MRM_FEATURES
                    SELECT_FEATURES
                    STORE_FEATURES
        --reports-out-dir $HOME/data/reports
        --output-features $HOME/data/output_features

In Bash

.. code-block:: bash

    ./SmartPeakCLI \
        --load-session $HOME/data/GCMS_SIM_Unknowns \
        --report featuredb \
        --report-metadata peak_area logSN RT \
        --workflow  LOAD_RAW_DATA \
                    MAP_CHROMATOGRAMS \
                    EXTRACT_CHROMATOGRAM_WINDOWS \
                    ZERO_CHROMATOGRAM_BASELINE \
                    PICK_MRM_FEATURES \
                    SELECT_FEATURES \
                    STORE_FEATURES \
        --reports-out-dir $HOME/data/reports \
        --output-features $HOME/data/output_features

.. note::
    The full list of commands with short descriptions is included here: :ref:`workflow-commands`.

.. warning::
    SmartPeak validates values for the ``--workflow`` option before running the process.
    Note that values are case-insensitive. When incorrect value given, the error status ``1`` is returned.

Run integrity checks
~~~~~~~~~~~~~~~~~~~~

SmartPeak CLI allows to run several integrity checks before the workflow. 
By default no checs are performed, user can list them using ``--integrity`` option. 
If at least one verification fails, the process will exit with status code ``1``. 
However, user can modify this behaviour with ``--allow-inconsistent`` flag, 
which forces SmartPeak to continue with the workflow even if one or more integrity checks failed.

In Powershell

.. code-block:: powershell

    ./SmartPeakCLI
        --load-session C:/data/GCMS_SIM_Unknowns
        --report featuredb
        --integrity SAMPLE COMP_GROUP
        --allow-inconsistent
        --reports-out-dir $HOME/data/reports
        --output-features $HOME/data/output_features

In Bash

.. code-block:: bash

    ./SmartPeakCLI \
        --load-session $HOME/data/GCMS_SIM_Unknowns \
        --report featuredb \
        --integrity SAMPLE COMP_GROUP \
        --allow-inconsistent \
        --reports-out-dir $HOME/data/reports \
        --output-features $HOME/data/output_features

.. note::
    Refer to list of integrity checks here: :ref:`integrity-checks`.


Full List of CLI Parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

    Available parameters:

    -h    --help

    This parameter is optional. The default value is ''.

    -l    --load-session  (required)
    The path to a session file, or to a directory with a structured file organization (session file is preferred)

    -r    --report
    Specify a report type to export.
    This parameter is optional. The default value is '[ FeatureDB PivotTable ]'.

    -rt   --report-sample-types
    A list of sample types to include in the report. The full list can be found in the documentation https://smartpeak.readthedocs.io
    This parameter is optional. The default value is '[ ALL ]'.

    -rm   --report-metadata
    A list of metadata to include in the report. The full list can be found in the documentation https://smartpeak.readthedocs.io
    This parameter is optional. The default value is '[ ALL ]'.

    -ro   --reports-out-dir
    An absolute or relative path to an output directory for reports. Overrides the default location which is the current working directory. SmartPeak will create the given directory if one does not exist.
    This parameter is optional. The default value is '.'.

    -w    --workflow
    The workflow sequence as a list of commands, e.g. LOAD_DATA MAP_CHROMATOGRAMS ... Overrides the workflow settings loaded from the sequence file (with option --load-session)
    This parameter is optional. The default value is '[ ]'.

    -t    --integrity
    Specify which integrity checks to run, available are: SAMPLE, COMP, COMP_GROUP, IS and ALL (runs all listed).
    This parameter is optional. The default value is '[ NONE ]'.

    -a    --allow-inconsistent
    Given that any integrity checks were specified with '--integrity', this option allow to run workflow even if one or more check has failed.
    This parameter is optional. The default value is '0'.

    -v    --verbose
    Run SmartPeak in verbose mode, display more detailed information
    This parameter is optional. The default value is '0'.

    -d    --disable-colors
    By default the console output is colored, this flag disables colors.
    This parameter is optional. The default value is '0'.

    -pg   --disable-progressbar
    Progress bar allows tracks the progress of the entire workflow. This option disables the progress bar.
    This parameter is optional. The default value is '0'.

    -ld   --log-dir
    The path to the log directory. The given directory has to exist. Overrides the default location for the log file: https://smartpeak.readthedocs.io/en/latest/guide/guistart.html#logs
    This parameter is optional. The default value is ''.

    -o    --output-features
    An absolute or relative path to an output directory. Overrides the default location which is the current working directory. SmartPeak will create the given directory if one does not exist.
    This parameter is optional. The default value is './features'.

    -i    --input-features
    An absolute or relative path to the input features directory. Overrides the default location which is the current working directory.
    This parameter is optional. The default value is './features'.

    -z    --mzml
    An absolute or relative path to the mzML directory. Overrides the default location which is the mzML folder under the current working directory. SmartPeak will create given directory if one does not exist.
    This parameter is optional. The default value is './mzML'.

    -f    --input-file
    Override input file. Ex: -f featureQCComponents="./featureQCComponents_new.csv".
    This parameter is optional. The default value is '[ ]'.

    -p    --parameter
    Override parameter. Ex: '-p MRMFeatureFinderScoring:TransitionGroupPicker:peak_integration=smoothed'.
    This parameter is optional. The default value is '[ ]'.


Running SmartPeakCLI from a container
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can also pull the `smartpeak-cli <https://hub.docker.com/r/autoflowresearch/smartpeak-cli>`_ container and run workflows in it, to do so please use the following commands depending on your OS.

In Powershell

.. code-block:: powershell

    docker run --rm -ti -v C:/data:/sample-data autoflowresearch/smartpeak-cli:latest bash

In Bash

.. code-block:: bash

    docker run --rm -ti -v $HOME/data:/sample-data autoflowresearch/smartpeak-cli:latest bash

The previous commands will pull the required image locally, mounts ``C:/data`` or ``$HOME/data`` to ``/sample-data`` in the container and destroys the container once exited (please omit ``--rm`` if you don't wish to do so).

Inside the container you can execute ``SmartPeakCLI`` following the examples mentioned above.

.. end_smartpeak_cli_usage