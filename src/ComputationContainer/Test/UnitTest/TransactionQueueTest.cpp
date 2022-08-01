#include "gtest/gtest.h"
#include "TransactionQueue/TransactionQueue.hpp"
#include <thread>
#include <vector>
#include <atomic>

const long long SIZE = 5000;

/*
step1：簡単なテスト
*/

// Pushで異常が投げられないかテスト
TEST(TransactionQueTest, CanPushTest)
{
    qmpc::Utils::TransactionQueue<long long> q;
    // push
    for (long long i = 0; i < SIZE; i++)
    {
        EXPECT_NO_THROW(q.push(i));
    }
}

// Popが順にされるかテスト
TEST(TransactionQueTest, SinglePushSinglePopTest)
{
    qmpc::Utils::TransactionQueue<long long> q;
    // push
    for (long long i = 0; i < SIZE; i++)
    {
        q.push(i);
    }
    // pop
    for (long long i = 0; i < SIZE; i++)
    {
        EXPECT_EQ(q.pop(), i);
    }
}

// Queuesizeが変わるのかテスト
TEST(TransactionQueTest, SetMaxTest)
{
    const long long RESIZE = 10000;
    qmpc::Utils::TransactionQueue<long long>::setMax((size_t)RESIZE);
    qmpc::Utils::TransactionQueue<long long> q;
    // push(Resize回)
    for (long long i = 0; i < RESIZE; i++)
    {
        q.push(i);
    }
    // push(Resize回)
    for (long long i = 0; i < RESIZE; i++)
    {
        EXPECT_EQ(q.pop(), i);
    }
}

/*
step2：同時処理
*/

// pushとpopを同時に行っても大丈夫かテスト
TEST(TransactionQueTest, AsyncSinglePushSinglePopTest)
{
    qmpc::Utils::TransactionQueue<long long> q;
    // 順にpushさせる
    std::thread th1([&]()
                    {
                        for (long long i = 0; i < SIZE; i++)
                        {
                            q.push(i);
                        }
                    });
    // 順にpopさせる
    std::thread th2([&]()
                    {
                        for (long long i = 0; i < SIZE; i++)
                        {
                            EXPECT_EQ(q.pop(), i);
                        }
                    });
    // pushとpopを複数threadで非同期に行っている
    th1.join();
    th2.join();
}

// pushを複数threadでやってもqueueが壊れないかテスト
TEST(TransactionQueTest, AsyncMultiPushSinglePopTest)
{
    // init
    long long num_thread = 10;
    qmpc::Utils::TransactionQueue<long long> q;
    std::vector<std::thread> threads_push;
    long long check[SIZE];
    for (long long i = 0; i < SIZE; i++)
    {
        check[i] = 0;
    }

    // num_thread個のthreadでpushさせる
    // pushさせる物の順は[0, SIZE)の順列を並び替えた物と同値
    for (long long i = 0; i < num_thread; i++)
    {
        threads_push.push_back(std::thread([&, i]()
                                           {
                                               for (long long j = 0; j < SIZE / num_thread; j++)
                                               {
                                                   q.push((SIZE / num_thread) * i + j);
                                               }
                                           }));
    }

    // SIZE全部出てくるかテスト
    // 順繰りに出てくるわけではないので注意
    for (long long i = 0; i < SIZE; i++)
    {
        long long id = q.pop(); // 一つ一つpopする
        check[id]++;            // 値が何回popされたか見る
    }

    for (long long i = 0; i < SIZE; i++)
    {
        EXPECT_EQ(check[i], 1); // 一回だけpopされたのならok
    }

    // 一応
    for (auto &th : threads_push)
    {
        th.join();
    }
}

