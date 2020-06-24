/*
 *
 * Murat Yirci - Copyright 2020
 *
 * Contact: myirci@gmail.com
 *
 *
*/

#include "EightNumber.hpp"
#include "../utility/Utility.hpp"

#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <queue>
#include <stack>
#include <utility>

// construct the connectivity graph
const std::array<std::array<unsigned char, 9>,9> EightNumber::TileGraph =
{{
     {0,1,0,1,0,0,0,0,0}, // 0
     {1,0,1,0,1,0,0,0,0}, // 1
     {0,1,0,0,0,1,0,0,0}, // 2
     {1,0,0,0,1,0,1,0,0}, // 3
     {0,1,0,1,0,1,0,1,0}, // 4
     {0,0,1,0,1,0,0,0,1}, // 5
     {0,0,0,1,0,0,0,1,0}, // 6
     {0,0,0,0,1,0,1,0,1}, // 7
     {0,0,0,0,0,1,0,1,0}  // 8
 }};

EightNumber::EightNumber(Board&& b)
{
    m_board = std::move(b);
}

EightNumber::EightNumber(const Board& b)
{
    std::copy(b.begin(), b.end(), m_board.begin());
}

void EightNumber::SetBoard(Board&& b)
{
    m_board = std::move(b);
}

void EightNumber::SetBoard(const Board& b)
{
    std::copy(b.begin(), b.end(), m_board.begin());
}

size_t EightNumber::GetPosition(uint8_t val) const
{
    return Utility::GetPosition(m_board, val);
}

const EightNumber::Board& EightNumber::GetBoard() const
{
    return m_board;
}

uint8_t EightNumber::GetVal(size_t pos) const
{
    return m_board[pos];
}

bool EightNumber::UpdateBoard(size_t pos)
{
    for(auto i = 0; i < 9; i++)
    {
        if(TileGraph[pos][i])
        {
            if(m_board[i] == 0)
            {
                m_board[i] = m_board[pos];
                m_board[pos] = 0;
                return true;
            }
        }
    }
    return false;
}

void EightNumber::Shuffle()
{
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    do
    {
        std::shuffle(m_board.begin(), m_board.end(), generator);
    } while(!Utility::IsSolvable(m_board));
}

bool EightNumber::SolveBFS(Moves& moves, bool graphSearch) const
{
    return graphSearch
            ? SolveBFS_GraphSearch(moves)
            : SolveBFS_TreeSearch(moves);
}

bool EightNumber::SolveBFS_TreeSearch(Moves& moves) const
{
    bool solved{false};
    long num_expanded_nodes{0};
    int depthLimit{16};

    std::queue<BoardAndMoves> nodes;
    nodes.push(BoardAndMoves(m_board, Moves()));
    while(!nodes.empty())
    {
        BoardAndMoves& currentNode = nodes.front();
        if(currentNode.second.size() > depthLimit)
        {
            break;
        }

        if(Utility::IsSolved(currentNode.first))
        {
            std::copy(currentNode.second.begin(), currentNode.second.end(), std::back_inserter(moves));
            solved = true;
            break;
        }

        auto emptyPos = Utility::GetPosition(currentNode.first, 0);
        for(size_t i = 0; i < 9; i++)
        {
            if(TileGraph[emptyPos][i] == 0)
            {
                continue;
            }

            // update the current board
            currentNode.first[emptyPos] = currentNode.first[i];
            currentNode.first[i] = 0;
            currentNode.second.push_back(currentNode.first[emptyPos]);

            // add new board to the queue
            nodes.push(currentNode);

            // restore the current board
            currentNode.first[i] = currentNode.first[emptyPos];
            currentNode.first[emptyPos] = 0;
            currentNode.second.pop_back();
        }
        num_expanded_nodes++;
        nodes.pop();
    }

    std::cout << "SolveBFS_TreeSearch: number of expanded nodes is " << num_expanded_nodes << std::endl;
    std::cout << "SolveBFS_TreeSearch: number of nodes remained in the queue is " << nodes.size() << std::endl;

    return solved;
}

bool EightNumber::SolveBFS_GraphSearch(Moves& moves) const
{
    bool solved{false};
    long num_expanded_nodes{0};

    std::queue<BoardAndMoves> nodes;
    nodes.emplace(BoardAndMoves(m_board, Moves()));
    HashSet hashSet{Utility::GetBoardAsUint(m_board)};

    while(!nodes.empty())
    {
        BoardAndMoves& currentNode = nodes.front();
        if(Utility::IsSolved(currentNode.first))
        {
            std::copy(currentNode.second.begin(), currentNode.second.end(), std::back_inserter(moves));
            solved = true;
            break;
        }

        auto emptyPos = Utility::GetPosition(currentNode.first, 0);
        for(auto i = 0; i < 9; i++)
        {
            if(TileGraph[emptyPos][i] == 0)
            {
                continue;
            }

            // update the board of the current node
            currentNode.first[emptyPos] = currentNode.first[i];
            currentNode.first[i] = 0;

            // if the current node has not been processed/explored/visited/expanded before
            if(hashSet.insert(Utility::GetBoardAsUint(currentNode.first)).second)
            {
                // update the move of the current board and push it to the queue
                currentNode.second.push_back(currentNode.first[emptyPos]);
                nodes.push(currentNode);

                // take back the move
                currentNode.second.pop_back();
            }

            // restore the board
            currentNode.first[i] = currentNode.first[emptyPos];
            currentNode.first[emptyPos] = 0;
        }

        nodes.pop();
        num_expanded_nodes++;
    }

    std::cout << "SolveBFS_GraphSearch: number of expanded nodes is " << num_expanded_nodes << std::endl;
    std::cout << "SolveBFS_GraphSearch: number of nodes remained in the queue is " << nodes.size() << std::endl;
    return solved;
}

