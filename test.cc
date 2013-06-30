// test.cc

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <map>

#include "test.h"
#include "Vose.h"
#include "VoseTree.h"


bool test::test1() {
  std::cerr << "\nAdd several marbles, return correct weight and size.\n";
  
  Vose vose;

  std::cerr << "\tAdd m('red',5): ";
  double red_score = vose.add("red", 5);
  std::cerr << red_score << "\n";
  assert(red_score == 1);

  std::cerr << "\tAdd m('blue',10): ";
  std::cerr << vose.add("blue", 10) << "\n";

  std::cerr << "\tAdd m('yellow',20): ";
  double yellow_score = vose.add("yellow", 20);
  std::cerr << yellow_score << "\n";
  assert(yellow_score == 20.0/(5+10+20));

  return true;
}


bool test::test2() {
  std::cerr << "Check that the vose data structure comes close to actual values.\n";

  Vose vose;
  vose.add("red", 5);
  vose.add("blue", 10);
  vose.add("yellow", 20);

  vose.init();
  
  std::cerr << "Vose Dist: ";
  for (size_t i = 0; i != vose.scores.size(); ++i) {
    std::cerr << "\t" << i << "-->" << vose.scores[i].weight << "\n";
  }
  std::cerr << "\n";
  for (size_t i = 0; i != vose.probabilities().size(); ++i) {
    std::cerr << "\t" << i << "-->" << vose.probabilities()[i] << "\n";
  }
  std::cerr << "\n";

  std::cerr << "Create a histogram by sampling the vose structure.\n";
  std::map<size_t,int> histogram;
  auto hist_sum = 0L;
  for (int i = 0; i != 50000; ++i) {
    size_t idx = vose.rand();

    if (histogram.find(idx) != histogram.end()) {
      histogram[idx] += 1;
      hist_sum += 1L;
    }
    else {
      histogram.insert(std::make_pair(idx, 1));
      hist_sum += 1L;
    }
  }

  std::cerr << "Sampled Distribution:\n";

  // Create the new dist
  std::vector<double> new_dist;
  for (auto& h : histogram) {
    std::cerr << "\t" << h.first <<  "-->" << (double)h.second/hist_sum << "\n";
    new_dist.push_back((double)h.second/hist_sum);
  }

  double kl_result = Vose::kl(vose.probabilities(), new_dist); 
  std::cerr << "kl(probabilities, new_dist): " << kl_result << "\n";
  std::cerr << "kl(new_dist, probabilities): " << Vose::kl(new_dist, vose.probabilities()) << "\n";

  return true;
}


bool test::test3() {

  // Create a VoseTree structure
  VoseTree tree;
  tree.set_max_marbles(4);

  // Create new marbles and add them to the Vose tree
  std::cerr << tree.toString();
  tree.add("red", 5L);
  std::cerr << tree.toString();
  tree.add("blue", 10L);
  std::cerr << tree.toString();
  tree.add("yellow", 20L);
  std::cerr << tree.toString();

  tree.add("red", 5L);
  std::cerr << tree.toString();
  tree.add("blue", 10L);
  std::cerr << tree.toString();
  tree.add("yellow", 20L);
  std::cerr << tree.toString();

  // Sample from the tree with and save the results to a histogram
  std::map<std::string,double> histogram;
  long hist_sum = 0L;
  for (int i = 0; i != 50000; ++i) {
    auto idx = tree.rand_marble();

    if (histogram.find(idx.label) != histogram.end()) {
      histogram[idx.label] += 1.0;
      hist_sum += 1L;
    }
    else {
      histogram.insert(std::make_pair(idx.label, 1.0));
      hist_sum += 1L;
    }
  }
  
  std::map<std::string,double> new_hist;
  std::for_each(histogram.cbegin(), histogram.cend(),
    [&new_hist,hist_sum] (std::pair<const std::string, double> a) {
      //std::cerr << a.first << ", " << (double)a.second/hist_sum << "\n";
      new_hist.insert( std::make_pair(a.first,(double)a.second/hist_sum));
  });

  std::map<std::string, double> exact_prob = tree.probabilities();
 
  // Compare the results with kl divergence
  double kl1 = VoseTree::kl(new_hist, exact_prob);
  double kl2 = VoseTree::kl(exact_prob, new_hist);
  std::cerr << "kl(histogram, exact_prob): " << kl1 << "\n";
  std::cerr << "kl(exact_prob, histogram): " << kl2 << "\n";
  
  return true;
}


