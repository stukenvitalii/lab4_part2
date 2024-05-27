//
// Created by stukenvitalii on 27.05.2024.
//
#include <windows.h>
#include <iostream>

// Define buffer size and pipe name
const size_t BUFFER_SIZE = 1024;
const std::string PIPE_NAME(R"(\\.\pipe\lab4)");
const char* EXIT_STR = "stop";

// Define callback function for file I/O completion
size_t callback;
void CALLBACK FileIOCompletionRoutine(
        DWORD dwErrorCode,
        DWORD dwNumberOfBytesTransfered,
        LPOVERLAPPED lpOverlapped)
{
    ++callback;
}

int main()
{
    // Wait for the pipe to be available
    WaitNamedPipeA(
            PIPE_NAME.c_str(),
            NMPWAIT_WAIT_FOREVER);

    // Open the pipe for reading
    HANDLE hPipe = CreateFileA(
            PIPE_NAME.c_str(),
            GENERIC_READ,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
            nullptr);

    if(hPipe != INVALID_HANDLE_VALUE)
    {
        OVERLAPPED over;
        size_t offset_i = 0;
        over.Offset = offset_i;
        over.OffsetHigh = offset_i >> 31;

        char buffer[BUFFER_SIZE];
        buffer[0] = '\0';

        // Read from the pipe until "stop" is received
        while(strcmp(buffer, EXIT_STR) != 0)
        {
            callback = 0;

            ZeroMemory(buffer, BUFFER_SIZE);

            // Read from the pipe asynchronously
            ReadFileEx(
                    hPipe,
                    buffer,
                    BUFFER_SIZE,
                    &over,
                    FileIOCompletionRoutine
            );

            // Wait for the read operation to complete
            SleepEx(-1, TRUE);

            std::cout << "Message: \"" << buffer << "\". " <<  std::endl;
        }

        // Close the pipe handle
        CloseHandle(hPipe);
    }
    else
    {
        std::cout << "Error: Can't connect to the pipe." << std::endl;
        return GetLastError();
    }

    return 0;
}