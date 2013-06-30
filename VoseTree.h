
#ifndef _VOSETREE_H_
#define _VOSETREE_H_

#include <map>
#include <random>
#include <string>
#include <vector>
#include "Vose.h"


struct vosetreenode {
  /**
    * If true bag is defined and marble is null, if false
    * bag is null and marble is defined
    */
  bool isBag;
  marble ball;
  Vose bag;
  vosetreenode() { }
  vosetreenode(marble _ball): isBag(false), ball(_ball) { /*bag = NULL;*/ }
  vosetreenode(Vose _bag): isBag(true), bag(_bag) { /*ball = NULL;*/ }
  std::string toString() {
    return (isBag)?bag.toString():ball.toString();
  }
};
  

class VoseTree {

private:
  friend class Vose;

  /** Branch random function */
  std::function<double()> random_branch;

  /**
    * The probability of entering each branch.
    */
  std::vector<long> branches;
  /**
    * This holds all the nodes.
    */
  std::vector<vosetreenode> nodes;
  /**
    * The index of the last bag. We have [0, k] can be bags, then [k+1, n] must
    * be marbles. Last bag is this k.
    */
  size_t last_bag;

  /** This utility function compresses all the bags into one. */
  Vose merge_bags();
  

protected:

  long total_weight; 

  /**
    * The max number of free marbles until we have to put them in a bag.
    */
  size_t max_marbles;

  /**
    * The max number of bags until we compress all the bags into one.
    */
  size_t max_bags;


public:
  
  VoseTree(): total_weight(0L),
              last_bag(0),
              max_marbles(100),
              max_bags(0),
              branches(std::vector<long> ()),
              random_branch(std::function<double()> ()),
              nodes(std::vector<vosetreenode> ()) { }

  double add(std::string label, long weight);
  double add(marble m);

  //virtual size_t rand(); // TODO
  marble rand_marble();

  /** init() all bags until we reach the first non bag */
  void init();

  /**
    * Calculate the probability of each of the keys.
    */
  std::map<std::string, double> probabilities();
  /** Find the distribution of weights in the vose tree. */
  std::map<std::string, long> distribution();
  
  /** Sample from the tree, and return the probability each marble is selected. */
  static std::map<std::string, double> 
    histogram(VoseTree tree, long samples);

  static double kl(const std::map<std::string, double> &a, const std::map<std::string, double> &b);

  /**
    * Pretty print the VoseTree
    */
  std::string toString();

  void set_max_marbles(long new_max) { max_marbles = new_max; }
  void set_max_bags(long new_max) { max_bags = new_max; }

};

#endif // _VOSETREE_H_

