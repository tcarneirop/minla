#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdlib.h>
#include <algorithm>

using namespace std;

class Solution
{
public:   
    int* tag;
    int cost;
    int size;

    Solution(){

    }

    Solution(int tag_solution[], int cost_solution, int size_solution){
        size = size_solution;
        tag = new int[size];
        cost = cost_solution;
        std::copy(tag_solution, tag_solution + size, tag); //copy tag_solution to tag
    }

    bool operator==(const Solution& sol) const
    {

          if(cost != sol.cost)
              return false;

          if(!std::equal(tag, tag+ size, sol.tag, [](int a, int b)->bool{return a == b;}))
              return false;

          return true;
    }

    bool operator!=(const Solution& sol) const
    {

          if(cost != sol.cost)
              return true;

          if(!std::equal(tag, tag+ size, sol.tag, [](int a, int b)->bool{return a == b;}))
              return true;

          return false;
    }

    bool operator <(const Solution& sol) const
    {
        return cost < sol.cost;
    }

    int distanceTo(Solution sol)
    {
          int c = 0;
          for(int i = 0; i < size; i++)
          {
               c += abs( tag[i] - sol.tag[i]);
          }

          return c;
    }

    void vertice(int vertice[])
    {
        for(int i = 0; i < size; i++ )
        {
            vertice[tag[i]] = i;
        }
    }


};

#endif // SOLUTION_H
