
#include <iostream>
#include <map>
#include <sstream>

#include "VoseTree.h"
#include "VoseUtil.h"


double VoseTree::add(std::string label, long weight) {

  struct marble m;
  m.weight = weight;
  m.label = label; // TODO update with a proper copy() method

  return VoseTree::add(m);
}

double VoseTree::add(marble m) {
  
  // Update the new weight
  total_weight += m.weight;
  double contribution = (double) m.weight / total_weight;

  // Add the marble to another branch
  if(!branches.empty()) {
    double new_total = branches.back() + m.weight;
    branches.push_back(new_total);
  }
  else {
    branches.push_back(m.weight);
  }
    
  // Add this marble to the free marble list
  vosetreenode vtn;
  vtn.isBag = false;
  vtn.ball = m;
  nodes.push_back(vtn);

  // Shrink data structure if necessary
  if (branches.size() - last_bag > max_marbles) {
    // Compress the marbles to make a new bag
    Vose v;
    for(size_t i = last_bag; i != nodes.size(); ++i) {
      v.add(nodes[i].ball);
    }
    v.init();
    vosetreenode vtn2 (v);

    nodes[last_bag] = vtn2;
    nodes.resize(last_bag+1);
    
    branches.resize(last_bag+1);
    branches[last_bag] = total_weight;

    ++last_bag;
    if (last_bag > max_bags) {
      // Merge all the bags into one bag
      Vose z;
      z = merge_bags();
      nodes.resize(1);
      nodes[0] = z;

      // Update the branch and bag locations
      last_bag = 1;
      total_weight = z.weight();

      branches.resize(1);
      branches[0] = total_weight;
    } 
  } 
  
  // Update a random number generator to new range
  std::default_random_engine generator(VoseUtil::RANDOM_SEED);
  std::uniform_real_distribution<double>
    real_distribution(0, total_weight);
  random_branch = std::bind(real_distribution, generator);

  return contribution;
}


marble VoseTree::rand_marble() {

  // Select a random value in the range of [0, totalsize)
  double rand_sz = random_branch();

  // Binary search to find correct branch O(log n), n = branches.size()
  //std::vector<double>::iterator branch
  auto branch_idx
    = std::lower_bound (branches.begin(), branches.end(), rand_sz);

  size_t branch = branch_idx - branches.begin();
  if (nodes[branch].isBag) {
    // If a Vose class is at the node, we call rand() on the object.
    return nodes[branch].bag.rand_marble(); // O(1)
  }
  else {
    // If a marble is at the branch, return the marble
    return nodes[branch].ball; // O(1)
  }
}


Vose VoseTree::merge_bags() {
  Vose v;
  size_t i = 0;
  for(; i != nodes.size(); ++i) {
    if (!nodes[i].isBag) break;
    v = v.merge(nodes[i].bag);
  }
  nodes.erase(nodes.begin(), nodes.begin()+i-1); // -1 because we have to replace it
  nodes[0] = vosetreenode(v);
  
  branches.erase(branches.begin(), branches.begin()+i-1);
  branches[0] = v.weight();
  v.init();
  return v;
}

void VoseTree::init() {
  // Init  all the Vose bags
  for (size_t i = 0; i != nodes.size(); ++i) {
    if (!nodes[i].isBag) break;
    nodes[i].bag.init();
  }
}


std::string VoseTree::toString() {

  std::stringstream ss;
  ss << "[" << total_weight << "]\n";
  std::for_each (nodes.cbegin(), nodes.cend(), [this,&ss] (vosetreenode v) {
    ss << "----> " << v.toString() << "\n";
  });
  ss << "\n";
  return ss.str();

}


std::map<std::string, long> VoseTree::histogram() {

  std::map<std::string, long> map;
  std::for_each(nodes.cbegin(), nodes.cend(), [&map] (vosetreenode vtn) {
    if(vtn.isBag) {
      for (auto m : vtn.bag.get_scores()) {
        if(map.find(m.label) == map.end()) {
          map.insert(std::pair<std::string, long>(m.label, m.weight));
        }
        else {
          map[m.label] += m.weight;
        }
      }
    }
    else {
      if(map.find(vtn.ball.label) == map.end()) {
        map.insert(std::pair<std::string, long>(vtn.ball.label, vtn.ball.weight));
      }
      else {
        map[vtn.ball.label] += vtn.ball.weight;
      }
    }
  });
  return map;
}

std::map<std::string, double> VoseTree::probabilities() {

  auto h = histogram();
  std::map<std::string, double> prob;
  //map.resize(h.size());
  std::for_each(h.begin(), h.end(), [&] (std::pair<std::string, long> m) {
    //std::cerr << "p|" << m.first << "," << (double)m.second/total_weight << "\n";
    prob.insert(std::make_pair(m.first, (double)m.second/total_weight));
  });
  return prob;
}
double VoseTree::kl(const std::map<std::string, double> &a, const std::map<std::string, double> &b) {

  /*std::cerr<< "a: ";
  std::for_each(a.begin(), a.end(), [] (std::pair<const std::string, double> z) { 
    std::cerr << "<" << z.first << "," << z.second << ">";
  });
  std::cerr << "\nb: ";
  std::for_each(b.begin(), b.end(), [] (std::pair<const std::string, double> z) { 
    std::cerr << "<" << z.first << "," << z.second << ">";
  });*/

  std::vector<double> v;
  v.resize(a.size());
  std::transform(a.cbegin(), a.cend(), b.cbegin(), v.begin(), [&v] (std::pair<const std::string, double> a, std::pair<const std::string, double> b) {
    //printf("a:%f, b:%f, log(a/b):%f\n", a.second, b.second, log(a.second/b.second));
    return (double)log(a.second/b.second)*a.second;
  });
  return std::accumulate(v.cbegin(), v.cend(), 0.0, std::plus<double>());
}


