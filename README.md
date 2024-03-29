# OS Course Projects - Fall 2023

This repository is a collection of three distinct projects developed for the Operating Systems course at the University of Tehran. Each project tackles a different aspect of system programming and parallel processing, showcasing the practical application of concepts learned in class.

## Project 1: Restaurant Application using Socket Programming

### Overview
This project simulates a real-world restaurant-supplier-customer ecosystem. Written in low-level C, it demonstrates the use of socket programming to facilitate communication between different entities. The application mimics the operations of a restaurant by handling orders, managing supplier interactions, and maintaining customer service through a networked environment.

### Features
- Real-time order placement and processing.
- Dynamic interaction between customers and suppliers.
- Implementation of TCP and UDP protocols for reliable and fast communication.
- Secure login with unique username authentication for all users.
- Loading ingredient data from CSV file.
---
## Project 2: Bill Calculator

### Overview
The bill calculator is designed to automate the computation of utility costs for gas, water, and electricity in a residential complex. It employs the MapReduce pattern to distribute the processing load across multiple subprocesses, highlighting the efficiency of parallel computations in C++.

The project's core lies in its ability to break down complex calculations into smaller, manageable tasks that subprocesses can execute. This division of labor not only speeds up the computation but also serves as a practical example of the MapReduce model in action.

---
## Project 3: Image Processing

### Overview
Our image processing application is a versatile tool capable of performing a variety of manipulations on BMP images. From basic transformations like flipping to more complex filters such as purple haze, this application showcases the power of parallel processing in image editing.

### Versions
- **Serialized Version**: Processes images in a step-by-step manner, suitable for understanding the basic flow of operations.
- **Parallelized Version**: Leverages the `pthread` library in C++ to divide the workload among multiple threads, significantly reducing processing time.

