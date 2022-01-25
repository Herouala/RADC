/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2016,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NFD_DAEMON_TABLE_CS_POLICY_LRU01_HPP
#define NFD_DAEMON_TABLE_CS_POLICY_LRU01_HPP

#include "ns3/ndnSIM/NFD/daemon/table/cs-policy.hpp"


#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

//For the tracer
#include "ns3/ndnSIM/model/ndn-common.hpp"

#include <list>
#include <vector>

#include "ns3/ptr.h"
#include "ns3/net-device.h"
#include "ns3/nstime.h"
#include "ns3/traced-callback.h"

#include <boost/property_tree/ptree_fwd.hpp>
#include "ns3/ndnSIM/NFD/daemon/table/cs.hpp"
using namespace ns3;
namespace nfd {
namespace cs {
namespace lru01 {

struct EntryItComparator
{
  bool
  operator()(const iterator& a, const iterator& b) const
  {
    return *a < *b;
  }
};

typedef boost::multi_index_container<
    iterator,
    boost::multi_index::indexed_by<
      boost::multi_index::sequenced<>,
      boost::multi_index::ordered_unique<
        boost::multi_index::identity<iterator>, EntryItComparator
      >
    >
  > Queue;

/** \brief LRU cs replacement policy
 *
 * The least recently used entries get removed first.
 * Everytime when any entry is used or refreshed, Policy should witness the usage
 * of it.
 */
class Lru01Policy : public Policy 
{
	
public:
  Lru01Policy();

public:
  static const std::string POLICY_NAME;

private:
  virtual void
  doAfterInsert(iterator i) override;

  virtual void
  doAfterRefresh(iterator i) override;

  virtual void
  doBeforeErase(iterator i) override;

  virtual void
  doBeforeUse(iterator i) override;

  virtual void
  evictEntries() override;

private:
  /** \brief moves an entry to the end of queue
   */
  void
  insertToQueue(iterator i, bool isNewEntry);

private:
  Queue m_queue_Organization,m_queue_Other;
public:  
  Name DataName;
  int j=0;
  

  
};

} // namespace lru

using lru01::Lru01Policy;

} // namespace cs
} // namespace nfd

#endif // NFD_DAEMON_TABLE_CS_POLICY_LRU_HPP