// popを複数threadでやってもqueueが壊れないかテスト
TEST(TransactionQueTest, AsyncSinglePushMultiPopTest)
{
    // init
    long long num_thread = 10;
    qmpc::Utils::TransactionQueue<long long> q;
    std::atomic<long long> check[SIZE];
    for (long long i = 0; i < SIZE; i++)
    {
        check[i] = 0;
    }

    // [0, SIZE)を一つずつ昇順にpush
    for (long long i = 0; i < SIZE; i++)
    {
        q.push(i);
    }

    // SIZE個全部出てくるかnum_thread個のthreadでテスト
    // 各threadで順繰りに取れるわけではないので注意
    // 尚，popさせた順は[0, SIZE)を昇順に並び替えた配列と同値
    std::vector<std::thread> threads_pop;
    for (long long i = 0; i < num_thread; i++)
    {
        threads_pop.push_back(std::thread([&]()
                                          {
                                              for (long long j = 0; j < SIZE / num_thread; j++)
                                              {
                                                  long long id = q.pop(); // 一つ一つpopする
                                                  check[id]++;            // 値が何回popされたか見る
                                              }
                                          }));
    }

    // 全部終わるまで待機
    for (auto &th : threads_pop)
    {
        th.join();
    }

    for (long long i = 0; i < SIZE; i++)
    {
        EXPECT_EQ(check[i], 1); // 全部一回popされたならok
    }
}

// push,popを複数threadでやってもqueueが壊れないかテスト
TEST(TransactionQueTest, AsyncMultiPushMultiPopTest)
{
    // init
    long long num_thread = 10;
    qmpc::Utils::TransactionQueue<long long> q;
    std::vector<std::thread> threads_push;
    std::vector<std::thread> threads_pop;
    std::atomic<long long> check[SIZE];
    for (long long i = 0; i < SIZE; i++)
    {
        check[i] = 0;
    }

    // num_thread個のthreadでpushさせる
    // pushさせる物の順は[0, SIZE)の順列を並び替えた物と同値
    for (long long i = 0; i < num_thread; i++)
    {
        threads_push.push_back(std::thread([&, i]()
                                           {
                                               for (long long j = 0; j < SIZE / num_thread; j++)
                                               {
                                                   q.push((SIZE / num_thread) * i + j);
                                               }
                                           }));
    }

    // SIZE個全部出てくるかnum_thread個のthreadでテスト
    // 各threadで順繰りに取れるわけではないので注意
    // 尚，popさせた順は[0, SIZE)を昇順に並び替えた配列と同値
    for (long long i = 0; i < num_thread; i++)
    {
        threads_pop.push_back(std::thread([&]()
                                          {
                                              for (long long j = 0; j < SIZE / num_thread; j++)
                                              {
                                                  long long id = q.pop(); // 一つ一つpopする
                                                  check[id]++;            // 値が何回popされたか見る
                                              }
                                          }));
    }

    // 全部終わるまで待機
    for (auto &th : threads_push)
    {
        th.join();
    }
    for (auto &th : threads_pop)
    {
        th.join();
    }

    for (long long i = 0; i < SIZE; i++)
    {
        EXPECT_EQ(check[i], 1); // 全部一回popされたならok
    }
}

/*
step3：MAX_SIZE以上回数の処理
*/

// SIZE以上のpushをシングルpushシングルpopで達成できるかテスト
TEST(TransactionQueTest, AsyncSinglePushSinglePopMoreSIZETest)
{
    // init
    long long RESIZE = SIZE * 2;
    qmpc::Utils::TransactionQueue<long long> q;

    // 順にpushさせる
    std::thread th1([&]()
                    {
                        for (long long i = 0; i < RESIZE; i++)
                        {
                            q.push(i);
                        }
                    });
    // 順にpopさせる
    std::thread th2([&]()
                    {
                        for (long long i = 0; i < RESIZE; i++)
                        {
                            EXPECT_EQ(q.pop(), i);
                        }
                    });
    // pushとpopを複数threadで非同期に行っている
    th1.join();
    th2.join();
}

// SIZE以上のpushをマルチpushシングルpopで達成できるかテスト
TEST(TransactionQueTest, AsyncMultiPushSinglePopMoreSIZETest)
{
    // init
    long long num_thread = 10;
    long long RESIZE = SIZE * 2;
    qmpc::Utils::TransactionQueue<long long> q;
    std::vector<std::thread> threads_push;
    std::atomic<long long> check[RESIZE];
    for (long long i = 0; i < RESIZE; i++)
    {
        check[i] = 0;
    }

    // num_thread個のthreadでpushさせる
    // pushさせる物の順は[0, RESIZE)の順列を並び替えた物と同値
    for (long long i = 0; i < num_thread; i++)
    {
        threads_push.push_back(std::thread([&, i]()
                                           {
                                               for (long long j = 0; j < RESIZE / num_thread; j++)
                                               {
                                                   q.push((RESIZE / num_thread) * i + j);
                                               }
                                           }));
    }

    // RESIZE全部出てくるかテスト
    // 順繰りに出てくるわけではないので注意
    std::thread thread_pop([&]()
                           {
                               for (long long i = 0; i < RESIZE; i++)
                               {
                                   long long id = q.pop(); // 一つ一つpopする
                                   check[id]++;            // 値が何回popされたか見る
                               }
                           });

    // 全部終わるまで待機
    for (auto &th : threads_push)
    {
        th.join();
    }

    thread_pop.join();

    for (long long i = 0; i < RESIZE; i++)
    {
        EXPECT_EQ(check[i], 1); // 全部一回popされたならok
    }
}

