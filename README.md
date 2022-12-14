# C++ HTML server
## Compile
```
g++ src/*.cpp -o server -std=c+=20
```

## Features
1. Support sending text/html, text/css, text/js, text/plain, image/jpg, image/png
2. Currently only support GET request. But can easily extend to parse other requests like POST DEL
3. Epolling
File descriptors for each client connection is set to non-blocking. They are only served when ready

3. Thread pool
Create a thread pool. For each incoming request, one idle thread will be assigned to handle the request. After completing, the thread is put back into the pool.

4. Producer-Consumer model
As server receives request, they are placed into a queue. Idle threads takes out the requests out from the queue.
