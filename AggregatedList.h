#ifndef _AGGREGATED_LIST_H
#define _AGGREGATED_LIST_H

#include <vector>
#include <string>

typedef std::vector< std::pair<std::string, unsigned int> > AggregatedList;
typedef std::vector< std::pair< std::string, std::vector<unsigned int> > > AggregatedReducedList;

#endif  // _AGGREGATED_LIST_H
