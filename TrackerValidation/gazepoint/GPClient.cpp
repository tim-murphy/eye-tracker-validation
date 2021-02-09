//////////////////////////////////////////////////////////////////////////////////////
// GPClient.cpp - Example class using Open Gaze API
//
// To the extent possible under law, the author(s) have dedicated all copyright 
// and related and neighboring rights to this software to the public domain worldwide. 
// This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this 
// software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//////////////////////////////////////////////////////////////////////////////////////

#include "GPClient.h"

#include <thread>
#include <chrono>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #define TRUE true
    #define FALSE false

    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #define SOCKADDR_IN sockaddr_in
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif

#define RX_TCP_BUFFER_MAX 64000

#pragma comment(lib, "Ws2_32.lib")

unsigned long getTickCount()
{
    return static_cast<unsigned long>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
}

GPClient::GPClient(void)
{ 
  _ip_port = 4242;
  _ip_address = "127.0.0.1"; 

  // By default keep 60 records/sec * 60 seconds * 3 minutes of data
  // If we kept ALL records but never read them we could run out of memory
  _rx_buffer_size = 60*60*3;
  _rx_status = false;
  _connected_status = false;
}

GPClient::~GPClient(void)
{
  client_disconnect();
}

void GPClient::client_connect ()
{
  _rx_mutex.lock();
  _tx_buffer.clear();
  _rx_buffer.clear();
  _rx_mutex.unlock();

  std::thread t (GPClientThread, this);
  t.detach();

  std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
  
void GPClient::client_disconnect ()
{
  _rx_mutex.lock();
  _thread_exit = TRUE;
  _rx_mutex.unlock();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  _rx_mutex.lock();
  _rx_buffer.clear();
  _tx_buffer.clear(); 
  _rx_mutex.unlock();
}

unsigned int GPClient::GPClientThread (GPClient *ptr)
{
  unsigned int result;
  unsigned int delimiter_index;
  std::string rxstr;
  char rxbuffer [RX_TCP_BUFFER_MAX];
  // int state = 0; FIXME this is never used
  unsigned long rx_time = getTickCount();

  SOCKADDR_IN addr;

  SOCKET ipsocket;
  u_long poll = TRUE;

#ifdef _WIN32
  WSADATA wsadata; 
  if (WSAStartup(0x0101, &wsadata)) 
  {
    return 0;
  }
#endif // note: the linux equivalent of this is a no-op
  
  ipsocket = socket(AF_INET, SOCK_STREAM, 0);
  if (ipsocket == INVALID_SOCKET) 
  {
    return 0;
  }
  
#ifdef _WIN32
  if (ioctlsocket(ipsocket, FIONBIO, &poll) == SOCKET_ERROR) 
#else
  if (ioctl(ipsocket, FIONBIO, &poll) == SOCKET_ERROR) 
#endif
  {
    return 0;
  }
  
  addr.sin_family = AF_INET;
  addr.sin_port = htons(ptr->_ip_port);

#ifdef _WIN32
  addr.sin_addr.S_un.S_addr
#else
  addr.sin_addr.s_addr
#endif
    = inet_addr(ptr->_ip_address.c_str());

  connect(ipsocket, (struct sockaddr*)&addr, sizeof (addr));
  
  ptr->_thread_exit = FALSE;

  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  ptr->_rx_status = FALSE;
  ptr->_connected_status = TRUE;

  do
  {
    auto end_time = std::chrono::system_clock::now() + std::chrono::milliseconds(5);

    // if data is not received within last 4 second then rx is not active
    if (getTickCount() > rx_time + 4000)
    {
      ptr->_rx_status = FALSE;
    }

    if (ipsocket != 0 && ptr->_thread_exit != TRUE)
    {
    do
    {
        result = recv(ipsocket, rxbuffer, RX_TCP_BUFFER_MAX, 0);

        // FIXME this is never used
        // if (result == SOCKET_ERROR)
        // {
            // state = WSAGetLastError();
        // }
        // else
        if (result > 0)
        {
            rx_time = getTickCount();
            ptr->_rx_status = TRUE;

            // save room to add a NULL character to incoming char stream
            if (result > RX_TCP_BUFFER_MAX-1) 
{
                result = RX_TCP_BUFFER_MAX-1;
            }

            rxbuffer[result] = '\0';
            rxstr = rxstr + rxbuffer;

            // find end of record delimiter
            delimiter_index = static_cast<unsigned int>(rxstr.find ("\r\n", 0));

ptr->_rx_mutex.lock();
            while (delimiter_index != std::string::npos && rxstr != "")
            {
                std::string tmp = rxstr.substr(0, delimiter_index);

                // save record at head of queue (FIFO)
                ptr->_rx_buffer.push_back(tmp);

                // remove records longer than queue size (so we don't run out of memory)
                while (ptr->_rx_buffer.size() > ptr->_rx_buffer_size)
                {
                    ptr->_rx_buffer.pop_front();
                }

rxstr.erase(0, delimiter_index+2);
                delimiter_index = static_cast<unsigned int>(rxstr.find ("\r\n", 0));
            }
          ptr->_rx_mutex.unlock();
        }
      }
      while (result > 0 && result != SOCKET_ERROR && ptr->_thread_exit  != TRUE);

      ptr->_tx_mutex.lock();
      for (unsigned int i = 0; i < ptr->_tx_buffer.size(); i++)
      {
        send(ipsocket, ptr->_tx_buffer.at(i).c_str(), static_cast<int>(ptr->_tx_buffer.at(i).size()), 0);
      }      
      ptr->_tx_buffer.clear();
      ptr->_tx_mutex.unlock();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  while (ptr->_thread_exit  != TRUE);

#ifdef _WIN32
  closesocket(ipsocket);
  WSACleanup();
#else
  close(ipsocket);
#endif

  ptr->_connected_status = FALSE;

   return TRUE;
}

void GPClient::send_cmd(std::string cmd)
{
  cmd = cmd + "\r\n";
 
  _rx_mutex.lock();
  _tx_buffer.push_back (cmd);
  _rx_mutex.unlock();
}

std::string GPClient::get_rx_latest()
{
  std::string tmp;
  
  _rx_mutex.lock();
  if (_rx_buffer.size() > 0)
  {
    tmp = _rx_buffer.back();
    _rx_buffer.clear();
  }
  _rx_mutex.unlock();

  return tmp;
}

void GPClient::get_rx(std::deque <std::string> &data)
{
  _rx_mutex.lock();
  data.clear();
  _rx_buffer.swap(data);
  _rx_mutex.unlock();
}

bool GPClient::get_rx_status()
{
  return _rx_status;
}

bool GPClient::is_connected()
{
  return _connected_status;
}
