/*
 *
 * Murat Yirci - Copyright 2020
 *
 * Contact: myirci@gmail.com
 *
 *
*/

#pragma once

#include <memory>
#include <string>
#include <array>
#include <vector>
#include <unordered_map>

struct Node
{
    Node(const std::array<uint8_t, 9>& b, int dtg, int mtg);
    std::array<uint8_t, 9> board;
    int dist_to_goal;
    int move_to_goal;
    std::vector<unsigned int> successors;
};

class StateSpaceGraph
{
private:
    std::unique_ptr<std::unordered_map<unsigned int, Node>> m_snodes;
    std::unique_ptr<std::unordered_map<unsigned int, Node>> m_wnodes;
public:
    StateSpaceGraph();
    void ComputeStandardEightPuzzleStateSpaceGraph();
    void ComputeWeightedEightPuzzleStateSpaceGraph();
    int ExportStandardEightPuzzleStateSpaceGraph(const std::string& fname) const;
    int ExportWeightedEightPuzzleStateSpaceGraph(const std::string& fname) const;
};