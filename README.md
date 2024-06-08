# Hotel-Management-System-C
Hotel Management System - posix compliant C program


Description:
The system simulates a hotel environment where the different entities like an Admin, a Hotel Manager, Tables, Customers and Waiters are represented as processes. There are table processes (maximum number of concurrent table processes will be 10) in this system such that each table process represents a table of this hotel. As we all know, customers enter a hotel, sit at the different tables and order food items from a predefined menu. We will simulate this using processes as well. Each table process will create one or more child processes, each child process representing a customer sitting at that particular table. We will call each such child process as a customer process. Note that each table process needs to create at least one customer process and can create a maximum of five concurrent customer processes at a time. In this hotel system, we will also have waiter processes. Each waiter process takes orders for a particular table and calculates the total bill amount for that table. Each table has a Number and each waiter has a Waiter ID. A waiter having ID x will only attend table number x. Note that the number of table processes will always be equal to the number of waiter processes. Moreover, the table processes and the waiter processes do not have any parent-child relationship. This implies that during execution, you should run each table process in a different terminal and each waiter process should run in a separate terminal. Additionally, there is a hotel manager process responsible for overseeing the total earnings, calculating the total earnings of all the waiters and handling termination, upon receiving a termination intimation from the admin process. The overall workflow is depicted in the diagram below. Note that the arrows in the figure depict communications between the relevant processes. In the rest of the problem statement, the term user is used to refer to a person who will be executing this application.



# IPC Constructs Used
<img width="662" alt="Screenshot 2024-06-08 at 10 04 18 PM" src="https://github.com/itsadnanlone/Hotel-Management-System-C/assets/155386596/4dfe7854-156b-4842-a16c-9ea12c566cf3">

## Video Run
https://github.com/itsadnanlone/Hotel-Management-System-C/assets/155386596/1d65ede8-c83b-4a72-ad76-cb89f8420a6e

