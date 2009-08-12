 // This is the header.
 // The following are message definitions.
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_SYSTEM                  0x0
#define FACILITY_STUBS                   0x3
#define FACILITY_RUNTIME                 0x2
#define FACILITY_IO_ERROR_CODE           0x4


//
// Define the severity codes
//
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: MSG_CANNOT_CREATE_SOCKET
//
// MessageText:
//
//  Cannot create socket, JaxerManager might already be running.
//
#define MSG_CANNOT_CREATE_SOCKET         ((DWORD)0xC0020001L)

//
// MessageId: MSG_CANNOT_BIND_PORT
//
// MessageText:
//
//  Cannot bind port, JaxerManager might already be running.
//
#define MSG_CANNOT_BIND_PORT             ((DWORD)0xC0020002L)

//
// MessageId: MSG_CANNOT_LISTEN_PORT
//
// MessageText:
//
//  Cannot listen on port, JaxerManager might already be running.
//
#define MSG_CANNOT_LISTEN_PORT           ((DWORD)0xC0020003L)

//
// MessageId: MSG_CANNOT_ACCEPT_SOCKET
//
// MessageText:
//
//  Cannot accept socket, make sure another JaxerManager is not already running.
//
#define MSG_CANNOT_ACCEPT_SOCKET         ((DWORD)0xC0020004L)

//
// MessageId: MSG_CANNOT_CREATE_PIPE
//
// MessageText:
//
//  Unable to create a named pipe.
//
#define MSG_CANNOT_CREATE_PIPE           ((DWORD)0xC0020005L)

//
// MessageId: MSG_CANNOT_CONNECT_PIPE
//
// MessageText:
//
//  Unable to connect the named pipe.
//
#define MSG_CANNOT_CONNECT_PIPE          ((DWORD)0xC0020006L)

//
// MessageId: MSG_CANNOT_CREATE_THREAD
//
// MessageText:
//
//  Thread creation failed.
//
#define MSG_CANNOT_CREATE_THREAD         ((DWORD)0xC0020007L)

//
// MessageId: MSG_CANNOT_INIT_LOG
//
// MessageText:
//
//  Init LogClient failed.
//
#define MSG_CANNOT_INIT_LOG              ((DWORD)0xC0020008L)

//
// MessageId: MSG_CANNOT_INIT_SOCKET
//
// MessageText:
//
//  Socket initialization failed, make sure another JaxerManager is not already running.
//
#define MSG_CANNOT_INIT_SOCKET           ((DWORD)0xC0020009L)

//
// MessageId: MSG_CANNOT_DUP_HANDLE
//
// MessageText:
//
//  DuplicateHandle failed.
//
#define MSG_CANNOT_DUP_HANDLE            ((DWORD)0xC002000AL)

//
// MessageId: MSG_CANNOT_CREATE_LOG_SERVER
//
// MessageText:
//
//  Failed to create log server.
//
#define MSG_CANNOT_CREATE_LOG_SERVER     ((DWORD)0xC002000BL)

//
// MessageId: MSG_CANNOT_COMMUNICATE_LOG_SERVER
//
// MessageText:
//
//  Failed to communicate with log server.
//
#define MSG_CANNOT_COMMUNICATE_LOG_SERVER ((DWORD)0xC002000CL)

//
// MessageId: MSG_CANNOT_CREATE_WAITABLE_TIMER
//
// MessageText:
//
//  Create waitable timer failure.
//
#define MSG_CANNOT_CREATE_WAITABLE_TIMER ((DWORD)0xC002000DL)

//
// MessageId: MSG_CANNOT_SET_WAITABLE_TIMER
//
// MessageText:
//
//  Set waitable timer failure.
//
#define MSG_CANNOT_SET_WAITABLE_TIMER    ((DWORD)0xC002000EL)

//
// MessageId: MSG_CANNOT_INSTALL_SERVICE
//
// MessageText:
//
//  Install service failure: %1
//
#define MSG_CANNOT_INSTALL_SERVICE       ((DWORD)0xC002000FL)

//
// MessageId: MSG_CANNOT_UNINSTALL_SERVICE
//
// MessageText:
//
//  Uninstall service failure: %1
//
#define MSG_CANNOT_UNINSTALL_SERVICE     ((DWORD)0xC0020010L)

//
// MessageId: MSG_CANNOT_START_SERVICE
//
// MessageText:
//
//  Start service failure: %1
//
#define MSG_CANNOT_START_SERVICE         ((DWORD)0xC0020011L)

//
// MessageId: MSG_CANNOT_STOP_SERVICE
//
// MessageText:
//
//  Stop service failure: %1
//
#define MSG_CANNOT_STOP_SERVICE          ((DWORD)0xC0020012L)

//
// MessageId: MSG_SERVICE_INSTALLED
//
// MessageText:
//
//  Successfully installed service %1.
//
#define MSG_SERVICE_INSTALLED            ((DWORD)0x40020013L)

//
// MessageId: MSG_SERVICE_UNINSTALLED
//
// MessageText:
//
//  Successfully uninstalled service %1.
//
#define MSG_SERVICE_UNINSTALLED          ((DWORD)0x40020014L)

//
// MessageId: MSG_SERVICE_STARTED
//
// MessageText:
//
//  Started service %1.
//
#define MSG_SERVICE_STARTED              ((DWORD)0x40020015L)

//
// MessageId: MSG_SERVICE_STOPPED
//
// MessageText:
//
//  Stopped service %1.
//
#define MSG_SERVICE_STOPPED              ((DWORD)0x40020016L)