bool EightNumber::SolveNonRecursiveDFS(Moves& moves) const
{
    bool solved = false;
    long num_expanded_nodes{0};
    std::stack<BoardAndMoves> nodes;
    BoardAndMoves currentNode;
    HashSet hashSet;

    nodes.push(BoardAndMoves(m_board, Moves()));
    hashSet.insert(Utility::GetBoardAsUint(m_board));

    while(!nodes.empty())
    {
        if(Utility::IsSolved(nodes.top().first))
        {
            std::copy(nodes.top().second.begin(), nodes.top().second.end(), std::back_inserter(moves));
            solved = true;
            break;
        }

        currentNode = nodes.top();
        nodes.pop();
        num_expanded_nodes++;

        auto emptyPos = Utility::GetPosition(currentNode.first, 0);
        for(auto i = 0; i < 9; i++)
        {
            if(TileGraph[emptyPos][i] == 0)
            {
                continue;
            }

            // update the board of the current node
            currentNode.first[emptyPos] = currentNode.first[i];
            currentNode.first[i] = 0;

            // if the current node has not been explored/visited/processed before
            if(hashSet.insert(Utility::GetBoardAsUint(currentNode.first)).second)
            {
                // update the move of the current board and push it to the stack
                currentNode.second.push_back(currentNode.first[emptyPos]);
                nodes.push(currentNode);

                // take back the move
                currentNode.second.pop_back();
            }

            // restore the board
            currentNode.first[i] = currentNode.first[emptyPos];
            currentNode.first[emptyPos] = 0;
        }
    }

    std::cout << "SolveNonRecursiveDFS: number of expanded nodes is " << num_expanded_nodes << std::endl;
    std::cout << "SolveNonRecursiveDFS: number of nodes remained in the queue is " << nodes.size() << std::endl;

    return solved;
}

bool EightNumber::SolveDepthLimitedRecursiveDFS(Moves& moves, bool withHash) const
{
    int depthLimit{31};
    auto board = m_board;
    if(withHash)
    {
        HashMap hashMap;
        return DepthLimitedRecursiveDFS_GraphSearch(board, hashMap, moves, depthLimit);
    }

    return DepthLimitedRecursiveDFS_TreeSearch(board, moves, depthLimit);
}

bool EightNumber::SolveIterativeDeepening(Moves& moves, bool withHash) const
{
    int maxDepth = 31;
    int depth = 1;
    auto board = m_board;

    while(depth <= maxDepth)
    {
        if(withHash)
        {
            HashMap hashMap;
            auto ret = DepthLimitedRecursiveDFS_GraphSearch(board, hashMap, moves, depth);
            if(ret)
            {
                return true;
            }
        }
        else
        {
            if(DepthLimitedRecursiveDFS_TreeSearch(board, moves, depth))
            {
                return true;
            }
        }

        depth++;
    }

    return false;
}

bool EightNumber::DepthLimitedRecursiveDFS_TreeSearch(Board& board, Moves& moves, int depth) const
{
    if(Utility::IsSolved(board))
    {
        return true;
    }
    else if(depth == 0)
    {
        return false;
    }

    auto emptyPos = Utility::GetPosition(board, 0);
    for(auto i = 0; i < 9; i++)
    {
        if(TileGraph[emptyPos][i] == 0 || (!moves.empty() && board[i] == moves.back()))
        {
            continue;
        }

        // make the move
        board[emptyPos] = board[i];
        board[i] = 0;
        moves.push_back(board[emptyPos]);

        // investigate children
        if(DepthLimitedRecursiveDFS_TreeSearch(board, moves, depth - 1))
        {
            return true;
        }

        // undo the move
        board[i] = board[emptyPos];
        board[emptyPos] = 0;
        moves.pop_back();
    }

    return false;
}

bool EightNumber::DepthLimitedRecursiveDFS_GraphSearch(Board& board, HashMap& hashMap, Moves& moves, int depth) const
{
    if(Utility::IsSolved(board))
    {
        return true;
    }
    else if(depth == 0)
    {
        return false;
    }

    auto emptyPos = Utility::GetPosition(board, 0);
    for(auto i = 0; i < 9; i++)
    {
        if(TileGraph[emptyPos][i] == 0)
        {
            continue;
        }

        // make the move
        board[emptyPos] = board[i];
        board[i] = 0;
        moves.push_back(board[emptyPos]);

        // explore the next boards
        auto key = Utility::GetBoardAsUint(board);
        auto it = hashMap.find(key);
        if(it == hashMap.end())
        {
            hashMap.insert({key, depth});
            if(DepthLimitedRecursiveDFS_GraphSearch(board, hashMap, moves, depth - 1))
            {
                return true;
            }
        }
        else
        {
            if(depth > it->second)
            {
                it->second = depth;
                if(DepthLimitedRecursiveDFS_GraphSearch(board, hashMap, moves, depth - 1))
                {
                    return true;
                }
            }
        }

        // undo the move
        board[i] = board[emptyPos];
        board[emptyPos] = 0;
        moves.pop_back();
    }

    return false;
}

bool EightNumber::SolveGreedySearch(Moves& moves) const
{
    return false;
}

bool EightNumber::SolveAStar(Moves& moves) const
{
    return false;
}
