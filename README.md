
# Streaming Vose implementation

## How to

This Requires gcc version 4.5 or higher and Make. This uses the c++11 lambdas.
Compile the code and run test cases:

    make clean; make test
    ./test

## Vose Algorithm

Vose algorithm is of of the best methods of sampling from a discreet distributon. For a
discussion on Vose, see the blog post by Keith Schwarz entited 
[Darts, Dice, and Coins: Sampling from a Discrete Distribution]
(http://www.keithschwarz.com/darts-dice-coins/).
Vose Alias algorithm is based on a paper by Michael D. Vose entitled 
[A Linear Algorithm For Generating Random Numbers with a Given Distribution]
(http://web.eecs.utk.edu/~vose/Publications/random.pdf).

This algorithm allows building such a data structure in Θ(n) time and space
such that random sampling from this data structure can be done in constant time.
In this case the distributon of probabilities is given a priori.


## Streaming Vose

In our case, we sample items from a distribution that is constantly changing.
For example, given a jar of marbles. We would like to simulate removing marbles
from the jar. Each color we choose should selection with a probability that is 
proportional to the distribution of marbles in the jar. Vose allows use to build
a structure that simulates this very nicely.

Now imaging while we are trying to select from the marbles, someone is puring 
more adding marbles to the jar. Trying to rebuild a data structure each time
is cumberson and slow. With Streaming Vose we aim for simple additions to the 
data structure to allow fast streaming updates while still supporting constant
time random selections.


## Architecture 

To implement streaming vose we create a tree with only one level. 
At this leaf level we have nodes that may be bags of marbles or individual marbles.
The branches at each node is weighted by the size of the marble or the weight of 
the bag. Each bag of marbles is a vose alias data structure.


## Algorithms

#### rand()
This function is in both the Vose structure and the VoseTree structure.
In the Vose structure, when called it returns an index to a marble in proportion
to its weight. In the VoseTree method it does the same. Here is some pseudocode
for the VoseTree method.

```python
def rand():
  # Select a random real value in the range of the total weight
  rand_val = random_branch()

  # Select the branch that falls in this range
  branch = floor(binary_search(branches, rand_val))

  if nodes[branch] is bag:
    return nodes[branch].rand()
  else:
    return nodes[branch].ball
```

The random_branch function does a selection of the branches by the weight of each one.
The binary search method on this structure makes each rand() call Θ(log b) where `b` is the number
of branches. So it is ideal to keep the number of branches as small as possible.
If a bag is return or a ball is return it is performed in constant time.

#### add()
The add function adds a single new weighted marble to the set. When a marble is added, a weighted
branch is added to the set. The random branch selector is also updates with the new branch.
By adding the marble, if a threshold of too many marble is reached, all the free marbles in the
collection are compressed into a new bag. This takes O(m) time where `m` is the number of
free marbles. If adding a new bag means there are too many bags, then all the bags are compressed
into one large bag. Recall, each bag is a Vose structure.


## Parameters

Choosing two parameters, _max_bags_ and _max_marbles_ is important for the performance of the structure.
We would like to chose value for these parameters such that that accessing the extra bags and balls is
identical to the cost of keeping one. This property is discussed in the paper `A Novel Index Supporting
High Volume Data Warehouse Insertions.`

## Proofs

How do we know that each marball is selected in proportion to its weight? Lets check with the probability
of selecting an item.

The probability of selecting a branch b_i is the weight of the branch over the total weight of 
all the branches. That is: p(b_i) = b_i / (\Sigma_i b_i).

Attached to each of the branches are either free marbles or bags of marbles.
The probability of selecting a free marble is the probability of selecting that marbles branch
multiplied by 1, because there is no other decision to make once that free matble is selected.
That is P(free_marble) = p(m_b_i) * 1.

The probability of selecting a marble from inside a bag is the probability of selecting the bag times
the probability of selecting the ball inside the bag. Let m_b_i be the marble inside a bag. Therefore,
P(marball in a bag) = p(m_b_i | b_i) = (m_b_i / b_i) * (b_i / \Sigma_i b_i). We can cancel out the cost
of the branches and we get  (m_b_i / \Sigma_i b_i), this is the exact cost of one marble. This validates
our tree structure.

This is the proof for a tree of height one. A similar proof may work for the case of a tree high greater 
than one.


## TODO 

See [Issues](https://github.com/cegme/vose/issues).

## Related Papers

    @inproceedings{Jermaine:2004:OMV:1007568.1007603,
     author = {Jermaine, Christopher and Pol, Abhijit and Arumugam, Subramanian},
     title = {Online Maintenance of Very Large Random Samples},
     booktitle = {Proceedings of the 2004 ACM SIGMOD International Conference on Management of Data},
     series = {SIGMOD '04},
     year = {2004},
     isbn = {1-58113-859-8},
     location = {Paris, France},
     pages = {299--310},
     numpages = {12},
     url = {http://doi.acm.org/10.1145/1007568.1007603},
     doi = {10.1145/1007568.1007603},
     acmid = {1007603},
     publisher = {ACM},
     address = {New York, NY, USA},
    } 
----
    @inproceedings{Jermaine:1999:NIS:645925.671517,
     author = {Jermaine, Chris and Datta, Anindya and Omiecinski, Edward},
     title = {A Novel Index Supporting High Volume Data Warehouse Insertion},
     booktitle = {Proceedings of the 25th International Conference on Very Large Data Bases},
     series = {VLDB '99},
     year = {1999},
     isbn = {1-55860-615-7},
     pages = {235--246},
     numpages = {12},
     url = {http://dl.acm.org/citation.cfm?id=645925.671517},
     acmid = {671517},
     publisher = {Morgan Kaufmann Publishers Inc.},
     address = {San Francisco, CA, USA},
    } 
----
    @article{Vose:1991:LAG:126262.126280,
     author = {Vose, Michael D.},
     title = {A Linear Algorithm for Generating Random Numbers with a Given Distribution},
     journal = {IEEE Trans. Softw. Eng.},
     issue_date = {September 1991},
     volume = {17},
     number = {9},
     month = sep,
     year = {1991},
     issn = {0098-5589},
     pages = {972--975},
     numpages = {4},
     url = {http://dx.doi.org/10.1109/32.92917},
     doi = {10.1109/32.92917},
     acmid = {126280},
     publisher = {IEEE Press},
     address = {Piscataway, NJ, USA},
     keywords = {arbitrary probability distribution, finite set, genetic algorithms, linear algorithm, probability, random number generation, random numbers, random variable, simple genetic algorithm},
    }
----
    @article{goldberg1991comparative,
     title={A comparative analysis of selection schemes used in genetic algorithms},
     author={Goldberg, David E and Deb, Kalyanmoy},
     journal={Urbana},
     volume={51},
     pages={61801--2996},
     year={1991}
    }


