//
// Created by stukenvitalii on 27.05.2024.
//
#include <windows.h>
#include <iostream>

// Define buffer size and pipe name
const size_t BUFFER_SIZE = 1024;
const std::string PIPE_NAME(R"(\\.\pipe\lab4)");
const char* EXIT_STR = "stop";

int main()
{
    size_t i;

    // Create a named pipe with specified attributes
    HANDLE hPipe = CreateNamedPipeA(
            PIPE_NAME.c_str(),
            PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED | WRITE_DAC,
            PIPE_TYPE_MESSAGE | PIPE_WAIT,
            1,
            0,
            0,
            0,
            nullptr
    );

    if(hPipe != INVALID_HANDLE_VALUE)
    {
        // Connect to the named pipe
        if(ConnectNamedPipe(hPipe, nullptr))
        {
            OVERLAPPED over;
            over.hEvent = CreateEvent(
                    nullptr,
                    false,
                    false,
                    nullptr);

            char buffer[BUFFER_SIZE];
            std::string string_buffer;

            // Send messages until "stop" is entered
            while(strcmp(buffer, EXIT_STR) != 0)
            {
                ZeroMemory(buffer, 0);
                std::cout << "Enter message (" << EXIT_STR << " to exit): ";
                getline(std::cin, string_buffer);

                // Check if message length exceeds buffer size
                if(string_buffer.length() - 1 > BUFFER_SIZE)
                {
                    std::cout << "Error: Message more than " << BUFFER_SIZE << " bytes." << std::endl;
                    continue;
                }
                else
                {
                    // Copy message to buffer
                    for(i = 0; i < string_buffer.length(); ++i)
                        buffer[i] = string_buffer[i];
                    buffer[i] = '\0';
                }

                // Write message to the pipe
                WriteFile(hPipe, buffer, strlen(buffer) + 1, nullptr, &over);
                WaitForSingleObject(over.hEvent, INFINITE);
                std::cout << "Message was sent!" << std::endl;
            }

            // Disconnect from the pipe and close event handle
            DisconnectNamedPipe(hPipe);
            CloseHandle(over.hEvent);
        }
        else
        {
            std::cout << "Error: Can't connect to pipe." << std::endl;
            return GetLastError();
        }

        // Close pipe handle
        CloseHandle(hPipe);
    }
    else
    {
        std::cout << "Error: Can't create pipe." << std::endl;
        return GetLastError();
    }

    return 0;
}