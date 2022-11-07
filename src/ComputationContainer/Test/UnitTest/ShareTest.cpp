#include "Logging/Logger.hpp"
#include "Share/Share.hpp"
#include "gtest/gtest.h"

#define ENABLE_KKRT ON
#define NDEBUG true
#define LIBOTE_HAS_NCO ON
TEST(ShareTest, GetValue)
{
    Share a(FixedPoint("3.0"));
    EXPECT_EQ(a.getVal(), FixedPoint("3.0"));
}

TEST(ShareTest, IncrementId)
{
    Share a(FixedPoint("1.0"));
    Share b(FixedPoint("1.0"));
    int diff = b.getId().getShareId() - a.getId().getShareId();
    EXPECT_EQ(diff, 1);
}

TEST(ShareTest, AddBetweenShares)
{
    Share a(FixedPoint("3.0"));
    Share b(FixedPoint("0.141"));
    Share result = a + b;
    EXPECT_EQ(result.getVal(), FixedPoint("3.141"));
}

TEST(ShareTest, SubBetweenShares)
{
    Share a(FixedPoint("3.141"));
    Share b(FixedPoint("0.141"));
    Share result = a - b;
    EXPECT_EQ(result.getVal(), FixedPoint("3.0"));
}

TEST(ShareTest, AddBetweenShareAndScalar)
{
    Share a(FixedPoint("3.0"));
    FixedPoint b("0.141");
    Share result = a + b;
    EXPECT_EQ(result.getVal(), FixedPoint("3.141"));
}

TEST(ShareTest, SubBetweenShareAndScalar)
{
    Share a(FixedPoint("3.141"));
    FixedPoint b("0.141");
    Share result = a - b;
    EXPECT_EQ(result.getVal(), FixedPoint("3.0"));
}

TEST(ShareTest, MulBetweenShareAndScalar)
{
    Share a(FixedPoint("0.1"));
    FixedPoint b("10.0");
    Share result = a * b;
    EXPECT_EQ(result.getVal(), FixedPoint("1.0"));
}

TEST(ShareTest, DivBetweenShareAndScalar)
{
    Share a(FixedPoint("1.0"));
    FixedPoint b("10.0");
    Share result = a / b;
    EXPECT_EQ(result.getVal(), FixedPoint("0.1"));
}

TEST(ShareTest, GetVarStrOfShare)
{
    FixedPoint a("12.2"), b("1.355");
    Share sa(a), sb(b);
    Share result = sa + sb;
    EXPECT_EQ(result.getVal(), FixedPoint("13.555"));
}

TEST(ShareTest, GetVarDoubleOfShare)
{
    FixedPoint a("12.2"), b("1.355");
    Share sa(a), sb(b);
    Share result = sa + sb;
    QMPC_LOG_INFO("{}", result.getDoubleVal());
    EXPECT_NEAR(result.getDoubleVal(), 13.555, 0.001);
}

