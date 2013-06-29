
#ifndef _VOSE_H_
#define _VOSE_H_

#include <algorithm>
#include <functional>
#include <random>
#include <string>
#include <sstream>
#include <vector>

#include "VoseUtil.h"


struct marble {
  long weight;
  std::string label;
  size_t index;
  bool operator<(const marble &m) const {
    return (label < m.label && weight < m.weight);
  }
  std::string toString() {
    std::stringstream ss;
    ss << "<" << label;
    ss << ",";
    ss << weight << ">"; 
    return ss.str();
  } 
};


class Vose {

private:
  friend class test;
  /** A random generator in the range of [0,scores.size() ) */
  std::function<double()> random_scores_die;
  /** A random number generator in the range [0,1] */
  std::function<double()> random_prob_die;

protected:
  /** All the marbles with their label and weight */
  std::vector<marble> scores;
  /** The total weight of all the marbles */
  long total_weight; 

  std::vector<size_t> alias;
  std::vector<double> prob;

 
public:
  Vose(): total_weight(0L), 
          scores(std::vector<marble> ()),
          alias(std::vector<size_t> ()),
          prob(std::vector<double> ()),
          random_scores_die(std::function<double()> ()),
          random_prob_die(std::function<double()> ()) { }

  /**
    * Adds a new marble(label, weight) to the Urn.
    * It updates all necessary internal structures.
    * Returns the new contribution of te added marble.
    */
  double add(marble m);
  double add(std::string label, long weight);

  /**
    * Runs vose algorithm over the whole Urn of marbles.
    */
  void init();

  /**
    * Make a random from from vose.
    * Vose must be initialized with init.
    */
  size_t rand();

  /**
    * Call rand() but return the actual marble
    */
  marble rand_marble();

  /**
    * Merge this Vose structure with another and return the result.
    */
  Vose merge(const Vose& vose);
  Vose merge(const Vose &v1, const Vose &v2);

  /**
    * Given a key, find this marble and update the value. O(n)
    * If new_val is zero, remove it.
    */
  //virtual Vose update(std::string key, long new_val); 

  // Utility Methods -----------------------------------------
  /**
    * Compares two distributions of values
    */
  static double kl(const std::vector<double> &a, const std::vector<double> &b);

  /**
    * Return a vector of the relative probabilities of the scores.
    */ 
  inline std::vector<double> probabilities() {
    std::vector<double> v;
    std::for_each (scores.begin(), scores.end(), [&] (marble m) {
        v.push_back((double) m.weight/total_weight);
    });
    return v;
  }

  /**
    * Use the random int generator to generate a random in in the
    * range [0, scores.size()).
    */
  inline int random_scores_index() {
    return floor(random_scores_die());
  } 
  inline double random_scores_double() {
    return random_scores_die();
  }
  void reset_scores_die() {
    // Update the random number generators
    std::default_random_engine generator(VoseUtil::RANDOM_SEED); 
    std::uniform_real_distribution<double>
      real_distribution(0, scores.size()>0?scores.size():1);

    random_scores_die = std::bind(real_distribution, generator);
  }
    
  /**
    * Return a double from a uniform distribution in the range [0.0, 1.0].
    */
  inline double random_double() {
    return random_prob_die();
  }
  void reset_random_double() {
    // Update the random number generators
    std::default_random_engine generator(VoseUtil::RANDOM_SEED);
    std::uniform_real_distribution<double> uniform_distribution(0.0,1.0);

    random_prob_die = std::bind(uniform_distribution, generator);
  }
  
  long weight() { return total_weight; }

  std::vector<marble> get_scores() const { return scores;}

  std::string toString();
};


#endif // _VOSE_H_
