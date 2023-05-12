# TCPclient-server
this is basic server-client program that uses TCP, implemented using POSIX standart


to compile
  ```
  cmake -s . -B build
  cd build
  make
  ```
  
  execution example from build dir
   ```
   ./server 5000
   ```
   another terminal page
   ```
   ./client 127.0.0.1 5000
   ```