#include "Share/Compare.hpp"
#include "libOTe/NChooseOne/NcoOtExt.h"
#include "libOTe/TwoChooseOne/Iknp/IknpOtExtReceiver.h"
#include "libOTe/TwoChooseOne/Iknp/IknpOtExtSender.h"
#include "libOTe_Tests/Common.h"
#include "libOTe_Tests/NcoOT_Tests.h"
#include "libOTe_Tests/OT_Tests.h"
namespace tests_libOTe
{
using namespace osuCrypto;
void setBaseOts(NcoOtExtSender &sender, NcoOtExtReceiver &recv)
{
    u64 baseCount = sender.getBaseOTCount();
    std::cout << "base count is " << baseCount << std::endl;
    std::vector<block> baseRecv(baseCount);
    std::vector<std::array<block, 2>> baseSend(baseCount);
    BitVector baseChoice(baseCount);
    PRNG prng0(ZeroBlock);
    PRNG prng1(OneBlock);
    baseChoice.randomize(prng0);

    prng0.get((u8 *)baseSend.data()->data(), sizeof(block) * 2 * baseSend.size());
    for (u64 i = 0; i < baseCount; ++i)
    {
        baseRecv[i] = baseSend[i][baseChoice[i]];
    }

    auto sock = cp::LocalAsyncSocket::makePair();

    auto p0 = sender.setBaseOts(baseRecv, baseChoice, sock[0]);
    auto p1 = recv.setBaseOts(baseSend, prng0, sock[1]);

    eval(p0, p1);
    std::cout << "base ots is end" << std::endl;
}

void testNco(
    NcoOtExtSender &sender, const u64 &numOTs, PRNG &prng0, NcoOtExtReceiver &recv, PRNG &prng1
)
{
    auto sock = cp::LocalAsyncSocket::makePair();

    u64 stepSize = 33;
    std::vector<block> inputs(stepSize);

    for (size_t j = 0; j < 3; j++)
    {
        // perform the init on each of the classes. should be performed concurrently

        auto p0 = sender.init(numOTs, prng0, sock[0]);
        auto p1 = recv.init(numOTs, prng1, sock[1]);
        std::cout << "test nco start" << std::endl;
        eval(p0, p1);
        std::cout << "first eval clear" << std::endl;
        std::vector<block> encoding1(stepSize), encoding2(stepSize);

        // Get the random OT mMessages
        for (u64 i = 0; i < numOTs; i += stepSize)
        {
            auto curStepSize = std::min<u64>(stepSize, numOTs - i);
            std::vector<u8> skips(curStepSize);

            prng0.get(inputs.data(), inputs.size());
            prng0.get((bool *)skips.data(), skips.size());

            for (u64 k = 0; k < curStepSize; ++k)
            {
                // The receiver MUST encode before the sender. Here we are only calling encode(...)
                // for a single i. But the receiver can also encode many i, but should only make one
                // call to encode for any given value of i.
                if (skips[k])
                {
                    recv.zeroEncode(i + k);
                }
                else
                {
                    recv.encode(i + k, &inputs[k], (u8 *)&encoding1[k], sizeof(block));
                }
            }

            // This call will send to the other party the next "curStepSize " corrections to the
            // sender. If we had made more or less calls to encode above (for contigious i), then we
            // should replace curStepSize  with however many calls we made. In an extreme case, the
            // reciever can perform encode for i \in {0, ..., numOTs - 1}  and then call
            // sendCorrection(recvChl, numOTs).
            auto p1 = recv.sendCorrection(sock[1], curStepSize);

            // receive the next curStepSize  correction values. This allows the sender to now call
            // encode on the next curStepSize  OTs.
            auto p0 = sender.recvCorrection(sock[0], curStepSize);

            eval(p0, p1);

            for (u64 k = 0; k < curStepSize; ++k)
            {
                // the sender can now call encode(i, ...) for k \in {0, ..., i}.
                // Lets encode the same input and then we should expect to
                // get the same encoding.
                sender.encode(i + k, &inputs[k], (u8 *)&encoding2[k], sizeof(block));

                // check that we do in fact get the same value

                // In addition to the sender being able to obtain the same value as the receiver,
                // the sender can encode and other codeword. This should result in a different
                // encoding.
                inputs[k] = prng0.get<block>();

                sender.encode(i + k, &inputs[k], (u8 *)&encoding2[k], sizeof(block));
            }
        }

        p0 = sender.check(sock[0], prng0.get());
        p1 = recv.check(sock[1], prng1.get());

        eval(p0, p1);
    }
}
}  // namespace tests_libOTe
TEST(ShareTest, OTTest)
{
    using namespace osuCrypto;
    osuCrypto::PRNG prng0(osuCrypto::block(4253465, 3434565));
    osuCrypto::PRNG prng1(osuCrypto::block(42532335, 334565));

    // The total number that we wish to do
    coproto::u64 numOTs = 128;

    osuCrypto::KkrtNcoOtSender sender;
    osuCrypto::KkrtNcoOtReceiver recv;

    std::vector<block> recvMsg(numOTs), baseRecv(128);
    std::vector<std::array<block, 2>> sendMsg(numOTs), baseSend(128);
    BitVector choices(numOTs), baseChoice(128);
    choices.randomize(prng0);
    baseChoice.randomize(prng0);
    // get up the parameters and get some information back.
    //  1) false = semi-honest
    //  2) 40  =  statistical security param.
    //  3) numOTs = number of OTs that we will perform
    sender.configure(false, 40, 128);
    recv.configure(false, 40, 128);
    try
    {
        std::cout << "first OT" << std::endl;

        tests_libOTe::setBaseOts(sender, recv);
        std::cout << "set OT clear" << std::endl;
        tests_libOTe::testNco(sender, numOTs, prng0, recv, prng1);
        std::cout << "test NCo clear" << std::endl;
        auto sender2 = sender.split();
        auto recv2 = recv.split();

        tests_libOTe::testNco(*sender2, numOTs, prng0, *recv2, prng1);
        // tests_libOTe::OtExt_Iknp_Test();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    std::cout << "end of OT" << std::endl;
}
#include <cryptoTools/Common/CLP.h>
#include <cryptoTools/Common/Log.h>
#include <cryptoTools/Network/Channel.h>

#include "coproto/Socket/AsioSocket.h"
#include "cryptoTools/Common/Matrix.h"
#include "libOTe/Tools/Coproto.h"
#include "libOTe/config.h"
namespace osuCrypto
{
enum class Role
{
    Sender,
    Receiver,
};

auto chls = cp::LocalAsyncSocket::makePair();

template <typename NcoOtSender, typename NcoOtReceiver>
void NChooseOne_example(Role role, int totalOTs, int numThreads, std::string ip, std::string tag, CLP &)
{
    const u64 step = 1024;

    if (totalOTs == 0) totalOTs = 1 << 20;

    bool randomOT = true;
    u64 numOTs = (u64)totalOTs;
    auto numChosenMsgs = 256;

    // get up the networking
    auto chl = cp::asioConnect(ip, role == Role::Sender);
    // auto chl = role == Role::Sender ? chls[0] : chls[1];

    PRNG prng(ZeroBlock);  // sysRandomSeed());

    NcoOtSender sender;
    NcoOtReceiver recver;

    // all Nco Ot extenders must have configure called first. This determines
    // a variety of parameters such as how many base OTs are required.
    bool maliciousSecure = false;
    u64 statSecParam = 40;
    u64 inputBitCount = 76;  // the kkrt protocol default to 128 but oos can only do 76.

    // create a lambda function that performs the computation of a single receiver thread.
    auto recvRoutine = [&]() -> task<>
    {
        MC_BEGIN(
            task<>,
                &,
            i = u64{},
            min = u64{},
            recvMsgs = std::vector<block>{},
            choices = std::vector<u64>{}
        );

        recver.configure(maliciousSecure, statSecParam, inputBitCount);
        // MC_AWAIT(sync(chl, Role::Receiver));

        if (randomOT)
        {
            // once configure(...) and setBaseOts(...) are called,
            // we can compute many batches of OTs. First we need to tell
            // the instance how many OTs we want in this batch. This is done here.
            MC_AWAIT(recver.init(numOTs, prng, chl));

            // now we can iterate over the OTs and actually retrieve the desired
            // messages. However, for efficiency we will do this in steps where
            // we do some computation followed by sending off data. This is more
            // efficient since data will be sent in the background :).
            for (i = 0; i < numOTs;)
            {
                // figure out how many OTs we want to do in this step.
                min = std::min<u64>(numOTs - i, step);

                //// iterate over this step.
                for (u64 j = 0; j < min; ++j, ++i)
                {
                    // For the OT index by i, we need to pick which
                    // one of the N OT messages that we want. For this
                    // example we simply pick a random one. Note only the
                    // first log2(N) bits of choice is considered.
                    block choice = prng.get<block>();

                    // this will hold the (random) OT message of our choice
                    block otMessage;

                    // retrieve the desired message.
                    recver.encode(i, &choice, &otMessage);

                    // do something cool with otMessage
                    // otMessage;
                }

                // Note that all OTs in this region must be encode. If there are some
                // that you don't actually care about, then you can skip them by calling
                //
                //    recver.zeroEncode(i);
                //

                // Now that we have gotten out the OT mMessages for this step,
                // we are ready to send over network some information that
                // allows the sender to also compute the OT mMessages. Since we just
                // encoded "min" OT mMessages, we will tell the class to send the
                // next min "correction" values.
                MC_AWAIT(recver.sendCorrection(chl, min));
            }

            // once all numOTs have been encoded and had their correction values sent
            // we must call check. This allows to sender to make sure we did not cheat.
            // For semi-honest protocols, this can and will be skipped.
            MC_AWAIT(recver.check(chl, prng.get()));
        }
        else
        {
            recvMsgs.resize(numOTs);
            choices.resize(numOTs);

            // define which messages the receiver should learn.
            for (i = 0; i < numOTs; ++i) choices[i] = prng.get<u8>();

            // the messages that were learned are written to recvMsgs.
            MC_AWAIT(recver.receiveChosen(numChosenMsgs, recvMsgs, choices, prng, chl));
        }

        MC_AWAIT(chl.flush());
        MC_END();
    };

    // create a lambda function that performs the computation of a single sender thread.
    auto sendRoutine = [&]()
    {
        MC_BEGIN(task<>, &, sendMessages = Matrix<block>{}, i = u64{}, min = u64{});

        sender.configure(maliciousSecure, statSecParam, inputBitCount);
        // MC_AWAIT(sync(chl, Role::Sender));

        if (randomOT)
        {
            // Same explanation as above.
            MC_AWAIT(sender.init(numOTs, prng, chl));

            // Same explanation as above.
            for (i = 0; i < numOTs;)
            {
                // Same explanation as above.
                min = std::min<u64>(numOTs - i, step);

                // unlike for the receiver, before we call encode to get
                // some desired OT message, we must call recvCorrection(...).
                // This receivers some information that the receiver had sent
                // and allows the sender to compute any OT message that they desired.
                // Note that the step size must match what the receiver used.
                // If this is unknown you can use recvCorrection(chl) -> u64
                // which will tell you how many were sent.
                MC_AWAIT(sender.recvCorrection(chl, min));

                // we now encode any OT message with index less that i + min.
                for (u64 j = 0; j < min; ++j, ++i)
                {
                    // in particular, the sender can retrieve many OT messages
                    // at a single index, in this case we chose to retrieve 3
                    // but that is arbitrary.
                    auto choice0 = prng.get<block>();
                    auto choice1 = prng.get<block>();
                    auto choice2 = prng.get<block>();

                    // these we hold the actual OT messages.
                    block otMessage0, otMessage1, otMessage2;

                    // now retrieve the messages
                    sender.encode(i, &choice0, &otMessage0);
                    sender.encode(i, &choice1, &otMessage1);
                    sender.encode(i, &choice2, &otMessage2);
                }
            }

            // This call is required to make sure the receiver did not cheat.
            // All corrections must be received before this is called.
            MC_AWAIT(sender.check(chl, ZeroBlock));
        }
        else
        {
            // populate this with the messages that you want to send.
            sendMessages.resize(numOTs, numChosenMsgs);
            prng.get(sendMessages.data(), sendMessages.size());

            // perform the OTs with the given messages.
            MC_AWAIT(sender.sendChosen(sendMessages, prng, chl));
        }

        MC_AWAIT(chl.flush());
        MC_END();
    };

    Timer time;
    auto s = time.setTimePoint("start");

    task<> proto;
    if (role == Role::Sender)
        proto = sendRoutine();
    else
        proto = recvRoutine();
    try
    {
        cp::sync_wait(proto);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

    auto e = time.setTimePoint("finish");
    auto milli = std::chrono::duration_cast<std::chrono::microseconds>(e - s).count();

    if (role == Role::Sender)
        std::cout << tag << " n=" << totalOTs << " " << milli << " microsecond  " << std::endl;
}
}  // namespace osuCrypto
TEST(ShareTest, OTTest2)
{
    using namespace osuCrypto;
    auto n = 32;
    CLP cmd;
    auto t = 1;
    auto ip = "127.0.0.1:1212";
    std::vector<std::string> tag{"kkrt", "kkrt"};
    auto thrd = std::thread(
        [&]
        {
            try
            {
                NChooseOne_example<osuCrypto::KkrtNcoOtSender, osuCrypto::KkrtNcoOtReceiver>(
                    Role::Sender, n, t, ip, tag.back(), cmd
                );
            }
            catch (std::exception &e)
            {
                lout << e.what() << std::endl;
            }
        }
    );

    try
    {
        NChooseOne_example<osuCrypto::KkrtNcoOtSender, osuCrypto::KkrtNcoOtReceiver>(
            Role::Receiver, n, t, ip, tag.back(), cmd
        );
    }
    catch (std::exception &e)
    {
        lout << e.what() << std::endl;
    }
    thrd.join();
}