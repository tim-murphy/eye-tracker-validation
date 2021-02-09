//////////////////////////////////////////////////////////////////////////////////////
// GPClient.h - Example class using Open Gaze API
//
// To the extent possible under law, the author(s) have dedicated all copyright 
// and related and neighboring rights to this software to the public domain worldwide. 
// This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this 
// software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>
#include <deque>
#include <mutex>

class GPClient
{
private:
  unsigned int _ip_port;
  std::string _ip_address;

  unsigned int _rx_buffer_size;
  std::deque <std::string> _rx_buffer;
  std::vector <std::string> _tx_buffer;

  std::mutex _rx_mutex;
  std::mutex _tx_mutex;
  volatile bool _thread_exit;
  static unsigned int GPClientThread (GPClient *ptr);

  bool _keep_all_data;
  bool _rx_status;
  bool _connected_status;

public:
  GPClient(void);
  ~GPClient(void);

  void set_address (std::string address) {_ip_address = address;} // set server IP address
  void set_port (unsigned int port) {_ip_port = port;} // set server IP port

  void client_connect ();
  void client_disconnect ();

  void send_cmd(std::string cmd);

  void set_rx_buffer_max(unsigned int max) {_rx_buffer_size = max;} // set maximum records to hold in internal buffer
  std::string get_rx_latest(); // get latest record and clear buffer
  void get_rx(std::deque <std::string> &data); // get all records and clear buffer
  bool get_rx_status(); // query if server has sent any data recently (connection may be closed from server side)?
  bool is_connected(); // query if connected to server
};
