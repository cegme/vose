#include "Vose.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <utility>


double Vose::add(std::string label, long weight) {
  // Create a new marble
  struct marble m;
  m.weight = weight;
  m.label = label; // TODO update with a proper copy() method

  return Vose::add(m);
}

double Vose::add(marble m) {
  
  // Update the total size of the Urn
  total_weight += m.weight;

  // Add the marble to the list
  scores.push_back(m);

  // TODO update streaming data structures

  return (double) m.weight / total_weight;
}

void Vose::init() {

  // Reset the random number generators
  reset_scores_die();
  reset_random_double();

  // clear alias array
  //vose_alias.clear();
  alias.resize(scores.size(), 0);

  // clear prob array
  //vose_prob.clear();
  prob.resize(scores.size(), 0);

  // Work lists
  std::list<size_t> small;
  std::list<size_t> large;

  size_t n = scores.size();

  std::vector<double> probs_list(n);
  for(size_t i = 0; i != n; ++i) {
    // Compute the weighted probabilies, times the size
    double prob = (double) n * scores[i].weight / total_weight;
    probs_list[i] = prob;
    //std:: cerr << "probs_list[" << i << "]: " << probs_list[i] << "\n";

    // Build work lists
    if(prob < 1) 
      small.push_back(i);
    else
      large.push_back(i);
  }

  while(!small.empty() && !large.empty()) {
    size_t from_small = small.front(); small.pop_front();
    size_t from_large = large.front(); large.pop_front();
    
    prob[from_small] = probs_list[from_small];
    alias[from_small] = from_large;

    probs_list[from_large] += probs_list[from_small] - 1;

    if(probs_list[from_large] < 1)
      small.push_front(from_large);
    else
      large.push_front(from_large);
  }

  while(!small.empty()) {
    size_t from_small = small.front(); small.pop_front();
    prob[from_small] = 1;
  }
  while (!large.empty()) {
    size_t from_large = large.front(); large.pop_front();
    prob[from_large] =  1;
  }
}
  

size_t Vose::rand() {
  // Warning! init() must have been called. 
  // TODO update this method to a more numerically stable version

  double u = random_scores_double();
  //std::cerr << "rand(): u = " << u << "\n";
  // Grab a random column from the main scores list
  size_t col = floor(u);
  //std::cerr << "rand(): col = " << col << "\n";

  // Make a toss to choose either alias side or the probability side 
  return (u - col)<=prob[col]? col : alias[col];
}

marble Vose::rand_marble() {
  return scores[rand()];
}

double Vose::kl(const std::vector<double> &a, const std::vector<double> &b) {

  std::vector<double> v;
  for(size_t i = 0; i != a.size(); ++i) {
    printf("a:%f, b:%f, log(a/b):%f\n", a[i], b[i], log(a[i]/b[i]));
    v.push_back(log(a[i]/b[i])*a[i]);
  }
  return std::accumulate(v.cbegin(), v.cend(), 0.0, std::plus<double>());
}

Vose Vose::merge(const Vose &v1, const Vose &v2) {

  Vose v;
  std::copy (v1.scores.begin(), v1.scores.end(), std::back_inserter(v.scores)); 
  std::copy (v2.scores.begin(), v2.scores.end(), std::back_inserter(v.scores)); 
  v.total_weight = v1.total_weight + v2.total_weight;

  return v;
} 


Vose Vose::merge(const Vose& vose) {

  Vose v;

  // concatenate the two scores arrays
  std::copy (scores.begin(),scores.end(),std::back_inserter(v.scores)); 
  std::copy (vose.scores.begin(),vose.scores.end(),std::back_inserter(v.scores)); 

  // update the total_weight
  v.total_weight = total_weight + vose.total_weight;

  return v; // The person using this shoud call 'init' as soon as possible
  
}

std::string Vose::toString() {
  std::stringstream ss;
  ss << "[" << total_weight << "|";
  std::for_each(scores.cbegin(), scores.cend(),[&ss] (marble m) {
    ss << m.toString();
  });
  ss << "]";
  return ss.str();
}

