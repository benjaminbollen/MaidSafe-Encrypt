/*
* ============================================================================
*
* Copyright [2009] maidsafe.net limited
*
* Description:  Threadpool class test.
* Version:      1.0
* Created:      2009-08-06-03.12.31
* Revision:     none
* Compiler:     gcc
* Author:       Fraser Hutchison (fh), fraser.hutchison@maidsafe.net
* Company:      maidsafe.net limited
*
* The following source code is property of maidsafe.net limited and is not
* meant for external use.  The use of this code is governed by the license
* file LICENSE.TXT found in the root of this directory and also on
* www.maidsafe.net.
*
* You are not free to copy, amend or otherwise use this source code without
* the explicit written permission of the board of directors of maidsafe.net.
*
* ============================================================================
*/

#include <boost/date_time/posix_time/posix_time.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "boost/threadpool.hpp"

namespace test_threadpool {

class Task {
 public:
  Task() {}
  ~Task() {}
  void TaskFunction(const int &task_no,
                    const int &delay,
                    std::vector<int> *result_order,
                    boost::mutex *mutex) {
//    printf("Started task %i - sleeping for %i ms.\n", task_no, delay);
    boost::this_thread::sleep(boost::posix_time::milliseconds(delay));
    {
      boost::mutex::scoped_lock lock(*mutex);
      result_order->push_back(task_no);
    }
//    printf("Finished task %i\n", task_no);
  }
};

}  // namespace test_threadpool

namespace maidsafe {

class TestThreadPool : public testing::Test {
 protected:
  TestThreadPool() {}
  ~TestThreadPool() {}
 private:
  TestThreadPool(const TestThreadPool&);
  TestThreadPool &operator=(const TestThreadPool&);
};

TEST_F(TestThreadPool, BEH_MAID_ThreadPool) {
  test_threadpool::Task task;
  std::vector<int> result_order;
  boost::mutex result_order_mutex;
  // Create a thread pool.
  boost::threadpool::pool tp(2);
  // Add some tasks to the pool.
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 0, 50, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 1, 3000, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 2, 150, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 3, 100, &result_order, &result_order_mutex)));
  tp.wait();
  ASSERT_EQ(static_cast<unsigned int>(4), result_order.size());
  ASSERT_EQ(0, result_order.at(0));
  ASSERT_EQ(2, result_order.at(1));
  ASSERT_EQ(3, result_order.at(2));
  ASSERT_EQ(1, result_order.at(3));
}

TEST_F(TestThreadPool, BEH_MAID_ThreadPoolCancel) {
  test_threadpool::Task task;
  std::vector<int> result_order;
  boost::mutex result_order_mutex;
  {
    // Create a thread pool.
    boost::threadpool::thread_pool<boost::threadpool::task_func,
                                   boost::threadpool::fifo_scheduler,
                                   boost::threadpool::static_size,
                                   boost::threadpool::resize_controller,
                                   boost::threadpool::immediately> tp(2);
    // Add some tasks to the pool.
    ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
        &task, 0, 50, &result_order, &result_order_mutex)));
    ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
        &task, 1, 3000, &result_order, &result_order_mutex)));
    ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
        &task, 2, 150, &result_order, &result_order_mutex)));
    ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
        &task, 3, 100, &result_order, &result_order_mutex)));
    boost::this_thread::sleep(boost::posix_time::milliseconds(400));
  }
  ASSERT_EQ(static_cast<unsigned int>(3), result_order.size());
  ASSERT_EQ(0, result_order.at(0));
  ASSERT_EQ(2, result_order.at(1));
  ASSERT_EQ(3, result_order.at(2));
}

TEST_F(TestThreadPool, BEH_MAID_ThreadPoolClear) {
  test_threadpool::Task task;
  std::vector<int> result_order;
  boost::mutex result_order_mutex;
  // Create a thread pool.
  boost::threadpool::thread_pool<boost::threadpool::task_func,
                                 boost::threadpool::fifo_scheduler,
                                 boost::threadpool::static_size,
                                 boost::threadpool::resize_controller,
                                 boost::threadpool::immediately> tp(2);
  // Add some tasks to the pool.
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 0, 500, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 1, 3000, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 2, 2000, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 3, 100, &result_order, &result_order_mutex)));
  ASSERT_EQ(static_cast<unsigned int>(0), result_order.size());
  boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
  tp.clear();
  tp.wait();
  boost::this_thread::sleep(boost::posix_time::milliseconds(3000));
  ASSERT_EQ(static_cast<unsigned int>(3), result_order.size());
  ASSERT_EQ(0, result_order.at(0));
  ASSERT_EQ(2, result_order.at(1));
  ASSERT_EQ(1, result_order.at(2));
}

