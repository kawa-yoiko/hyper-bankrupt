/* HOW TO RUN
   1) Configure things in the Configuration class
   2) Compile: g++ -o bot.exe bot.cpp
   3) Run in loop: while true; do ./bot.exe; sleep 1; done
*/

/* C includes for networking things */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

/* C++ includes */
#include <string>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <sstream>

#include "state.h"

/* The Configuration class is used to tell the bot how to connect
   to the appropriate exchange. The `test_exchange_index` variable
   only changes the Configuration when `test_mode` is set to `true`.
*/
class Configuration {
private:
  /*
    0 = prod-like
    1 = slower
    2 = empty
  */
  static int const test_exchange_index = 0;
public:
  std::string team_name;
  std::string exchange_hostname;
  int exchange_port;
  /* replace REPLACEME with your team name! */
  Configuration(bool test_mode) : team_name("drax"){
    exchange_port = 20000; /* Default text based port */
    if(true == test_mode) {
      exchange_hostname = "test-exch-" + team_name;
      exchange_port += test_exchange_index;
    } else {
      exchange_hostname = "production";
    }
  }
};

/* Connection establishes a read/write connection to the exchange,
   and facilitates communication with it */
class Connection {
private:
  FILE * in;
  FILE * out;
  int socket_fd;
public:
  Connection(Configuration configuration){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
      throw std::runtime_error("Could not create socket");
    }
    std::string hostname = configuration.exchange_hostname;
    hostent *record = gethostbyname(hostname.c_str());
    if(!record) {
      throw std::invalid_argument("Could not resolve host '" + hostname + "'");
    }
    in_addr *address = reinterpret_cast<in_addr *>(record->h_addr);
    std::string ip_address = inet_ntoa(*address);
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ip_address.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(configuration.exchange_port);

    int res = connect(sock, ((struct sockaddr *) &server), sizeof(server));
    if (res < 0) {
      throw std::runtime_error("could not connect");
    }
    FILE *exchange_in = fdopen(sock, "r");
    if (exchange_in == NULL){
      throw std::runtime_error("could not open socket for writing");
    }
    FILE *exchange_out = fdopen(sock, "w");
    if (exchange_out == NULL){
      throw std::runtime_error("could not open socket for reading");
    }

    setlinebuf(exchange_in);
    setlinebuf(exchange_out);
    this->in = exchange_in;
    this->out = exchange_out;
    this->socket_fd = res;
  }

  /** Send a string to the server */
  void send_to_exchange(const char *input) {
    //std::string line(input);
    /* All messages must always be uppercase */
    //std::transform(line.begin(), line.end(), line.begin(), ::toupper);
    int res = fprintf(this->out, "%s\n", input);
    if (res < 0) {
      throw std::runtime_error("error sending to exchange");
    }
  }

  /** Read a line from the server, dropping the newline at the end */
  std::string read_from_exchange()
  {
    /* We assume that no message from the exchange is longer
       than 10,000 chars */
    const size_t len = 10000;
    char buf[len];
    if(!fgets(buf, len, this->in)){
      //throw std::runtime_error("reading line from socket");
      return "";
    }

    int read_length = strlen(buf);
    std::string result(buf);
    /* Chop off the newline */
    result.resize(result.length() - 1);
    return result;
  }
};

/** Join a vector of strings together, with a separator in-between
    each string. This is useful for space-separating things */
std::string join(std::string sep, std::vector<std::string> strs) {
  std::ostringstream stream;
  const int size = strs.size();
  for(int i = 0; i < size; ++i) {
    stream << strs[i];
    if(i != (strs.size() - 1)) {
      stream << sep;
    }
  }
  return stream.str();
}


void run()
{
    // Be very careful with this boolean! It switches between test and prod
#ifdef PROD
    bool test_mode = false;
#else
    bool test_mode = true;
#endif
    Configuration config(test_mode);
    Connection conn(config);

    std::vector<std::string> data;
    /*
      A common mistake people make is to conn.send_to_exchange() > 1
      time for every conn.read_from_exchange() response.
      Since many write messages generate marketdata, this will cause an
      exponential explosion in pending messages. Please, don't do that!
    */
    conn.send_to_exchange("HELLO DRAX");

    state st;
    st.send_callback = [&conn] (const char *s) {
        printf(">> %s\n", s);
        conn.send_to_exchange(s);
    };
    while (1) {
        std::string line = conn.read_from_exchange();
        //printf("<< %s\n", line.c_str());
        if (line.empty()) break;
        st.handle(line);
    }
}

void run_local()
{
    printf("HELLO DRAX\n");
    std::string s;
    state st;
    st.send_callback = [] (const char *s) {
        printf(">> %s\n", s);
    };
    while (1) {
        std::getline(std::cin, s);
        //printf("<< %s\n", s.c_str());
        if (s.empty()) break;
        st.handle(s);
    }
}

int main(int argc, char *argv[])
{
#ifdef LOCAL
    run_local();
#else
    run();
#endif
    return 0;
}
