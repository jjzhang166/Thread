# Part 6: Need of Event Handling

Sometimes a thread needs to wait for an event to happen like a condition to become true or a task to be completed by another thread.

For example,

Suppose we are building a network based application. This application does following tasks,

1. Perform some handshaking with server
2. Load Data from XML files.
3. Do processing on data loaded from XML.

As we can see that Task 1 is not dependent on any other Tasks but Task 3 is dependent on Task 2. So, it means Task 1 and Task 2 can be run in parallel by different Threads to improve the performance of application.

So, let’s break this into a multi-threaded application,

Now, it includes two threads,

Responsibilities of Thread 1 are,

- Perform some handshaking with server.
- Wait for data to be loaded from XML by Thread 2
- Do processing on data loaded from XML.

Responsibilities of Thread 2 are,

- Load data from XML
- Notify another Thread i.e. waiting for the message.

[![Multithreaded Application](http://thispointer.com/wp-content/uploads/2015/06/first.png)](http://thispointer.com/wp-content/uploads/2015/06/first.png)

In above, Thread 1 performs some operations and then waits for an event/condition to happen. The event or condition here is,

*Is Data loaded successfully.*

Once Thread 1 receives that Event then it performs some processing on the data.

Thread 2, loads the data in parallel when Thread 1 was busy in doing Hand Shake Mechanism.

When Thread 2 successfully loads the data from XML, it then notifies the Thread 1 by signaling that Event.

Now When Event or Condition is signaled then Thread 1 will continue the processing of Data.

What’s the benefit of making it multi-threaded?

When Thread 1 is busy in some handshaking mechanism then Thread 2 will load the data parallel from XML. So, it will increase the performance of application.

Now, how to achieve this,

**Option 1:**

Make a Boolean global variable with default value false. Set its value to true in Thread 2 and Thread 1 will keep on checking its value in loop and as soon as it becomes true Thread 1 will continue with processing of data. But as it’s a global variable shared by both of the Threads it needs synchronization with mutex. Let’s see its code,

```c++
#include<iostream>
#include<thread>
#include<mutex>

class Application
{
 std::mutex m_mutex;
 bool m_bDataLoaded;
public:
 Application()
 {
 m_bDataLoaded = false;
 }
 void loadData()
 {
 // Make This Thread sleep for 1 Second
 std::this_thread::sleep_for(std::chrono::milliseconds(1000));
 std::cout<<"Loading Data from XML"<<std::endl;

 // Lock The Data structure
 std::lock_guard<std::mutex> guard(m_mutex);

 // Set the flag to true, means data is loaded
 m_bDataLoaded = true;

 }
 void mainTask()
 {
 std::cout<<"Do Some Handshaking"<<std::endl;

 // Acquire the Lock
 m_mutex.lock();
 // Check if flag is set to true or not
 while(m_bDataLoaded != true)
 {
  // Release the lock
  m_mutex.unlock();
  //sleep for 100 milli seconds
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // Acquire the lock
  m_mutex.lock();
  }
  // Release the lock
  m_mutex.unlock();
  //Doc processing on loaded Data
  std::cout<<"Do Processing On loaded Data"<<std::endl;
 }
};

int main()
{
  Application app;

  std::thread thread_1(&Application::mainTask, &app);
  std::thread thread_2(&Application::loadData, &app);

  thread_2.join();
  thread_1.join();
  return 0;
}
```

Output is as follows,

[![Output](http://thispointer.com/wp-content/uploads/2015/06/second_output.png)](http://thispointer.com/wp-content/uploads/2015/06/second_output.png)

**It has following disadvantages,**

Thread will keep on acquiring the lock and release it just to check the value, therefore it will consume CPU cycles and will also make Thread 1 slow, because it needs to acquire same lock to update the bool flag.

So obviously we need a better mechanism to achieve this, like if somehow Thread 1 could just block by waiting for an Event to get signaled and another Thread could signal that Event and make Thread 1 continue. It would have save many CPU cycles and gave better performance. But the question is how to achieve this?

We will see the answer in Option 2.

**Option 2:**

We can achieve this using *Condition Variables*.

*Condition Variable* is a kind Event used for signaling between 2 threads. One thread can wait for it to get signaled, while other thread can signal this.

Check out next article for detailed explanation of Conditional Variable in this Multi-threading series and solution to this problem using conditional variable.