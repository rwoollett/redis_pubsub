<h1 align="center">Redis PubSub Implementation with Boost 1.86.0</h1>

<br />
The sample Redis PubSub application runs as standalone exe on a machine. 
Requires boost_1_86_0 (maybe 1_85 at minimum for redis boost)
<br />

# 🚀 Available Scripts

In the project directory, you can build the Application with CMake

<br />

Use current folder as: ~/redisnet (Project root folder)
```
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_PUBSUB_TESTS=on -DCMAKE_INSTALL_PREFIX=/usr/local -G "Unix Makefiles" . -B ./build
cmake --build build --target all
```

## 🧪 test

No tests implemented.

```
cmake --build build --target test

```
Launches the test runner.

```
docker compose up -d
```

<br />

## 🧪 Containment Docker image
Minikube env docker do use command:

```
eval $(minikube docker-env)
```

```
docker build -t redisnet:v1.0 -f Dockerfile .

docker run --network="host" --env REDIS_HOST=0.0.0.0 --env REDIS_PORT=6379 --env REDIS_CHANNEL=csToken_request,csToken_acquire --env REDIS_PASSWORD=<password>  -w /usr/src redisnet:v1.0

```

<br />

# 🧬 Project structure

This is the structure of the files in the project:

```sh
    │
    ├── clientPublisher        # ClientPublish application
    │   ├── CMakeLists.txt
    │   └── main.cpp          
    ├── clientSubscriber       # Subscriber application
    │   ├── AwakenerWaitable.h
    │   ├── CMakeLists.txt
    │   └── main.cpp
    ├── cmake                  # cmake scripts (3.13)
    ├── nmtoken_runner         # folder for nm_go.bat working directory
    ├── pubsub
    │   ├── publish            # Publish subjects
    │   │   ├── CMakeLists.txt
    │   │   └── *.cpp/*.h      # code
    │   └── subscribe          # Subcribe to subjects
    │       ├── CMakeLists.txt
    │       └── *.cpp/*.h      # code
    ├── .dockerignore
    ├── .gitignore
    ├── CMakeLists.txt         # Main CMake file
    ├── docker-compose.yaml
    ├── Dockerfile
    ├── Dockerfile.client
    ├── INSTALL.txt       
    ├── LICENCE.txt
    ├── README.md              # This README.md document
    ├── redispubsub_go.sh      # Scripts
    └── redispubsub_stop.sh
 
```

 ┌──────────────────────────┐
 │        Idle (start)      │
 └──────────────┬───────────┘
                │
                │ create connection object once
                ▼
        ┌──────────────────────┐
        │     Connecting       │
        └──────────┬──────────┘
                   │ async_run() started
                   │
                   ▼
        ┌──────────────────────┐
        │   Authenticating     │
        └──────────┬──────────┘
                   │ HELLO / AUTH / CLIENT SETNAME
                   │ (Boost.Redis does this internally)
                   ▼
        ┌──────────────────────┐
        │        Ready         │
        └──────────┬──────────┘
                   │
                   │ normal operation:
                   │   - async_exec(PUBLISH)
                   │   - health checks
                   │   - worker thread pushes messages
                   │
                   ▼
        ┌──────────────────────┐
        │        Broken        │
        └──────────┬──────────┘
                   │ triggered by:
                   │   - publish timeout
                   │   - async_exec error
                   │   - async_run handler firing
                   │   - startup PING failure
                   │
                   ▼
        ┌──────────────────────┐
        │     Reconnecting     │
        └──────────┬──────────┘
                   │
                   │ wait CONNECTION_RETRY_DELAY
                   │ cancel + reset_stream
                   │
                   ▼
        ┌──────────────────────┐
        │      Connecting      │  (loop)
        └──────────────────────┘