bool test::test4() {

  VoseTree tree;
  tree.set_max_marbles(4);
  tree.set_max_bags(5);

  std::cerr << tree.toString();
  for(size_t i = 0; i != 10; ++i) {
    tree.add("red", 5L);
    std::cerr << tree.toString();
    tree.add("blue", 10L);
    std::cerr << tree.toString();
    tree.add("yellow", 20L);
    std::cerr << tree.toString();
  }

  return true;
}


bool test::test5() {

  VoseTree tree;
  tree.set_max_marbles(4);
  tree.set_max_bags(5);

  // Generate a set of label names.
  std::string labels[7] = {"red", "yellow", "pink", "green", "purple", 
                           "orange", "blue"};

  // Make a random number generator for marble labels
  std::default_random_engine label_generator(VoseUtil::RANDOM_SEED); 
  std::uniform_int_distribution<size_t>
    label_distribution(0, 7-1);
  std::function<size_t()> random_label =
    std::bind(label_distribution, label_generator);
  
  // Make a random number generator for marble weights
  std::default_random_engine weight_generator(VoseUtil::RANDOM_SEED); 
  std::uniform_int_distribution<long>
    weight_distribution(1, 1000);
  std::function<long()> random_weight =
    std::bind(weight_distribution, weight_generator);

  for(size_t i = 0; i != 50; ++i) {
    // Get a random marble with random label and random weight.
    size_t label_idx = random_label();
    size_t weight = random_weight();
    //std::cerr << "label_idx: " << label_idx << ", " << labels[label_idx] << "\n";
    //std::cerr << " weight: " << weight << "\n";
    tree.add(std::string(labels[label_idx]), weight);
    std::cerr << tree.toString();
  }

  // Generate a histogram.
  auto hist = VoseTree::histogram(tree, 50000L);

  // Get the real probabilities
  auto prob = tree.probabilities();

  // Compare the results
  double kl1 = VoseTree::kl(hist, prob);
  double kl2 = VoseTree::kl(prob, hist);
  std::cerr << "kl(hist, prob): " << kl1 << "\n";
  std::cerr << "kl(prob, hist): " << kl2 << "\n";
  return true;
}


bool test::test6() {

  VoseTree tree;
  tree.set_max_marbles(4);
  tree.set_max_bags(5);

  // Generate a set of label names.
  std::string labels = "red yellow blue";

  // Make a random number generator for marble weights
  std::default_random_engine weight_generator(VoseUtil::RANDOM_SEED); 
  std::uniform_int_distribution<long>
    weight_distribution(1, 1000);
  std::function<long()> random_weight =
    std::bind(weight_distribution, weight_generator);

  // FIXME why does 280 node work? Where is the error, need more stats
  for(size_t i = 0; i != 279; ++i) {
    // Get a random marble with random label and random weight.
    size_t weight = random_weight();
    //std::cerr << "label_idx: " << label_idx << ", " << labels[label_idx] << "\n";
    //std::cerr << " weight: " << weight << "\n";
    std::random_shuffle(labels.begin(), labels.end());
    tree.add(labels, weight);
    //std::cerr << tree.toString();
  }

  // Generate a histogram.
  auto hist = VoseTree::histogram(tree, 50000L);

  // Get the real probabilities
  auto prob = tree.probabilities();

  // Compare the results
  double kl1 = VoseTree::kl(hist, prob);
  double kl2 = VoseTree::kl(prob, hist);
  std::cerr << "kl(hist, prob): " << kl1 << "\n";
  std::cerr << "kl(prob, hist): " << kl2 << "\n";
  return true;
}


bool test::test7() {

  std::cerr << "Need to make sure each branch is being selected.\n";

  VoseTree tree;
  tree.set_max_marbles(0);
  tree.set_max_bags(4);

  tree.add("red", 5);
  tree.add("blue", 5);
  tree.add("yellow", 5);
  std::cerr << tree.toString();

  for( size_t i = 0; i != 6; ++i) {
    std::cerr << ">>> " << tree.rand_marble().label << "\n";
  }

  return true;
}


int main(int argc, char **argv) {
  
  test t;
  
  std::cout << "\nTest 1" << "----------------------------------------\n";
  t.test1();
  
  std::cout << "\nTest 2" << "----------------------------------------\n";
  t.test2();
  
  std::cout << "\nTest 3" << "----------------------------------------\n";
  t.test3();
  
  std::cout << "\nTest 4" << "----------------------------------------\n";
  t.test4();
  
  std::cout << "\nTest 5" << "----------------------------------------\n";
  t.test5();

  std::cout << "\nTest 6" << "----------------------------------------\n";
  t.test6();

  std::cout << "\nTest 7" << "----------------------------------------\n";
  t.test7();


 return 0;
}

