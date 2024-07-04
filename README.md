# MyRelay

A simple DNS relay server implemented in C using UDP sockets, deployable on **Unix or Unix-like** machines.

<img src="https://typora-1313035735.cos.ap-nanjing.myqcloud.com/img/2024-07-04-171313.png" style="zoom:50%;" />

<img src="https://typora-1313035735.cos.ap-nanjing.myqcloud.com/img/2024-07-04-172610.png" style="zoom:50%;" />

#### How to start

##### Compiling

```shell
cd src && make
```

##### Running

```shell
cd ../bin
```

```shell
./myrelay # or ./myrelay <dns-ip-addr>, google DNS(8.8.8.8) is default
```

<img src="https://typora-1313035735.cos.ap-nanjing.myqcloud.com/img/2024-07-04-165410.png" style="zoom:80%;" />

#### Functionality

- [x] **Fast Caching Mechanism**: The DNS relay offers a rapid caching system to efficiently handle DNS requests, ensuring quick responses.
- [x] **Customizable Responses**: By modifying the `knownhosts.txt` file, users can control DNS responses, including assigning `0.0.0.0` to domains on a blacklist.
- [x] **Clear Logging**: The DNS relay provides detailed and clear logs for monitoring and troubleshooting.
- [x] **Low Resource Consumption**: The system is designed to have low CPU and memory usage, ensuring minimal impact on system performance.
- [ ] **Concurrency Support**: The DNS relay is built to support concurrent processing, allowing it to handle multiple DNS requests simultaneously.

```mermaid
flowchart
    A[Start] --> B["DNS Client (Resolver)"]
    B --> C{DNS Relay}
    C -->|Check local database| D["Check knownhosts.txt"]
    D -- Domain exists --> E["Load IP from knownhosts.txt"]
    E -->|Check IP| F{IP Address}
    F -- IP is 0.0.0.0 --> G["Return: No Such Domain (RCode=0011 NameError_ResponseCode)"]
    F -- IP is not 0.0.0.0 --> H["Return IP Address"]
    D -- Domain not exists --> I[Check Cache]
    I -- Cache hit --> J["Get IP from Cache"]
    J --> H
    I -- Cache miss --> K[Forward Query to DNS Server]
    K --> L[DNS Server]
    L --> M["Return Response to DNS Relay"]
    M --> N["Check Response Code"]
    N -- NoError_ResponseCode --> O["Record in Cache if new IP"]
    O -->|Record IP| P["Update Cache with new IP"]
    N -- Other Response Codes --> Q["Handle Error (e.g., FormatError, ServerFailure)"]
    P --> C
    Q --> C
    M --> R["Forward Response to Resolver"]
    R --> S[End]
```

