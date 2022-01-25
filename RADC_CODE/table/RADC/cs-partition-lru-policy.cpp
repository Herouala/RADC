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

#include "cs-partition-lru-policy.hpp"
#include "ns3/ndnSIM/NFD/daemon/table/cs.hpp"
#include <iostream>
#include <regex>
#include <string>
#include <algorithm>
////////////
#include "ns3/log.h"
#include "ns3/packet.h"

namespace nfd {
namespace cs {
namespace lru01 {

const std::string Lru01Policy::POLICY_NAME = "Lru01Policy";
NFD_REGISTER_CS_POLICY(Lru01Policy);

Lru01Policy::Lru01Policy()
  : Policy(POLICY_NAME)
{
	
}

void
Lru01Policy::doAfterInsert(iterator i)
{
  auto iterator_fo_evictEntries=i->getData();
  DataName=iterator_fo_evictEntries.getName(); // this var is used in the fonction evictEntries
 
  this->insertToQueue(i, true);
  this->evictEntries();
}

void
Lru01Policy::doAfterRefresh(iterator i)
{
  this->insertToQueue(i, false);
}

void
Lru01Policy::doBeforeErase(iterator i)
{
  auto namei=i->getData();
  Name Organization="/Important/";
  Name Other="/NotImportant/";
 
  
  if(Organization.isPrefixOf(namei.getName())){
  m_queue_Organization.get<1>().erase(i);}
  else if(Other.isPrefixOf(namei.getName())){
  m_queue_Other.get<1>().erase(i);}
  
  //else{  m_queue_Other.get<1>().erase(i);}
}

void
Lru01Policy::doBeforeUse(iterator i)
{
  this->insertToQueue(i, false);
}

void
Lru01Policy::evictEntries()
{
  Name Organization="/Important/";
  Name Other="/NotImportant/";
  int  Count_of_Organization,Count_of_Other;
  float C1,C2; // represent the two spaces 
  float k= 2; // represent the degree of importance of the space C2
  iterator i;
  
  // This Code could be used for spaces allocation
  /*
  if(Organization.isPrefixOf(DataName)){Count_of_Organization++;} else if (Other.isPrefixOf(DataName)){Count_of_Other++;}
  C2=((Count_of_Other+1)/(k*Count_of_Organization+1))^(1/(k-1));
  C1= this->getLimit()-C2;
  if(C1<0){ C1=0; C2= this->getLimit(); }*/
  
  
    // This is used if the CS is plain	
  BOOST_ASSERT(this->getCs() != nullptr);
  if (this->getCs()->size() > this->getLimit()) {
	  
    if(Organization.isPrefixOf(DataName)&& !m_queue_Organization.empty()){
	 
     i = m_queue_Organization.front();
     m_queue_Organization.pop_front();
     this->emitSignal(beforeEvict, i);
    }
    else if(Other.isPrefixOf(DataName)&& !m_queue_Other.empty()){
	 
     i = m_queue_Other.front();
     m_queue_Other.pop_front();
     this->emitSignal(beforeEvict, i);
	  
    }
    	
  }else if (this->getCs()->size() < this->getLimit()) { // This is used if the CS is not plain
  // Here we use a fixed patition according to the offline calculated partition using Lagrange utility function
   if(Organization.isPrefixOf(DataName)&& !m_queue_Organization.empty()&& m_queue_Organization.size()>73){
	 
     i = m_queue_Organization.front();
     m_queue_Organization.pop_front();
     this->emitSignal(beforeEvict, i);
    }
    else if(Other.isPrefixOf(DataName)&& !m_queue_Other.empty() && m_queue_Other.size()>10){
	 
     i = m_queue_Other.front();
     m_queue_Other.pop_front();
     this->emitSignal(beforeEvict, i);
	  
    }
    
	}

void
Lru01Policy::insertToQueue(iterator i, bool isNewEntry)
{
  Queue::iterator it;
   
  bool isNew = false;
  auto dataIt = i->getData();
  Name name=dataIt.getName();
  Name Organization="/Important/";
  Name Other="/NotImportant/";
  std::string s=dataIt.getName().toUri();
    
  if(Organization.isPrefixOf(name)){ 
  // push_back only if iterator i does not exist
   std::tie(it, isNew) = m_queue_Organization.push_back(i);
  
   BOOST_ASSERT(isNew == isNewEntry);
   if (!isNewEntry) {
    m_queue_Organization.relocate(m_queue_Organization.end(), it);
  }}
  else if(Other.isPrefixOf(name)){
	  // push_back only if iterator i does not exist
	  std::tie(it, isNew) = m_queue_Other.push_back(i);
  
      BOOST_ASSERT(isNew == isNewEntry);
      if (!isNewEntry) {
      m_queue_Other.relocate(m_queue_Other.end(), it);
  }}
  
  
}

} // namespace lru01
} // namespace cs
} // namespace nfd