TEST_F(TestThreadPool, BEH_MAID_ThreadPoolSizes) {
  test_threadpool::Task task;
  std::vector<int> result_order;
  boost::mutex result_order_mutex;
  // Create a thread pool.
  boost::threadpool::thread_pool<boost::threadpool::task_func,
                                 boost::threadpool::fifo_scheduler,
                                 boost::threadpool::static_size,
                                 boost::threadpool::resize_controller,
                                 boost::threadpool::immediately> tp(2);
  // Add some tasks to the pool.
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 0, 1000, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 1, 200, &result_order, &result_order_mutex)));
  boost::this_thread::sleep(boost::posix_time::milliseconds(10));
  ASSERT_EQ(static_cast<unsigned int>(2), tp.active());
  ASSERT_EQ(static_cast<unsigned int>(0), tp.pending());
  ASSERT_EQ(static_cast<unsigned int>(2), tp.size());
  boost::this_thread::sleep(boost::posix_time::milliseconds(600));
  ASSERT_EQ(static_cast<unsigned int>(1), tp.active());
  ASSERT_EQ(static_cast<unsigned int>(0), tp.pending());
  ASSERT_EQ(static_cast<unsigned int>(2), tp.size());
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 2, 3000, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 3, 2000, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 4, 3000, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 5, 1000, &result_order, &result_order_mutex)));
  boost::this_thread::sleep(boost::posix_time::milliseconds(10));
  ASSERT_EQ(static_cast<unsigned int>(2), tp.active());
  ASSERT_EQ(static_cast<unsigned int>(3), tp.pending());
  ASSERT_EQ(static_cast<unsigned int>(2), tp.size());
  boost::this_thread::sleep(boost::posix_time::milliseconds(900));
  ASSERT_EQ(static_cast<unsigned int>(2), tp.active());
  ASSERT_EQ(static_cast<unsigned int>(2), tp.pending());
  ASSERT_EQ(static_cast<unsigned int>(2), tp.size());
  tp.wait();
  ASSERT_EQ(static_cast<unsigned int>(0), tp.active());
  ASSERT_EQ(static_cast<unsigned int>(0), tp.pending());
  ASSERT_EQ(static_cast<unsigned int>(2), tp.size());
  ASSERT_EQ(static_cast<unsigned int>(6), result_order.size());
  ASSERT_EQ(1, result_order.at(0));
  ASSERT_EQ(0, result_order.at(1));
  ASSERT_EQ(3, result_order.at(2));
  ASSERT_EQ(2, result_order.at(3));
  ASSERT_EQ(5, result_order.at(4));
  ASSERT_EQ(4, result_order.at(5));
}

TEST_F(TestThreadPool, BEH_MAID_ThreadPoolResize) {
  test_threadpool::Task task;
  std::vector<int> result_order;
  boost::mutex result_order_mutex;
  // Create a thread pool.
  boost::threadpool::thread_pool<boost::threadpool::task_func,
                                 boost::threadpool::fifo_scheduler,
                                 boost::threadpool::static_size,
                                 boost::threadpool::resize_controller,
                                 boost::threadpool::immediately> tp(1);
  // Add some tasks to the pool.
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 0, 3000, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 1, 2000, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 2, 1000, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 3, 500, &result_order, &result_order_mutex)));
  tp.wait();
  ASSERT_EQ(static_cast<unsigned int>(4), result_order.size());
  ASSERT_EQ(0, result_order.at(0));
  ASSERT_EQ(1, result_order.at(1));
  ASSERT_EQ(2, result_order.at(2));
  ASSERT_EQ(3, result_order.at(3));
  // Add some more tasks to the pool and increase its size.
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 4, 3000, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 5, 2000, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 6, 1000, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 7, 500, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.size_controller().resize(4));
  ASSERT_TRUE(tp.schedule(boost::bind(&test_threadpool::Task::TaskFunction,
      &task, 8, 50, &result_order, &result_order_mutex)));
  ASSERT_TRUE(tp.size_controller().resize(5));
  tp.wait();
  ASSERT_EQ(static_cast<unsigned int>(9), result_order.size());
  ASSERT_EQ(8, result_order.at(4));
  ASSERT_EQ(7, result_order.at(5));
  ASSERT_EQ(6, result_order.at(6));
  ASSERT_EQ(5, result_order.at(7));
  ASSERT_EQ(4, result_order.at(8));
}
}  // namespace maidsafe
