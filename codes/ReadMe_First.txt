About the Demo Project
--------------------------------------------------------------------------
The application blinks the user-selected LED at a user-selected frequency.

* To select the LED press the left button
* To select the frequency press the right button.
* The UART outputs the application status at 115,200 baud, 8-n-1 mode.

This application utilizes FreeRTOS to perform the tasks in a concurrent fashion.
The following tasks are created:

* An LED task, which blinks the user-selected on-board LED at a user-selected rate (changed via the buttons).
* A Switch task, which monitors the buttons pressed and passes the information to LED task.

In addition to the tasks, this application also uses the following FreeRTOS resources:

* A Queue to enable information transfer between tasks.
* A Semaphore to guard the resource, UART, from access by multiple tasks at the same time.
* A non-blocking FreeRTOS Delay to put the tasks in blocked state when they have nothing to do.
-----------------------------------------------------------------------------------------------
How to use the Demo project (freertos_on_tm4c123.tar.gz)
-----------------------------------------------------------------------------------------------
(a) Download freertos_on_tm4c123.tar.gz project from http://shukra.dese.iisc.ernet.in/emsys/rtos/freertos/      freertos_on_tm4c123.tar.gz
(b) untar freertos_on_tm4c123.tar.gz
(c) Start CCS7 with a new workspace
(d) From the CCS File menu, Select Import --> Code Composer Studio --> CCS Projects
(e) Select freertos_on_tm4c123 folder
(f) Create TIVAWARE_LOC Build Variable (Window --> Preferences --> Code Composer Studio --> Build --> Variables)
    (http://shukra.dese.iisc.ernet.in/edwiki/EmSys:Create_a_variable_at_workspace_level)
(g) Clean Project (Project --> Clean...)
(h) Build Project (http://shukra.dese.iisc.ernet.in/edwiki/EmSys:Build_FreeRTOS_Demo_Project)
(e) Configure Debug (http://shukra.dese.iisc.ernet.in/edwiki/EmSys:FreeRTOS_Demo_Project_Debug_Configuration)
(h) Load and Run the Demo Project (http://shukra.dese.iisc.ernet.in/edwiki/EmSys:Build_Load_Run_FreeRTOS_Demo_Project)
---------------------------------------------------------------------------------------------------------------------
Don't hesiate to contact us!
--------------------------------------------------------------------
Thanks and Best regards.

--JS

