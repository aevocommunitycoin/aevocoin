// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 5000;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        ( 0, uint256("0xbedc296640887d1a5b258b6684e5d3bc6b0c831817c716fea4d76a7223dcf9c3") )
        ( 10000, uint256("0x13bea83b666adac7a8d4447459d7ead459deb90f182928b23b596b340b7e8b79") )
        ( 100000, uint256("0x12c119fc1106f7fcdb21e25be9d0fd3ef18005a47e186fc3badfa5efe9dece8b") )
        ( 130000, uint256("0x2845979335241e1c4c9ada13be7d57fa25684499f0954bf1f9a77eb494d23580") )
        ( 140000, uint256("0x1acd29cc09573efd60649c9ec5e450abe7dc9ea61235488552e95c34426e89f9") )
        ( 200000, uint256("0x4a0209b2fbc0114a1bb99bcd40ab42707b0d89918dcfabd1fb2dd0d407a28633") )
        ( 300000, uint256("0x205a6c56856f4b50a1ac2abe17cdf9e957a02ac7eb5e0ffee4b81d419ea22a6e") )
        ( 400000, uint256("0x07b9645aee5d385db23b14fbb27f373a02a9094b7ff263c00919b2482fff0eae") )
        ( 500000, uint256("0x449b6751714382f62378d3f4f7f0c03f0b62567b71e25260b2ab27efb830d440") )
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();
        if (nHeight <= pindexSync->nHeight){
            return false;
        }
        return true;
    }
}