// SIZE以上のpushをシングルpushマルチpopで達成できるかテスト
TEST(TransactionQueTest, AsyncSinglePushMultiPopMoreSIZETest)
{
    // init
    long long num_thread = 10;
    long long RESIZE = SIZE * 2;
    qmpc::Utils::TransactionQueue<long long> q;
    std::vector<std::thread> threads_pop;
    std::atomic<long long> check[RESIZE];
    for (long long i = 0; i < RESIZE; i++)
    {
        check[i] = 0;
    }

    // 順にpushさせる
    std::thread thread_push([&]()
                            {
                                for (long long i = 0; i < RESIZE; i++)
                                {
                                    q.push(i);
                                }
                            });

    // RESIZE個全部出てくるかnum_thread個のthreadでテスト
    // 各threadで順繰りに取れるわけではないので注意
    // 尚，popさせた順は[0, RESIZE)を昇順に並び替えた配列と同値
    for (long long i = 0; i < num_thread; i++)
    {
        threads_pop.push_back(std::thread([&]()
                                          {
                                              for (long long j = 0; j < RESIZE / num_thread; j++)
                                              {
                                                  long long id = q.pop(); // 一つ一つpopする
                                                  check[id]++;            // 値が何回popされたか見る
                                              }
                                          }));
    }

    // 全部終わるまで待機
    thread_push.join();
    for (auto &th : threads_pop)
    {
        th.join();
    }

    for (long long i = 0; i < RESIZE; i++)
    {
        EXPECT_EQ(check[i], 1); // 全部一回popされたならok
    }
}

// SIZE以上のpushをマルチpushマルチpopで達成できるかテスト
TEST(TransactionQueTest, AsyncMultiPushMultiPopMoreSIZETest)
{
    // init
    long long num_thread = 10;
    long long RESIZE = SIZE * 2;
    qmpc::Utils::TransactionQueue<long long> q;
    std::vector<std::thread> threads_push;
    std::vector<std::thread> threads_pop;
    std::atomic<long long> check[RESIZE];
    for (long long i = 0; i < RESIZE; i++)
    {
        check[i] = 0;
    }

    // num_thread個のthreadでpushさせる
    // pushさせる物の順は[0, RESIZE)の順列を並び替えた物と同値
    for (long long i = 0; i < num_thread; i++)
    {
        threads_push.push_back(std::thread([&, i]()
                                           {
                                               for (long long j = 0; j < RESIZE / num_thread; j++)
                                               {
                                                   q.push((RESIZE / num_thread) * i + j);
                                               }
                                           }));
    }

    // RESIZE個全部出てくるかnum_thread個のthreadでテスト
    // 各threadで順繰りに取れるわけではないので注意
    // 尚，popさせた順は[0, RESIZE)を昇順に並び替えた配列と同値
    for (long long i = 0; i < num_thread; i++)
    {
        threads_pop.push_back(std::thread([&]()
                                          {
                                              for (long long j = 0; j < RESIZE / num_thread; j++)
                                              {
                                                  long long id = q.pop(); // 一つ一つpopする
                                                  check[id]++;            // 値が何回popされたか見る
                                              }
                                          }));
    }

    // 全部終わるまで待機
    for (auto &th : threads_push)
    {
        th.join();
    }
    for (auto &th : threads_pop)
    {
        th.join();
    }

    for (long long i = 0; i < RESIZE; i++)
    {
        EXPECT_EQ(check[i], 1); // 全部一回popされたならok
    }
}