# Simple C Thread Pool

This is a simple thread pool implementation in C that provides a convenient way to manage and distribute tasks across multiple threads. It can be used to parallelize tasks and improve the overall performance of your applications.

## Table of Contents

- [Features](#features)
- [Usage](#usage)
  - [Building](#building)
  - [Example](#example)
- [File Structure](#file-structure)
- [Contributing](#contributing)
- [License](#license)

## Features

- **Thread Pool**: The core of this project is the thread pool implementation. It manages a set of worker threads that can execute tasks concurrently.

- **Easy Integration**: The provided header file `threadpool.h` contains the necessary functions and structures to integrate the thread pool into your applications.

- **Sample Applications**: The repository includes sample applications such as `bruteforce_main` and `tpsample` to demonstrate how the thread pool can be used.

## Usage

### Building

To build the project and the sample applications, you can use the provided Makefile. Simply navigate to the project directory in your terminal and run:

`make`

This will generate the following executables:

- `bruteforce_main`: A simple application that uses the thread pool to find the password of a given hash using brute force.

- `tpsample`: A sample application that demonstrates how the thread pool can be used.

### Example

The following code snippet demonstrates how the thread pool can be used to parallelize a task. The example uses the `threadpool.h` header file to integrate the thread pool into the application.

```
#include "threadpool.h"

// Function that will be executed by the worker threads
void task(void *arg) {
    // Do something
}

int main() {
    // Create a thread pool with 4 worker threads
    threadpool_t *pool = threadpool_create(4);

    // Add 10 tasks to the thread pool
    for (int i = 0; i < 10; i++) {
        threadpool_add_task(pool, task, NULL);
    }

    // Wait for all tasks to complete
    threadpool_wait(pool);

    // Destroy the thread pool
    threadpool_destroy(pool);
}
```

## File Structure

```

├── bruteforce_main.c
├── bruteforce_main.h
├── Makefile
├── README.md
├── src
│   ├── threadpool.c
│   └── threadpool.h
└── tpsample.c

```

## Contributing

Contributions are welcome! If you have any suggestions or encounter any issues, please feel free to open an issue or submit a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
